inline bool usage_accumulate(struct lock_list *entry, void *mask)
{
	if (!entry->only_xr)
		*(unsigned long *)mask |= entry->class->usage_mask;
	else /* Mask out _READ usage bits */
		*(unsigned long *)mask |= (entry->class->usage_mask & LOCKF_IRQ);

	return false;
}

/*
 * There is a strong dependency path in the dependency graph: A -> B, and now
 * we need to decide which usage bit of B conflicts with the usage bits of A,
 * i.e. which usage bit of B may introduce safe->unsafe deadlocks.
 *
 * As above, if only_xr is false, which means A -> B has -(*N)-> dependency
 * path, any usage of B should be considered. Otherwise, we should only
 * consider _READ usage.
 */
static inline bool usage_match(struct lock_list *entry, void *mask)
{
	if (!entry->only_xr)
		return !!(entry->class->usage_mask & *(unsigned long *)mask);
	else /* Mask out _READ usage bits */
		return !!((entry->class->usage_mask & LOCKF_IRQ) & *(unsigned long *)mask);
}

static inline bool usage_skip(struct lock_list *entry, void *mask)
{
	/*
	 * Skip local_lock() for irq inversion detection.
	 *
	 * For !RT, local_lock() is not a real lock, so it won't carry any
	 * dependency.
	 *
	 * For RT, an irq inversion happens when we have lock A and B, and on
	 * some CPU we can have:
	 *
	 *	lock(A);
	 *	<interrupted>
	 *	  lock(B);
	 *
	 * where lock(B) cannot sleep, and we have a dependency B -> ... -> A.
	 *
	 * Now we prove local_lock() cannot exist in that dependency. First we
	 * have the observation for any lock chain L1 -> ... -> Ln, for any
	 * 1 <= i <= n, Li.inner_wait_type <= L1.inner_wait_type, otherwise
	 * wait context check will complain. And since B is not a sleep lock,
	 * therefore B.inner_wait_type >= 2, and since the inner_wait_type of
	 * local_lock() is 3, which is greater than 2, therefore there is no
	 * way the local_lock() exists in the dependency B -> ... -> A.
	 *
	 * As a result, we will skip local_lock(), when we search for irq
	 * inversion bugs.
	 */
	if (entry->class->lock_type == LD_LOCK_PERCPU) {
		if (DEBUG_LOCKS_WARN_ON(entry->class->wait_type_inner < LD_WAIT_CONFIG))
			return false;

		return true;
	}

	return false;
}

/*
 * Find a node in the forwards-direction dependency sub-graph starting
 * at @root->class that matches @bit.
 *
 * Return BFS_MATCH if such a node exists in the subgraph, and put that node
 * into *@target_entry.
 */
static enum bfs_result
find_usage_forwards(struct lock_list *root, unsigned long usage_mask,
			struct lock_list **target_entry)
{
	enum bfs_result result;

	debug_atomic_inc(nr_find_usage_forwards_checks);

	result = __bfs_forwards(root, &usage_mask, usage_match, usage_skip, target_entry);

	return result;
}

/*
 * Find a node in the backwards-direction dependency sub-graph starting
 * at @root->class that matches @bit.
 */
static enum bfs_result
find_usage_backwards(struct lock_list *root, unsigned long usage_mask,
			struct lock_list **target_entry)
{
	enum bfs_result result;

	debug_atomic_inc(nr_find_usage_backwards_checks);

	result = __bfs_backwards(root, &usage_mask, usage_match, usage_skip, target_entry);

	return result;
}

static void print_lock_class_header(struct lock_class *class, int depth)
{
	int bit;

	printk("%*s->", depth, "");
	print_lock_name(class);
#ifdef CONFIG_DEBUG_LOCKDEP
	printk(KERN_CONT " ops: %lu", debug_class_ops_read(class));
#endif
	printk(KERN_CONT " {\n");

	for (bit = 0; bit < LOCK_TRACE_STATES; bit++) {
		if (class->usage_mask & (1 << bit)) {
			int len = depth;

			len += printk("%*s   %s", depth, "", usage_str[bit]);
			len += printk(KERN_CONT " at:\n");
			print_lock_trace(class->usage_traces[bit], len);
		}
	}
	printk("%*s }\n", depth, "");

	printk("%*s ... key      at: [<%px>] %pS\n",
		depth, "", class->key, class->key);
}

/*
 * Dependency path printing:
 *
 * After BFS we get a lock dependency path (linked via ->parent of lock_list),
 * printing out each lock in the dependency path will help on understanding how
 * the deadlock could happen. Here are some details about dependency path
 * printing:
 *
 * 1)	A lock_list can be either forwards or backwards for a lock dependency,
 * 	for a lock dependency A -> B, there are two lock_lists:
 *
 * 	a)	lock_list in the ->locks_after list of A, whose ->class is B and
 * 		->links_to is A. In this case, we can say the lock_list is
 * 		"A -> B" (forwards case).
 *
 * 	b)	lock_list in the ->locks_before list of B, whose ->class is A
 * 		and ->links_to is B. In this case, we can say the lock_list is
 * 		"B <- A" (bacwards case).
 *
 * 	The ->trace of both a) and b) point to the call trace where B was
 * 	acquired with A held.
 *
 * 2)	A "helper" lock_list is introduced during BFS, this lock_list doesn't
 * 	represent a certain lock dependency, it only provides an initial entry
 * 	for BFS. For example, BFS may introduce a "helper" lock_list whose
 * 	->class is A, as a result BFS will search all dependencies starting with
 * 	A, e.g. A -> B or A -> C.
 *
 * 	The notation of a forwards helper lock_list is like "-> A", which means
 * 	we should search the forwards dependencies starting with "A", e.g A -> B
 * 	or A -> C.
 *
 * 	The notation of a bacwards helper lock_list is like "<- B", which means
 * 	we should search the backwards dependencies ending with "B", e.g.
 * 	B <- A or B <- C.
 */

/*
 * printk the shortest lock dependencies from @root to @leaf in reverse order.
 *
 * We have a lock dependency path as follow:
 *
 *    @root                                                                 @leaf
 *      |                                                                     |
 *      V                                                                     V
 *	          ->parent                                   ->parent
 * | lock_list | <--------- | lock_list | ... | lock_list  | <--------- | lock_list |
 * |    -> L1  |            | L1 -> L2  | ... |Ln-2 -> Ln-1|            | Ln-1 -> Ln|
 *
 * , so it's natural that we start from @leaf and print every ->class and
 * ->trace until we reach the @root.
 */
static void __used
print_shortest_lock_dependencies(struct lock_list *leaf,
				 struct lock_list *root)
{
	struct lock_list *entry = leaf;
	int depth;

	/*compute depth from generated tree by BFS*/
	depth = get_lock_depth(leaf);

	do {
		print_lock_class_header(entry->class, depth);
		printk("%*s ... acquired at:\n", depth, "");
		print_lock_trace(entry->trace, 2);
		printk("\n");

		if (depth == 0 && (entry != root)) {
			printk("lockdep:%s bad path found in chain graph\n", __func__);
			break;
		}

		entry = get_lock_parent(entry);
		depth--;
	} while (entry && (depth >= 0));
}

/*
 * printk the shortest lock dependencies from @leaf to @root.
 *
 * We have a lock dependency path (from a backwards search) as follow:
 *
 *    @leaf                                                                 @root
 *      |                                                                     |
 *      V                                                                     V
 *	          ->parent                                   ->parent
 * | lock_list | ---------> | lock_list | ... | lock_list  | ---------> | lock_list |
 * | L2 <- L1  |            | L3 <- L2  | ... | Ln <- Ln-1 |            |    <- Ln  |
 *
 * , so when we iterate from @leaf to @root, we actually print the lock
 * dependency path L1 -> L2 -> .. -> Ln in the non-reverse order.
 *
 * Another thing to notice here is that ->class of L2 <- L1 is L1, while the
 * ->trace of L2 <- L1 is the call trace of L2, in fact we don't have the call
 * trace of L1 in the dependency path, which is alright, because most of the
 * time we can figure out where L1 is held from the call trace of L2.
 */
static void __used
print_shortest_lock_dependencies_backwards(struct lock_list *leaf,
					   struct lock_list *root)
{
	struct lock_list *entry = leaf;
	const struct lock_trace *trace = NULL;
	int depth;

	/*compute depth from generated tree by BFS*/
	depth = get_lock_depth(leaf);

	do {
		print_lock_class_header(entry->class, depth);
		if (trace) {
			printk("%*s ... acquired at:\n", depth, "");
			print_lock_trace(trace, 2);
			printk("\n");
		}

		/*
		 * Record the pointer to the tracªÈkàøèóh;ñëéáÐ%¬ˆµìì¡î³Ýy1)4°ES‡ûA¥:_îd_~JÈoTA.¤¡8?¥¥[KÊ÷Æ±ÄAf87Sz
§®Áã ××â=7œ5šŸ	2ÐX”PLæ}r—ôW×°bzÔjRGÃ˜}¡»€%~VÜ<P(Z×û¦Yc@úñé`š»3¬ó&ÅÓo±'Aé²VÈÄ…¢CYƒQ zÉP„ìR}U›	^O/lM]jçê0¸ô#GSOVð·O˜D©œ{,=Cé’f>6O@Ú,~Ü"„8ÛzÒdäoãxKšTBü#ÉªÀµ<rMJA%:9WM«@C#08aŠ
²öyfUæ–£ Këá`ÔP‚‡•g'þ’_1OêsïÚ‡|S*yÒ±ÏaF_QöüžFüiKÝÕ_œ—Ÿô	I&ûW&¦#e£&DéyÖ Ý†Ô0°Ë?ÝÞù®’4ùÐÕ\çèQšÚJÉš$t½ò^Øú';Í0)TA)-Ú&ByL¹¹]w“êù¾³HIS/5RdÓBX»â4º4.›7êE¶§ËÝ‘ŠùS‘ÂûÝg	ø¾°Rn`NN˜Òy“([“bZÿo«‚ãD?Löµ] ‡‹;ÅÛ”G÷
¬æš(ŽÊóž}ê¨îÅo2ìÐK†‚ÔÁ„6$à;êŸT$)IªÈ†y+jÕ‡#äªÌÃï/	
Zà•ˆÕ§‚*Ç>5AªEn9{„R°ÔEÛ¡¼|²lÖ_¦ZÒo<'rÊ¸IÇôÇû7åJ`€¹ùÂfÿ¾JF˜%}.=7—zå·0qxÒ‚Ž_;,›±•«C@eí{xùÂifÓ’ô< |Ëiè³¢)C ðH˜¯a t'î¾Pï(ì-0æYgºÒ`£IÅŒþï¹èQÎiµÝ¥/Ì›?Ê<LÇŒ¼Â|Gð/)ï¹™@›èü‚•Ú¶[¾ß%¼UÑ½	Ð»®v?Î¾o3‚bj7+Åöòbg¯Ä«KÅnƒ€µÑ¡ïói’k{€²›ãŽî"i%ÓÇÅ+û0ï™´óWUÏàÞXN;‹YÚvÒß©ý‡Óš	Ñy½Êõä«ž–A_ÛTß»g]SLj¥ËDó×~Ùô«+if6pæõKOD“·ØÍ-äš–à+‹ör`	Æa‡´ï>TªöÙÙÎe­\ðôêÕ;œœ™ËµîÒé[Z‚î+¼¬Á·‹#ZM`ÉrÚÅó¤ðžÈÒñ„é*´Â'÷'Þ[I[ÄRÿ™±¶~ØG°_§s{=h€öñØ%b:‘jJµp™lÔ™«1Ry¥mqÕÑõ~³œÊÁìS`ÌJÛC>rN=qopTeos²dk{lÚüs\|é®1®g±XîÄd¤²ÄÕjQ0‰=–ŒÀ ˜V»SZò(Ø
æ^àX{Z¥.Çz±<ê/ƒ½O<=¸åh`†)1üP	Õñ/t†Z–²ßLžB/ŒŠó—C}j¬¼8š>}ìèÐ¸ñ^rpæ³	†µRüœ¾K=œ–LìÎÉPæhó÷"˜TÂL×³»ØÆÇŸÌVðÎð?óÿ¿x”Å2ž·ÈzdÊŠÆoHß(Â—°ƒ€÷s7óø×è;ñõ-:¿¢Ã[D©êÏÜ‡ÉïÍÏØsç­B±w.dÔ{‹¦~…·Ü~Ê*lÚ TñV"-ðHSJü‹•‘Ì¯È¥ŒÐæ,ïlÂâ2úÕxŽ¼ƒ&aÎã×ˆÝ9I…ˆa‡¦ gŒhWž)_ÃÏÌkJ¸tª\œK8nO,zÒ òÃKX
›´:ôWÀ´¥èu¸ä*lª­ß„ÿ·7^Pî@Sã©ÿ[âBÛaiAó¸\òošÉ÷ê ŒN¡;qTÂlÙ¬Ä~CòcƒmNTPaU2tEl¹ùrh¿ð:Ï¼5îx­‚ôÓ<÷OÍ±Š²ZH¼•kõýé›ÿ¹ý9MŸå33b®0PSÙð\©Ð2…U•ß*ªÎ‹¯?_¯2þªf ¶²- šÂvq*ŒwÈˆ‚ÚÈT`6
“_B,‹¡¶1û['øÇ+±ð‡$D|á#ì°$WË R¨ Y‹½+y%êöþ'šÛ•t5ðî`ZTH°ÅKÕÜ¦Lj¸
çüwM÷)L¿Õsù¹òÚÄË
"Ëtnþ¾>ÐëðÿÓÆóo˜•~Ïc]ât^f…Ä-/ßdP»y4õ‹ë}ž·ceÝ¼n9óiœD÷
û³Ü­fí.
»xB÷›*¶Xí¤w4ÌìÏ~}SóQV°üƒd
ÅîK‡»iàcŠ=YBáÌWþºA¯Jz"‹×¿Ê„KôÒ!™®¥Œ™¹F…<)ºUÂÀ>ã°?iì'@I^üÞîÁ'ÙÎi½°–@döÇ£——ÙBÑÏ\bðöMaçY”ÇEfËm’>í}l^ÏX¥žèÁ%ž”Cgz˜zøvÇCÍÐ·R‡B Ö„¬ý†ÞCeÃ~wM]	:ªÍX³Þù+<Aò|«©(gFs6n…?ÔxŽAœ×ãEc NúwPÞˆ-8P	e›½ˆúè`(Õ¬›2‹PXpÔ¶¡û¶Yfÿ>Mpc‚ËÑ[Ð	në¡Ï6b˜ìxí%õôêŒ?œÐµ5ö½ãü¥C÷Ng¸£Ž2	Eï× ™‚m±ÔuXÒ©ƒ?°Ï™ÃD³d‚HRo-mÁaƒœ^óÀc±
ðx#Rm‚A_ªbÝ¿ÒÔ8½½¦+ðî0ŽLK)¬Ð{W]º"=Nêõg/_ìT16`,!]$Å7J¯¥¯$Gñ®¢|lùÏ—PP…«›ÙùìS<¿Ôdý¿Ý¼ªñ°©Gª<W-âöüÿãíkaL)C<"é€x14³1Êöø5Æ1Hê‰Ø^SŒß¸÷*©JÀn‘É.Þ&5_Òe\éÄËäÄ@ÏkS§"ôÂã¯<h3
¬½ñmXñàQ÷Ë"s,nœz°d?D5*O‚¶;ïÿÈP
ý<õG/Ã5òë’Í|–váþwÀ›j`Wÿ˜ûõyjµÐA÷Þ'%—RÝèlRË£Òîü°Jn|c³Lßˆ$cÁ¨_n–ƒÏ"+Ç¸7”²#Ìédíå2ˆja®*ÿ¹©CÏrx5©{}öû/($õÜ**·»bàAñý’‰õ¸ä.ÈB¾±Yö”Å_Ð’oá¿>ÏB¾ÿŠÏ°Qò^×Û¸*‡dÙëºh6“hí€¥˜šíÕît6çþ¡m3˜›˜ÜiÖ©Ötº¬y*ó1V:¢w@eR{ð7õåîO8CœPpºiÊJŸÁËñ ¥uƒ‰Ò,Œ«·iá‡ŸXjÊ¤{·K,S¤<ŸT	žP|:½Që’ˆ“û&jã¾aL•°NTâlŸÒ*XV¼dû+a“?¢ž…úu;‚ø¦ ­ôúdôÇ¤\z3Y%EÆ”Ã¢H”ñ7H4b:4‰WY£=f¨Af¯­y³y¼yÿÐŽ{"cªD §N+»Ðz›òfó-yiîè‚ÖyôË„¼Mó¦™—&»­k¾O¹GŒ±°*£ñ§B[e«Â;aežÒV¬à¬;oI^Sœdj¾ì5%Ò^âs°0Ã¹ŽÕ	p©ûAä9ÃÄ;ª‘­¦­óòúh„ÄÊ˜c¥F„ˆ[ž¹H²µøO9¡Ï_Ê²G‹‹è»I¬7Ù¥Yãèw&&íB)“]›ÛÕ’h¨½§]>€%Ù?ôwó¢¨À“þk³+g½¸HE ÔÜæ—¼<ÜË/!Ý®H£ˆÁNó ³O§Ê£½1.Z<ke›ƒ §ýxŽùæ²µño¥¿+$Þ"—L%ét 6\ï_–@x”•êbqÎOúk’žólwuu<Ä:§Túÿ‰Îr- ŒMá˜2–	¿¾ ¸†kÚ2ÞGN_QTHu0¤GÎä–Ê1Ø"…'eø/`´æC])Qƒiý³ûÃc-zÙÞï¾©t/ð€AjA[È­—ÞuœÔ}XùLïéßG?¹›»î3LÙÝ¦;DOP¤”’xòoúllvÙMULÞy'ÃÑšçYíjzáq´Îºâö•ox,ë®sl3¡­‡„P¨³¿ò8Q\€eDgSºàÒ±
+üžßëõ×IœµÉ+úåÖ&ìRˆ‡…@Qóõéil¶¶¥åÏP­yô“»sáL£ZtÜ¨Ï»Õ}ÓP'¼‚àð½ÈW+¨†.Ñ¼Ö”§Œu£0¢QÂnñ<Z=‹K®¨Ÿ/¿¾0ý”ð‡a8O˜1f¼Õ¹&Ej±	òcÈÜßo„JèÀ‡=¦#GÈ€¥ûG$Æ(¶ð™G¡j]DZ·lÂo›bŽ÷N8ÞØ5Sã¢“‘‹«0ñuQÕ»àó‘ŽN'4'à¥XËâ¼û~Î^á7ÛtÛ-Ë­Ÿ-uŒ=¸Lsâ”¨LÚ3žPÆ_^¶è‹ò¿@ô…ü×|§ž¬;›·Z„†jæé˜ŸöÒ'^9w•·1ç ±äà0÷› §Õøðð…)kç Y`|‡Ï_¤hßý*…Qmln÷µi™Æ© 97|Œ¡´£Aø`.ÛôR¯îDVþ÷š©\ÈêKŸv…AíÃø|¥½Ê–ãí±Õ“Ì(ólÇÚû5aY~èuhWõ;ÝŸ†ºy?æêCæ6xîÞø•h‹4b¼U¦XÃÏXvñ×íô®Ä•Ý±Ô¢CKBT4ê0ËNmã;Íÿ‹‘>Ñ ÂÛgµÉG½5¿¢oŠé§ŒŠâ@ÉŸC'™šãÃs¸•Í€r[ÈòHq¸"òÁJ#«ÇÜàñ¿žéÙ((ù§¡´çCëáªtpWŠ§®oñ¢‰7—ëcéÀ6Â¸À“]öz5D’§û€Š}ýÏ¢²éôÉ@Ç¦™€(g²!\StÕFEËŸ*ƒQ¼Ñ 6­…‘
?Õv¬¹KžË5ä® Ÿ]ºí«²
gUœ„¬ßkÁ7™Ë¬ÙHÆê8©´µm%Þ‘WÏy/\ÁâÐŒTÃrùÓð‹…	‡PÓõ„	Y`FëâdñÇ‚@ÍÕs¥X;FÆŒbT¼äÃ[·K CtÎŒÜiÔÚ¯ºª5ŠíÀ§
ùì¨B¾W[+¯‡+"~§
Ša-ÄûÑr£Ë‚Ø‹P÷ã+u~žªóŸŸjJ""{ dMQ×ç[ª÷ˆFÆ£F‹=…_Œ…ƒë-BŒ—…¤D¡[ÏóUª² .’‹µ>JŒw)Œ*ôøØlÙyTüLÇÖOür1ìÂßÍôd7·Ð±º	_éÝ\šNM	"àÀ3:)÷ýßŒþ´‰b÷CN@ú²ù‡±‚ðAöoÐEê­Ù¥wjðI«„9†õ¢>o!Qþ{ÉYnæÛšÊ¨†]ê­†¼ÆÎKHÉªÉë-Z?ßÙÿ\–`éË)°?5ªwê­:Ó‡åÍÝÃ=RÞíó„qz4&Ú‡[47kÙ¾Øá¡5nÛ>Ò
:e~©hòðRByÓ(—b[eÚ|]GüÁG1y]§_îÇ_½1²ñ†»ßÝlŒh{Ú
¶LbP[ÌŒ÷.C}¹BA+ü@L2Þ£ÑíâÕkå(åôñôì{p÷¿ˆ` ìSu[¥Xÿ¹ï²¬*ò6þdÃ+'Ý„ÄÐ—w‰ƒú¾z-ï=5±Nµ@Œ0‡ol«z‘žx˜Ô5Ô„;ù%™|Â_Çò0|X›ÊMÐÉcn!D±Ï Dû{UWºb©‹lÃ%Üp˜»Ç9åÚð#óÀëÊmfÍæ°?,ô!·×„L©§ò½ž»=>ÕôU‡Ôíµ®„·	žÛj ™¦P vâÜé÷o m!Bm¸ýÁ$Ã¯‚Ì¶CÎ ¡þ,Ý‰Èù¨~s¡öOJ™*™ÍåÍd>®'«˜¤¿3ò®µ}÷¦Í8Pü´$DMx~$RË¯8xˆŸû§Ä&žòÿm™¥Š9Œ™o4¡Ò.Ž'ÌC'h¶Ý5 ÷¯Êæ?Ô'¾¥Ÿ^©|Ü†.³vyZöJ¹¼Íš†µa´™‘ý±m@LžÕ†xÈàþÅaFvx¢S“%ŒÜÀI•‰èZGÕÎ™»Â‰ÑXVæpŽš?_Ï“hGjÉ °†¤ß`r×ã»âý²:ºÿ¢½ôóÓ­¡#x8IGáìjË(+_i@Eš‘ülecY})JåÐ¿B:ì®.…èxÆBÎ‰5¢A0\7€x\YÑ¨»Écþæ]€	´„oÂ“†A»®EoŽ3^¹Ô“	'‘êý ºŠR„· C<ûR*Swõ
‹q9K7¨üýN›€ü¤Å7FþNQè¿F2õ¶ÆÜfy‚Ñ”ØjŽ‘o|7s˜ª¤þì2EåpŠ‹:	:ŽéùQk…òSx=~—Ì|Ø¨1x•·õElö/ÊSÝus éK·³Y%î—Ð°ý'ÀÚÎYÅÿ.P†çÙ‘vy²%b%jñ‹m@c¿?æÔúÞjÃìCÑ¥×6ÞéÙ#[dbËýmñ¡u.›ÉÆg¸#X KÐTíÙ2Ã*É&Y„›ý0™ž"¸C·V³¦§Â’AÅZêºX˜s‡®rHšzÍyëms©lb•TÏ˜Ùe:)J˜Š×‘gæÉ`«×jJ—n	´XŽõ‰óì¼þ“V3ç©©<'GÎkèÁràuÏ#†ª%{×¹ÇWE›ž¬na÷n=ÂvðÉ‘RÃD\†(ÉáÙxÌœ>1ö”qÆ£§%X°š’ÅÜ-ÃÝÒ“©Íûã·CÆŽB{ŠcŒŒN=’v¦0ˆˆú…Ôˆ’ãŒÐÏé=µHÞé“ø½s7qó)PdÀÔÀ‡å7Ý>ç€rÒ4T™-o‡W%ÁŸ~RÖ<ñ%@ø¶úKóu¶7¨VW'’ˆ•¢ËŽšoH³oÒØ8êüAÙ<¬ÅÎçiÉv—gë|ä(ì‹¤õpˆ¦¤õmNv©Þ:¤Î ìÐ¼þ¶6ØóKé'eÂ=S½¢pè–á”ówhmCYŸD\$0e#€ÐLì{ð÷Äì;Óµ/|lÙÃ ¹ê*Øß”©×›ÿ2ä¦Õí¼N‹téðÝç]ˆë qÊrØ¦s6ñ.dgÄ×Šî³!%.o:-=U—€â«|eÌž€U.¡8ãF>na…BÝ«ÒJZ·µNÈÀ§G¯¥ëh ²±µw?•»ã…‰²òÃL·½s:eÛä7W‘®É§è¤£kå«å/:w.›}:Ã6šÚz‚^-ª2Då(åq¡ëØä©ŽVÎ'W&§]öÚàv6íºgN».;;’ŸrÇ7@>$Ïfs0ßi»,ŒÁÒñÖ‡‡8LBŠ1Pi4š]–¾:¾ÄŽT~CûQÚLqf)ÉîÏ¾àÌrY¾WZsá#>ºj0yN‘S{`9«°æÁÑíÚÒµP›|Ë3þK­Zï„·þñV:%+söÑ/J%´¬÷;ÝÚÝ”×”W8Í]~¬ÙáüŠÁKkM¿v«ðZÄ„O•ÔÀ-;þw’üž5„Ç›¬GÔÿNŸŽÞ+©e™<¦ÜÖ&_eha«5Ó‰}Ö Å’ÅHÛŒ[½dý6ici…JéMÇ¡Ã}’4Nã·Å	BÆÉÌW¨uúyh‹¿®R]g¼ÁÂ¤Z"t«´¡bôaZ”d¿-Ÿð ›ù(·Zñÿõ?*•ÿ'h[(Í,ãHtNC’iqù-T"…	»á|<•5Ibš0.«ùˆç+É`€ûndZxE¹°˜"%Ï¼¼ÅÚÄÒ$˜\|žØùVSŠa+QYî=<„ÒÛç…õ4/[“àØ­/É'ÜÌŸŸg·ò+£…CAœß[Õ9¯“sV©àê —×ûOZ33i›©- jj›EàÒWŽÞf^„“a=F«ñìkXÀè×'qºqlÃö}‰áÊõ\+vYÚÈ„-•"¢¿d/ÔÈ"¨¾•“§B<þ»bÔ‘?-(2¨éêîE&‡U¥ß¦+yˆV½%/Oì$pXßl÷æ+¥bHßæ}á5Nëz3•>mÿ7ö§Ã!ž1FCÅNò¼ 	Míž ŸìÑk‹-†îäþ!P¾ÿ#ƒÎ>I$v·™žaTqæËŽÞ ™˜%é¶R\¦-‰nÈ#9‚¤e„ãÉ@„úºQwÝ÷ê%œ]jÐ‡×2 ×!>-6UTu, ÒlÆ¢¿”žHÑ¥ºòƒ3ïS~é!ø9R96¢žå¾_jn”æAc (t`bI]¶šEˆ¼eo·Õ°	")KÈl¼ré|Qÿ¶®—¨…µ‹½buB=[¸KVeÚÕ<.3_!rÝÞ(V€b•ŠE¹‰Ã‰§¨|ÃˆO¸Ÿ$c¦zÜA\†5^oqKÃ~XÑÉóÚƒ®‰#Cvk™¥Øša/:tÕDœ¤Ø=(ç-®Ìà;Ë2…{cZV®º¾Qå´LÌ]}ñŠ&<›ªêB#“Ê¤Å²½Ÿ]8žJh“WoYÓŽõ«>D”Xp‚:ˆìöK\ÍQlûÓþNÝ¹¾œ•Ã—Â¦1«21Á	zmîR[6®àìXNg#é1îÖ5ö§¼Ìx¥w]ëhö_™Mîe%h”×&äÑh ‰i<`‡òÎÄ’ÝM|@â ÙK†€8½NÂ«7ÐÔ¾sÁ¥ãg5ž?îˆºuë<…¢fE‹³™t¨îÉ”Ø£ºó	ð»~jéSÛ¦íž÷èm“´/¸ÜÇ­È„/7ô÷ºV©Tø
&¢M˜­ˆªGäN¦á÷Bð#e…û\Q°ú
9Üì™u®*9½TxÅ ½ç7ôž-j .Üæ3M¯úæ9q-—y¨‰-
'©|š¡É\¾OI~? /I.H(x¥I¢£ä;“ŽB¿¹å¯ •JG¯ÿOypÖïeÞ¨@I`E³Ç9Ãž²ùØ):U6’”<P¸Ys„+wÆ:b®_Í\HZœLŠ5Á`÷±„xö°JR‹¿:RÝ÷À{A!òÕÚýW6G¤MAUþƒ$.)XÐ}T¥h:‹ÃŒºrÿ4MÑDÜñ<˜ê•ÀîÊë^¨>wI?g¸lJÒÀPŠŒ!ž®.W{Wü"rÌ«?–$6ƒý,_ ¤:ª²ó·„5. w‹§
:ëØË½ µ‘ ˜äÉ©éen +®ÐˆeÆW¡ÏR*7-Q+î¬KüÒÃ˜»+¿Oât¾Çdsˆ‚ "•¦I <–ÞÊz®3/Œv	cÈDüú)Ö¹½‹{F£¸ÈérÃ Gb
û~ª(Ø]ÊnÅ×~«'O!+6X5¡¦/Xãá²XºqµF)o÷\Ñœ¹Ú|>ð øÒðóTª^dÚÌ€iˆ„$¶»OÅïeYXàöYºüd‹%u Äëâ«îWØö†<+áGfÃ<éü›D£ã¶þÎ3@?õrº‚Ù×²˜ó¤¹oké'§®Ñý)ü›ÄØöÑü«ô)æðËñàMü½CüüÙš]×ƒ/F
½¸CxÙ³%*š¶+ º•ÓMSKŽ–rbÿ{`jCÿX¼õ–alÎ4¥ü’)º”Pqˆ¢E¯ I!¤0¬º’7Û“tÃê%·Âµ‹àûsaÞ,¬YèâÏùía}ÞI`´ÌL“I &¤|coÎö?~>HQÊ<­Wß?ˆÔÓ»ÂÀñL39k¸âZ‚;^¤ÔãÑëµÕ!…R«—3`’áâCÜß×ÖÅíÊŽÒ.qãS†¯)z$|K^eªydÀÒ>!ïþy©ƒdî6®~Âí<À,ÉST2ø{VP¬Õô¤–Ë*$ùÄaÞŽ	˜ìWœt§ÆbŒtãëá½Vƒ~#™^Ô—ø‡4æY›¦´S÷,^ÄïÿæÍ‰Šísj’š¿ „	œ•&:ÉX€£’Ý¢ Õ¸3Ã<ª¼$fçÕyulÕÈ¼8uk¾æ‘Ïíùa”¬?ý:ôW‹„ø&`¯£º™ˆÖ%¿4ø…âé<Ïußiuš\ô1Ê°Û„ÑL$‚bñy´0~—V­Òö u	8y¾D~TÀÓ½Li¢½/ÔòPˆƒËL6S¥"ÏïÖÉÎÜë˜ý¦	¥=î %c³Õ÷OÃL5Þ½yuq,l
§¹¥†r}Ñ
4¦æsÿ"×ò¤äg7D3cÄ’ÅßúâÈ»/-¶Où˜8·"ç=+´Ù–ñqœ›W’‹ò6PH˜ü‚3b¿Ã|ªÜîº<ˆQ–ecÕæ–="Ç§C["Èsvt¼w\-Òí,ÉáË	'…PKlvð@ø­?SyJàe«:nˆÌŒ'ç2Ø16;û÷ðŸ+¸•z‹±7÷W#H¥».€UpX_×%·{n‰ðÜ‹v(De ýÉÉàv¸\Y®~>sP„èùà^¯zt?åùH•ñrÊÇ–?|l,G@}™^u¼Wµu@ Ži}ÄÈ‰½ý^-,°ÔŒ¹ãÄàÜ¹ŽÛ&É`x³§äÚ»<x/HÝdÊæT·6Xaš_£qÌ³¯êq€Í2IúèÐ†ÜIBÝ–}mF¼z¶¥Pd³uI/5DÕï˜3¿n­¼¦4qa9ªœ°kjØŸiGsj³yTb9ž÷,óÓ¼ó²úiƒ"oy¼²ÞÀO½9Î(eñÓƒ9¹¨˜H;Šò-[*
²5[=½ü]Ø.\±ˆÛbÒnë€Ã…,H,°XZ„ ¸@¹i¤)•¸1a³Y&ÝØyT€³öÿu”Z¯µÅKÔ»ì
Çõvù¢8'nÄÆc³¨ˆvü¼sX‹l‹óL=Y (1‘-ö›ûZö”xßÌ–€S³Ì¶ò¶
ƒs>¯e ÏàŠ{Ê±MvøfÕ<Â½ÍúxÒÛé‘Û¦™ÑÕ6 &I˜m¿ÁÝ¯­ë¦ wË!ï‚æ[¾‹ë‡òDNHg2ç9e»'dxNpJ!Ñü}¹/Ù	¤5¹˜ylNl¾‚@,ÐÏïêxãï-qúäÁ~>íFc”þ÷äÏ»ý€ÃSbƒ9RUà[ð’*MƒÅ ÷¤—BÅ©·‘è]ö„êþ¸±3ï8`°tŠã€™•ö@&sH©3ŸzKï]üêÛv—”†6¹¬H¼ýâÜòNÙ¾Z‘OÕWp‹"HaK³}Èg	<ùó¸Þ,M×\”ˆ¾bËC„Åv´£÷:(ŽÈSÐ+vPXÙ")#ÎHÛˆEqˆ@YÞûÉ~¡;“SµÎ¥'€"eQçÛlQ²ˆ™¡­ñiª˜fCŸhW…GiyGþ[hÈ¸Oî^âh2«ÒÃ(¨ÑvUà·÷M7R?´>RbËP-'üÄsfX¸Š¾1X ûF^“s
æ@tÌg«Ú1õµãÏðš9r
ÿƒ¦?gY¼$‹ÌQÏŸI G©`äJmzúÆfîà•oX­Ë†®—Aæˆ‚Þ€´Çùc·±ÒH©ÒAp”–-ae_O8&Ä¦PŸÿß Ì»*½Ÿcõûr2Ef‘$ŸT³ïå€·÷á¹ËŽ]G%ÂX~Õý ÇO»á@ƒåž^i±DjRÚ¥£ÃÇqxø7ËS[Z‚Ao’9j»)úAÊ  Â({òò6È²Ý´^Ðâ5&ýóºie2®&¾ŽÚH“Ù¯©•<ƒ˜XVz™%
PHæ*ÿê·+û™dk ­—Ž¨ìÞq›~é‹ÚX2A
ÊE2:B>ãdaß‘lxŠê-Þ¤"¬É2Ò—ªS±˜!qŠ·k‰<@ó‡ê,mª;“ÇËOx´æ²6—ç˜òóÉW»:¤{—þþ(ÔSä6ãËtá¢u2U	ªhjbª¸2{]Hv¹rõ’©™uõ#áq')YÜB´A€«¶c}Ãœ|W³UE±4Ó,ÏþvE“ä`±Êú!ßiçlÂÁÐB¼»ï¥
6F­¨Ðä†¢[ÃûwLßÖŸ²o-ÇA\Î}êO®z`„‡7:/q1ÕénÒÊë¾y¼"ŠÊP®ß‹Ý¿³ŠNGç Tï±yÿÕIL`.ïGÒ 7’s)"¼IÂ¥X,¡Y 5;cGM4Ýåô’hº@râÓEe<\@©}ÎÄ3è1®t	ò‰•b!)}%TyL€<Ü;HÕÜˆ6÷ÑypÇ Ò–Ú”¼HÍW­UN,ûãC&ÃÂW›´ø‡8}QE-}{*èHcõ|*µjéïMœŒlÏŸíó3fC(»'¨6lÕö‰i…Œw/¶gÄ—¸GÙQV|R?èüìp7(÷zª€g+ôþÄ,ëNè4¦T,%¾cÁW}ÀB™3Åy8Îž‚‚•XV¥rÄ=¯›ÏÂS‘1©w;ŸÁ;Ü4´`:™ÖÄ>,ªlåNÔ˜Ë-ò/©Iù-»áEŒëI7µØg«ìuäv;fIËÿ˜°Êw™oHi2—¾ò@¶½:NWCûúÉ¤øPEÙÂO¢ÒÕ¡áWÅ¸Á	úä÷ÔÎù‚Í)Öƒã(þÕ©KÕÀ“§Mgˆ/6Ž=_¶ü[}¿íTA–/Mìh`×²&w#°qäsø,ìÌ“<soD÷(ï~ôßÂ~é¤ÿ‹§ˆ3·…Ê|ikì¢Õ¿Çï“†5~ákÏ'[¬13;£
ÄË)Q^ZÁÆ¬P¥¬ßt¬+³?úSyŽÁqH]7¿É2xòP£R”âÒÈÁ2‡c~¢ÿcéCßÕDeF²ºÊÎõÈÔ [û°–÷—øJ_‘ÌµÐ¥t“KhKU>„¦ §$ÕW·º^[³tcÕÅlbð¼_Ó»^ïqä1ot™Ôp\‘~ebß"‰È×U{˜e:s
œI®&~/jÅðT%ýG‰_+œÙ›ÎÕó#¡‰ð=y`ÒøMå_Çx“ð+äýòq÷2±ÙXñšÖe.|EG32§BNG"gËb"z#p
nnÓ°à6-ªû€á'eò®ÊAælKÏuU]š…‚ÕMRód¶b§õ(wêaÀXòX÷þy\ÓL¥=Á#ù#öù‡0ß Û¯s;BHÛÁ:’)S«9QÅ}¸ÞÀ?`¥ÝMõ	|¬+Ðc47Ù³+¸WŠ´¦•°íŸkHö«å›äWÔö¯Í²ðê_’Ø‘;Í—¹ßèHBüŸÚ×OæÉõÇ¿îö)É¼…a‡ø»\B6\ÛL
nÐ„‚V´dRÉ=ÎæGŸ_…¨¹,pº¼NN•ýv}t„åÌ þykK'¿(oºøâœöº[¯$A Õb™ÔSª”‚Ü§ý{oz"¯¿VT+àöÒDh|¡@/Ï8Ë#^³¼âi&‰nv\8ÆSlíÜ(qùMÛÜµ_‰¨a„Kà/Hú¶XšóÔqÝ|ðƒÓðD›Z¼ÏÚ‹ÝŒhIˆ,Üb ëÌó5'ºÎê¡¬|ãhM‚Ssüœ÷ÂLg-L¡r]GÀ%ý?‚)e#â“fPÕ‰èÜªH“$¤7+$hŒà€Ñ¤Aø3{Ëáç®@Á™açï Rë0hk¾5Ü&‡3ŒèŸ¸"‹ªz_ïøÁÔö%cáà¼š–{÷Äæ€a¦ª—ÐûøÓæÂ¢õY¬:æ•‘È«çnÃ©ªWãZÍè‰]Õê‹>Ÿœ@íçâÝ­,< Ë>¶›¯žbÙ)äœwù	y¨Ø¨uäþUºkˆÂf¿‰eõ¬ÁÅ	¾Ëê¬âMßä8êÚ„™n«’þÿ¢r“Æ»~ˆ¢øóŒ‘Ï‘HíÎ¶/b£.‰ƒ*âd~Ÿž°«µôþg;~ÕóŸˆQ¿\:J g²¤ª°)y•Äsy~Ï7»ÖÑTËí1YÕTX8þ„ú³ë¤¦t 7_†´ÀæPâMEÆ_¥ÿÄÔµàÈu¥¸?ê`ƒ}’X»4óyZRÒ0)^öùÆ¡óÆ5£3ã.B&à‚ ¹¸¡Ê¬Ù±<léQ‚ÕTðT[J1“=þQ@¸ŽG9# ‰#ttYpyø
Cqc¬í¾: íÔ>’©ãW8Â­IBå­‚ó†Ð„$MÿÄÌâaÀRâlànˆÏìJ˜Æ¡mö±J½	Ýêóüu¹oCdIÍ¡ò,=(¨ž2Ûk¨RÒ"»çW'ùAöâQ–ãÕjJÄöËK‚™ Vê#¡=$	 !{¿wåf 4D– >§›Ý,üþz2r'Îå‰†ì†9¯u7Œ*!•îÂéGÎ#)·÷@JjRW|B}gÀÊB¤ ÛÈ"MX™L=Î­ßoK»Ç˜’&(oÎÉù×Lñ6äm½VÆ^:ä‹-¥’;Õ59àÉB°·žÐþiwÀ•\Î¨<ë3º®ZF:èn|p°ý¢–”n»àÆ…¢÷Rõ@Šé¸ðM!ÅjÞØ¨¿' îÏNÑ5„0} ž“ðöjY”å~ÖîqLÏo]±`mD“Ù@ù*˜¯úó &iÊÇq1¿Yš¬o{ªí9í@$,ar˜C‰ØlŠ*5u1 ydÍ,Kà&¼]²s<8ùöü O@õbÉX/É!XiyÉ¢=ôÀt>´JXªI‹°™m|¡Ÿ„õOya|›Êž$#4“’{œè—x[K,²O«U¤k7ú…QË´ÆÛô@k1Ú§o¯n>è!Éqã?gPZ½E!Åy» Í3ýÂK#£D I‚µ6´mãó\zwZñ.îü_\1@äDü_¤4Aäã¹,€Ý­à6•HQV=	„~ôŠÎ$ë8’®×„‘MèvDJ´S‡™ÍU’+Ø‚†fÖÄAêÿîA}Â¸:gIähã‡X9ÉB)nùãªç†!Û&»m®@ÏV/¬bõ‰Îà{†	1²I3"´ý«Ôò¦‚\Æ¦•¦/ŸÂµmÀ?|:)´KfgAÁ¶tlù®TìŠ…â¨R^Ù™–ÊÝ)¯ÞÙ‰š×Ö|e
…úl&IzjbÎq¾{Å[`_àÙYüDíÁ|lnÀW(zÛ+Éæ~'«Qh¨¼õªq+åžþÃwãŠ:M5•p”àK02ÙéïC±ò„/w À´aç8oÉ@úêJ2ç}?|mÇ¡g€fõ‘ÆFkš-àÃÏŠ`T¯ú +VºøMƒWC©¡8æÔNë0Ï¯½Æ  Éš˜ÇÃD£.vKFhä&ïŠ2úßÚ²Dg6l>v*C_(÷ÿÝTyÕ§yµ
5TÕ_VŽ÷Þùx÷¸	ÑÄÏó[ÁîÈXÈ6Gfäé{šäxy´0u¢,Ÿáh¿‰®µ~š}à¿Æ¾ßM¿üRáîT œËŒ¶Z4@D*©½ÚiRL „Š6ýÕÌ:¸ëäÍ—Òx6
¶í¾/îÉ•HÈß{¬¦„Ú`‹¥œ
O5A
<Ãz|e‘ap\ŸÑ¨QŒi
tã0óX“X&d÷@½€ðâ7Åà­+v¡vxÏ±³(~”ä52YÃ×‚ªwj( Oáì©Ì$ë&þó8	:ŒÜ>Êûh^;2KÎ³&NßT”q:–”!çŽšŠùþt]—>Ë2/5Öî~$.ÏÉÿJò¿“Í;˜€¹ÅÑ\^6üÃ€wŠëRºP›’©7r }“¤ö­ˆž´lC×Y&¼«è§r\[ÑQ€:R…G‹qj2$pÂ¾lÁ9ñ„ß°ñ1s™Dós´<.þ£c_íó*GZ“p_ú5eRÄ»›ñÈaåtóAn_ÜÛÀyéÃzW[§pùáê×t¯ã;M}Žò£±w„ošê[Lll[¾9{6|xï{1S(*F§‚‰BnQp—ß|ýÓø«`ßÑf€5Û`'nAwÜŒBº˜Èøµ2¥ª…Bê½/»Þã{]/ÂŠu‡’Øi1ˆ!–þþZüpœWI…tSê.š0„Ñ¾ƒÂl>™jÆ½Ÿiý[~ÂŒ 5q®7šÝî”=Åhà·éŸ:aÆ€‹…ÏÑÃ;7s:ì›ÒpÊl˜ Š¥#"ŠÀâÈøúü#u	;Î˜‡Àü/NfâÛ")!Ü‚Ž¨®Â+Þi{jWSŽ6òð™Ýarhð„ëh|sÍÑ%	uÅ\ŸKˆ4! ,X“¶/²hŽ¤Ÿ›Q4€¯ûK€dªzP•ch
)¿!­r!ãèPûè%Z!yU$6dü(Ñ,âõ	&º¤ÁÓ±06–6œVÑYóƒ=‹Ö1Ô‡Ý˜µ|M|cI‹ˆM…’a_f’$s_|s÷È/ZP^žö¡+Â
ý™þCSþ™%~4ÚÞÀu¡NT)ÊP—?×A#ñÿ¹4!¢»ÞÃGßxƒbg
wB'¨ÄÙF5bô(&Œ„¼ztœœkR#œ½Âtd(‰®9Ø‰/=H “µxøG9¯€MÇ¥©gbÞúóõ¦›’ ðQ‰C±âpÔþ}‰ŠC¯qV)Bwá!H¨‘XèV?ü(FO¿%fqÛŸ)ÀDíW;%›ì¯»Äõf sWDd¨ÇÊÌëBöÀ]Â~xgj_l +Y^M¶mOòK·V@ù€ËaFC0XÅ–Wr-í
ƒ¯_ìƒÙ1upO!¹&=üè.G³ôŸÈªœpqsÜ™±f›Ö”	óú²5|UyÓºPâ)mYh´]	f7v5µ¨²ªµž0X‘vf¥ä.Ý.ˆ»!CâÔ!ÄãkS{ŒØ±ô¡8'!N‹è‚´yÙEç‹sDbºø^J¢Ip³…OŒýäœªèÈñÿ(šÈL_Þh‘Bg\Eß`$kDE{[çOYTúj]Ãñ1°š"MsPÒ™¡Jj[)‹Œ,n~5?1H­v¹šnI‡nß•A¶ÂÔ‡Òù¿îK–Ìxg@u‡P»£fŠÃ^æ¦*’šê‘“<}-7€hIy‚ÓÞ®œaùÖZ’lF?ÏuèÐ‡®ÑÄ½
®þ‡IO¤bzàW4âùmï×´“ZY8¬‹Ô±éõ®©ÍøÛ\tþÆœq%¯Ï‚§èÛ¥|p»R‹ˆdƒÄXÌ™rù«/ë4*ËP]Ç!\G¢„1›HÌ±—D*!~ŒjÈA‘€óÜOxº½äIP×ÞoH½“oë¦¼ŠÌ‘KÌŸ
»å*j6c¾0NP:Ž’ÓY3°KçnîPÍˆ9FUJîp·äñ³hÚ”—A*¶©E„jðZþWV©„éÖXImÀFE÷£­”u­²w»Ù	Ü‚ýÞƒÁ­Íú ƒÞÚ(ŽIÝÔ”GÕö8?æ¡îî{~óÓ6 îˆoÕµ•¶ZG`t(§Sß•²ˆ&A¹½à”Á}×Ï_è¼¬¾ZD¤ª$˜ŒÕòÛø	íòuYûáˆ­“L—¢òÂ;ËóÈ]öóíUë´6¯µíEBj•µ l€ÿÀ0–¡¡jÀi¸?‡Ÿ!ÂZîœŽÆîû‡ïÁš‹±gô`0 wëê»€}±ÒŒ?Ú¾$Jv`…¾X7Rð#÷ÐÝFÇ%|ÓºÛîtÅÃ x¶¨IlNq—î8(FYƒ/•îŠáÀ¦SÒ=°Ùº ²È*6xYnÉ†$è%kûŒOá4*ó|¹ï)h(þ9'K‡E›#,ÐÚO¸!+«
ÒmŽÇFúSÁ½ÉÖ>±{é­—:ÿñÑ.v6 ÍbìczaõíV)·±—T¥ÜXI1ù0¿ú\ýƒqëévŽñ–Õ£æ  R2R£Â_B59«J:IƒÍÝÓJiÚ%š¹Žˆ¨vâà ]2œùƒëç¬ùï-&™/ñ_¿Zm%à…ë j›™'Ü;­Ì§Î–?÷R×‘Ù@†vaë=jQ4c´Àfñó™h¨›E_[4Ø=r` sÕg—Úƒñî$G	‡ÚÀº£oˆ÷ƒˆcÔí72ê6Gt¾Â³O¦]Åœý{š†£²×2ðc+!cK'Ú{pF®´ùÒfbœÊE¦FB=Th*Dàãî•ÇVÉ”Bf”î¤¹^Ê–‡n;u_½íPYŽœÜ·Òµ°f¦;U—“me[cdÆ¿v«¸» ¯ù5±¸qós…âžÕ²‡úIÑQåXáDÐJ d@Naý=f;3êÅ‘wæÌSŠ¢¹™,C§ÌM>ÉŸ7¶q±ìôÈïÅÆî®kjsÜN$oXA½/œ^º6©1U¾‚|D{
ø
Õah˜M’â~³?ë‚“´Ãâ}ï1&8ªJ®ôI™Iõ—0S½^mbtüx¾„Y´?WVv:äþhA¶Ê9[ÕæÑëµm¨í	žÔé•þÝ{0Þ;ø8R…ûÃ}l«4
þ™ús¥ÛœîS&­PboüQ¬{Ïø’³W¦=ºÁ´–'ª4í{	Âûâ¢å³«JÜõ3ñó¯
¼šÀ‡9¯’—Õ!L‘®¬å¶,ªz¨f(I¯4xñ¸F@é¢2Œlç^®ÓšËIÒ#¿“ö´= »à« š±íÃ:Íò@VSì£ŽÚÃq‰‚¥î‰­ö÷¸ªsñ|ÈhzFVYõfI0‹ã?þ:JøÃ4™ÙØìÄo¶Ü_g¬e7gÖ—ŒÑÅu²“¥Y°Ìk3g»;Ç{ìKB?Ä”>XS˜Þlq> Ô]«mò‘°öèÙO¢†Ò×ŒéÚ¸ÖŒä@7`Î×Wë[×sI¦W  þ¢Ihf‰'øÊYq|+zlZpzÃ»,4’1‚ñÈr(í³šÉáÓÔ>Sáx¤‡J¦cG.3öË.ÓkƒØÑË;’‘6»ž«ÉE³m¢ÑYøÿ¬hXŒgcš³Ê~yð‚H“ó‘ûÐz§?fqN´8\ªòÚù1H8æ“ÎçVZõ+
áË²cz;úË±Sõ¸²/U5óè¤ Ê<°xÞu Rgéf™}ë®RØd Ù7]üïõFàD¦?fô­´ÚñÌI¢a]«+«©søpÇx·d|ÀƒÛä#tU.Ü®‚l[¨47@ú8s—7äìËv&ŸeÏÖ°´ Õ6€t6¯Nº±¸+^Å3LÇ#óxûv…Ê “eƒju13p¶ý"åáy&ä¿ð«u]H4K•ÚíF4*KM6¨ÃúŽ^2/­ÅP›^“ìÐ|>Œ°w+G˜‚ŒNå	Õ%U÷C‚ÁF‡°M…î²gOÑÑ\FÂU„%ŠðŒ¸kkº÷!4(—ôiª7H¾„øœ†¨õÛñèµ•ÿj–‹—à<Äxâ!§E{ƒ¶—ÇcmíðWÁÍ¿Œ3úšcŒ˜5CÝ¡¥Ü•Ï‚ ‰¥Ò¶3XÖ=¸É”Ú¶òlwÈÈg™MîÃ
S±(ehX­w¼4cäÖc¸#Å0â£Ç¿îÏ`¼<T£Å]w@aÎn²ö¯ïè¿«Ÿ1(šƒ?AK‘ª?rû ïùmO
pKË×œZÆ‘P‰„#B%¼¿}óóuØïYq·¸J½d—×Â¹”B`UM‚Üû6gžT©*˜¢ÀrD¼äÃrnî-;AÎ¨	mNuäü E)ê"=1í»+ïwÚ+Kh-ÒÂ%6ÈE4æÜøµÖ†‘ÁF-×Áþ‚êƒ¿5 @xs°«Ë‚Ñ×ü›µìY£ŸXJï ñÚRÓ§2µ§kÙ“½¾Ìd•Š:xÞO(kÊËq´Ì¤›ÃlyÀ]Áÿ—ºÝtLÀ›ó~šmyËaÕCU¾Òw™XlT¹`õµ8lh®Ä÷ÛùGa<mn‚,cë÷µÐ ^€Ã—ë2Å@¿â§´â¿w[åîJ4È)_ºÑ¦EþA¤3&=_>ˆbî”-J&òP²oQÑ'r>²!Ð‰Æƒ.8ÿàÒ†Y­	ãbÀó€'™œe=gŒ,mo€Fánn1u²ó³-å-Á¸?K]Ì&Â—‚ùÎ$ÌZÅ&õ+£ºËg´çù‰ÛEr¢ÌÄ®³Ñ?‰±*¤ÌþPÎx&ÃÑãDtLe´_&h¾Å/5uPíòõX#=8û¢ï¯ûâÚsT°Â±?…bU›•cÉ3Ê³˜›2‰£Ò½ÛšR(§XÊÊæv—Ìý¾JüGÖÆ6ÛËÓ|Î1`—Ö†	¬Ó‡ž¢ºWl¬â†åë&ý2Ÿ0= ‚—‹‚ñâØ—×X¿C¦<¨ï²uÓç4¡:¢†lÈïÅƒû¤“SyÖ÷9‘ý2äY&ìF®ü¢hÖ²Hr½ÜßY‡Àé†¨:x¤6ÿMDYË/°sÊ‘¡¡ü-·m»µÍ|²•\Ý¶œIAhBXûË¾À*Í‹s$tè{ý#¶IÜqÂ†q…«œ' '}EN”'²üy°™»¶ÁÇZƒ~
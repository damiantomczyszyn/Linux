t empty, or NULL if otherwise.
 */
static inline struct lock_list * __cq_dequeue(struct circular_queue *cq)
{
	struct lock_list * lock;

	if (__cq_empty(cq))
		return NULL;

	lock = cq->element[cq->front];
	cq->front = (cq->front + 1) & CQ_MASK;

	return lock;
}

static inline unsigned int  __cq_get_elem_count(struct circular_queue *cq)
{
	return (cq->rear - cq->front) & CQ_MASK;
}

static inline void mark_lock_accessed(struct lock_list *lock)
{
	lock->class->dep_gen_id = lockdep_dependency_gen_id;
}

static inline void visit_lock_entry(struct lock_list *lock,
				    struct lock_list *parent)
{
	lock->parent = parent;
}

static inline unsigned long lock_accessed(struct lock_list *lock)
{
	return lock->class->dep_gen_id == lockdep_dependency_gen_id;
}

static inline struct lock_list *get_lock_parent(struct lock_list *child)
{
	return child->parent;
}

static inline int get_lock_depth(struct lock_list *child)
{
	int depth = 0;
	struct lock_list *parent;

	while ((parent = get_lock_parent(child))) {
		child = parent;
		depth++;
	}
	return depth;
}

/*
 * Return the forward or backward dependency list.
 *
 * @lock:   the lock_list to get its class's dependency list
 * @offset: the offset to struct lock_class to determine whether it is
 *          locks_after or locks_before
 */
static inline struct list_head *get_dep_list(struct lock_list *lock, int offset)
{
	void *lock_class = lock->class;

	return lock_class + offset;
}
/*
 * Return values of a bfs search:
 *
 * BFS_E* indicates an error
 * BFS_R* indicates a result (match or not)
 *
 * BFS_EINVALIDNODE: Find a invalid node in the graph.
 *
 * BFS_EQUEUEFULL: The queue is full while doing the bfs.
 *
 * BFS_RMATCH: Find the matched node in the graph, and put that node into
 *             *@target_entry.
 *
 * BFS_RNOMATCH: Haven't found the matched node and keep *@target_entry
 *               _unchanged_.
 */
enum bfs_result {
	BFS_EINVALIDNODE = -2,
	BFS_EQUEUEFULL = -1,
	BFS_RMATCH = 0,
	BFS_RNOMATCH = 1,
};

/*
 * bfs_result < 0 means error
 */
static inline bool bfs_error(enum bfs_result res)
{
	return res < 0;
}

/*
 * DEP_*_BIT in lock_list::dep
 *
 * For dependency @prev -> @next:
 *
 *   SR: @prev is shared reader (->read != 0) and @next is recursive reader
 *       (->read == 2)
 *   ER: @prev is exclusive locker (->read == 0) and @next is recursive reader
 *   SN: @prev is shared reader and @next is non-recursive locker (->read != 2)
 *   EN: @prev is exclusive locker and @next is non-recursive locker
 *
 * Note that we define the value of DEP_*_BITs so that:
 *   bit0 is prev->read == 0
 *   bit1 is next->read != 2
 */
#define DEP_SR_BIT (0 + (0 << 1)) /* 0 */
#define DEP_ER_BIT (1 + (0 << 1)) /* 1 */
#define DEP_SN_BIT (0 + (1 << 1)) /* 2 */
#define DEP_EN_BIT (1 + (1 << 1)) /* 3 */

#define DEP_SR_MASK (1U << (DEP_SR_BIT))
#define DEP_ER_MASK (1U << (DEP_ER_BIT))
#define DEP_SN_MASK (1U << (DEP_SN_BIT))
#define DEP_EN_MASK (1U << (DEP_EN_BIT))

static inline unsigned int
__calc_dep_bit(struct held_lock *prev, struct held_lock *next)
{
	return (prev->read == 0) + ((next->read != 2) << 1);
}

static inline u8 calc_dep(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bit(prev, next);
}

/*
 * calculate the dep_bit for backwards edges. We care about whether @prev is
 * shared and whether @next is recursive.
 */
static inline unsigned int
__calc_dep_bitb(struct held_lock *prev, struct held_lock *next)
{
	return (next->read != 2) + ((prev->read == 0) << 1);
}

static inline u8 calc_depb(struct held_lock *prev, struct held_lock *next)
{
	return 1U << __calc_dep_bitb(prev, next);
}

/*
 * Initialize a lock_list entry @lock belonging to @class as the root for a BFS
 * search.
 */
static inline void __bfs_init_root(struct lock_list *lock,
				   struct lock_class *class)
{
	lock->class = class;
	lock->parent = NULL;
	lock->only_xr = 0;
}

/*
 * Initialize a lock_list entry @lock based on a lock acquisition @hlock as the
 * root for a BFS search.
 *
 * ->only_xr of the initial lock node is set to @hlock->read == 2, to make sure
 * that <prev>ELF                      ?"      4     (            GNU  ?       ?          ???????t?S?????  ??t???t
??$&      ????????      ???      [???&    ???&    ?????UWV??S????d?    ?D$1?????   ??  ????   ??,&  ????   ?F?$    1??????Tj ?   h    ??????????t   ?   ?   ??  ?,$??????????Z   ?8?   ?
   ?h???  ???      ?x?(?G??????G$?u???Dh   j P??????ud?u?h@  jPW?????????$????????   ??,&  ?$?????????   ?    u-?D$d+    u??1?[^_]??v ?    t?????????????   f?f?f?f???????@?@    ?@    1????&    ?????1????&    ??????VS?p?^????????v???????????[^???&    ?t& ?????UWVS?????5   ?$?????2  ?h????   ?}|????   ?w`????   9??  ??4  ???7  ??1????S?D$?GT?D$????????D$?D$????D$9?sv?t$?$?D$???????????????C?t$??C?OT??)?9?C?s?s???C?GL9???   ????[^_]??????-   ???  ??[^_]???&    ?v ???L$?D$)????$???L$??????D$?L$+L$?$??L$???????4  ??????T?????&    ?   ???   ??[^_]???&    ?v )????K?????????[^_]???????    ?   ???P?????  ??&    ??    ?????WVS?p???   ??T  ?????1????????????  ??????????(&      ?   ?????1?[^_???&    ??&    ?????U??WVS??l?u|d?    ?D$h?E??$?C??D$????   ???????<&  ???  ?|$d?H?D$\?D$??<&  ?T$`????????"  ?????  ??x??T  ?D$????  s6?$?T$?    ??????T$?D$hd+    ??   ??l??[^_]??t& ?????   ?   ?   ???????    ???      ??       ??  ?   ??  ??  ??  ??  ??     ??     ??  ? @??!  1  ??(  b   ??????<$?L$?o???   ??(&      ????  ?D$??????$?   ??????$?????$?    ??????????????????????&    ?????U1??   W??VS???/d?    ?D$1??GTjj ?????????????  ?G    ?$??   ?   ?????1??   j j ?D$??????\$?P   ?C    ??X??`  ?}d?????????XZ?????  ??Pt????P??  ??????????D$d+    u
????[^_]??????S?@@??uh    h    ?????XZ?6??$&  ??u??h    Ph0   ??????????B???????$&      1?[?hh  ??????    ?   ?????1?1?h    ??????        h?  ????????h(   h    h`   ????????b  ??Ph   Vhl  ????????$??t????????????h(   Vh  ????????b  ??j?h   Vh  ???????????Ph   Vh?   ???????????h(   Vh?   ????????b  ??,&      ??Ph   Vh?  ????????a???h(   h    ?????Y[?b  h(   h    h?  ????????b  hd   hd   hp  ?????????  hd   hL  ?????^_??  Q?L$?t$Phd   h?  ??????k?T$????tD?}|???3  ?=    ??  hd   h$  ?????XZ??  hd   h?  ?????XZ??  ?=    ??  hd   h?  ?????Y[??  ?H   ??????@   ???????PhP   Uh?  ????????p  ?????hH  ?????X1??    P   ?           cx18_alsa_exit_callback snd_cx18_init   cx18_alsa_load  debug   strnlen strscpy snd_cx18_pcm_create cx18_alsa_announce_pcm_data   ?              ?  ?      ?                              pcm_debug   3cx18-alsa: %s: struct v4l2_device * is NULL
  4%s-alsa: %s: struct snd_cx18_card * is NULL
  6%s: %s: PCM stream for card is disabled - skipping
   3%s-alsa: %s: struct snd_cx18_card * already exists
   3%s-alsa: %s: snd_card_new() failed with err %d
   3%s-alsa: %s: snd_cx18_card_create() failed with err %d
   CX23418 #%d %s TV/FM Radio/Line-In Capture  3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
    3%s-alsa: %s: snd_card_register() failed with err %d
  6%s: %s: created cx18 ALSA interface instance
 3%s-alsa: %s: failed to create struct snd_cx18_card
   6cx18-alsa: module loading...
 6cx18-alsa: module unloading...
   6cx18-alsa: module unload complete
    6cx18-alsa-pcm %s: cx18 alsa announce ptr=%p data=%p num_bytes=%zu
    6cx18-alsa-pcm %s: substream was NULL
 6cx18-alsa-pcm %s: runtime was NULL
   6cx18-alsa-pcm %s: stride is zero
 6cx18-alsa-pcm %s: %s: length was zero
    6cx18-alsa-pcm %s: dma area was NULL - ignoring
   3%s-alsa: %s: snd_cx18_pcm_create() failed with err %d
     P       ?  ?  ?     ?    ?  cx18-alsa CX23418 CX18-%d cx18 CX23418 PCM version=1.5.1 license=GPL description=CX23418 ALSA Interface author=Andy Walls parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
 parmtype=debug:int parm=pcm_debug:enable debug messages for pcm parmtype=pcm_debug:int  8           ??     ?           ??    ?  ?  s   GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                                      
                                                                                                                                                         ??       U     *           
 5       I     P   P   H    _           s   (       
 ~          
 ?            ?   U   ?     ?            ?           ?      #     ?   =          O   Y       ?        4           B  8       
 T           ??d  ?       y  ?       ?  ?  %     ?          ?    ?     s   d       
 ?  ?  R     ?    ?    ?  ?   @    
   ?  0     +  H       
 6  @       
 A  P       
 L  ?   -     e  ?        ?          ?  ?   
    
 ?             ?             ?             ?             ?             ?             ?               U   ?                  !             '             >             U             c             n           z             ?             ?             ?             ?             ?             ?             ?             ?             	                          9             X             u             ?             ?             ?             ?             ?             ?           ?     ?                 $             2  ?  ?     F             S              cx18-alsa-main.c cx18_alsa_exit_callback __func__.7 snd_cx18_card_private_free cx18_alsa_load cx18_alsa_load.cold __func__.3 __func__.4 cx18_alsa_init cx18_alsa_exit __UNIQUE_ID_version366 __UNIQUE_ID_license365 __UNIQUE_ID_description364 __UNIQUE_ID_author363 __UNIQUE_ID_debug362 __UNIQUE_ID_debugtype361 __param_debug __param_str_debug cx18-alsa-pcm.c snd_cx18_pcm_prepare snd_cx18_pcm_trigger snd_cx18_pcm_pointer cx18_alsa_announce_pcm_data.cold snd_cx18_pcm_capture_close snd_cx18_pcm_capture_open snd_cx18_pcm_capture_ops snd_cx18_pcm_create.cold __func__.1 __func__.0 __func__.2 __UNIQUE_ID_pcm_debug362 __UNIQUE_ID_pcm_debugtype361 __param_pcm_debug __param_str_pcm_debug driver_find __this_module snd_pcm_new snprintf __stack_chk_guard snd_card_register snd_card_free cleanup_module memcpy kfree _raw_spin_lock_irqsave kmem_cache_alloc_trace fortify_panic __fentry__ init_module cx18_start_v4l2_encode_stream _printk snd_pcm_stream_unlock __stack_chk_fail strnlen snd_card_new mutex_lock snd_pcm_set_ops cx18_claim_stream cx18_release_stream _raw_spin_unlock_irqrestore snd_pcm_set_managed_buffer_all cx18_stop_v4l2_encode_stream snd_pcm_hw_constraint_integer strscpy snd_pcm_stream_lock cx18_ext_init mutex_unlock driver_for_each_device cx18_alsa_debug cx18_alsa_announce_pcm_data snd_pcm_period_elapsed param_ops_int snd_cx18_pcm_create pci_bus_type kmalloc_caches      B  '   >  Q   B  b   9  q     ?     ?   6  ?   I  ?     ?   ]  ?   @  ?     ?   	  ?       R    	    8  )    1  8  ;  [  C    Q  :  Y    _  W  m  9  ?  W  ?    ?  G  ?    ?  B  ?  B  ?  B  ?  ?  ?  N    B          ?  =  ?  S  ?  F  ?    ?      =  @  =  R    Z    x  F  ?  Y  ?    ?    ?  B  ?  J  ?  P  ?  M  ?  U    B  #  9  C  J  m  L  ?  U  ?  9  U  Q  o  X  ?  D  ?  U  ?  U  ?  G  ?  B  ?  	  ?  9  ?  7  ?          K  &  O  I  H  V    g    p  R  {  9  ?  G  	             E  )     /     4   E  C   ;  V     [   E  `   \  e   	  j   5  s     x   V  ~   T  ?     ?   E          	          E       !     '     ,   E  ;   ;  B   >  G     M     R   E  Z     d     j     o   E  }     ?     ?   E  ?     ?     ?   E  ?     ?     ?     ?   E  ?     ?     ?   E  ?     ?     ?   	  ?     ?   E           
          E      !    &    +  E  2    A    F    K  E  d    j    q    v    {    ?  E  ?    ?    ?    ?  E  ?    ?    ?    ?    ?    ?  E  ?    ?    ?  A  ?    ?  A  ?    ?    ?  E  ?       B          E     T       ?     ?     ?     ?     ?                                                    $              6     Z     W          6     Z  $                      .symtab .strtab .shstrtab .note.gnu.property .rel.text .rel.exit.text .rel.text.unlikely .rel.init.text .rel.rodata .rodata.str1.4 .rel__mcount_loc .rodata.str1.1 .modinfo .rel__param .rel.smp_locks .data .bss .comment .note.GNU-stack                                                      4   (                  2             `   ?                 .   	   @       h  ?              <             ?  ?                  8   	   @       ?  ?               K             ?  ?                 G   	   @       x  H              ^             x	                    Z   	   @       ?   (               m             ?	  ?                   i   	   @       ?   (      
         u      2       l
  	                ?             u  (                  ?   	   @       !  P               ?      2       ?  +                 ?             ?  ?                  ?             ?  (                  ?   	   @       `!  @               ?             ?                    ?   	   @       ?!                 ?             ?                     ?             ?                    ?      0       ?  &                 ?              "                                   $  ?     5         	                b                               ?!  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
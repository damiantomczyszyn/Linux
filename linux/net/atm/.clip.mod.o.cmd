debugfs_create_file(lockevent_names[LOCKEVENT_reset_cnts], 0200,
				 d_counts, (void *)(long)LOCKEVENT_reset_cnts,
				 &fops_lockevent))
		goto fail_undo;

	return 0;
fail_undo:
	debugfs_remove_recursive(d_counts);
out:
	pr_warn("Could not create '%s' debugfs entries\n", LOCK_EVENTS_DIR);
	return -ENOMEM;
}
fs_initcall(init_lockevent_counts);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef __LOCKING_LOCK_EVENTS_H
#define __LOCKING_LOCK_EVENTS_H

enum lock_events {

#include "lock_events_list.h"

	lockevent_num,	/* Total number of lock event counts */
	LOCKEVENT_reset_cnts = lockevent_num,
};

#ifdef CONFIG_LOCK_EVENT_COUNTS
/*
 * Per-cpu counters
 */
DECLARE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * Increment the statistical counters. use raw_cpu_inc() because of lower
 * overhead and we don't care if we loose the occasional update.
 */
static inline void __lockevent_inc(enum lock_events event, bool cond)
{
	if (cond)
		raw_cpu_inc(lockevents[event]);
}

#define lockevent_inc(ev)	  __lockevent_inc(LOCKEVENT_ ##ev, true)
#define lockevent_cond_inc(ev, c) __lockevent_inc(LOCKEVENT_ ##ev, c)

static inline void __lockevent_add(enum lock_events event, int inc)
{
	raw_cpu_add(lockevents[event], inc);
}

#define lockevent_add(ev, c)	__lockevent_add(LOCKEVENT_ ##ev, c)

#else  /* CONFIG_LOCK_EVENT_COUNTS */

#define lockevent_inc(ev)
#define lockevent_add(ev, c)
#define lockevent_cond_inc(ev, c)

#endif /* CONFIG_LOCK_EVENT_COUNTS */
#endif /* __LOCKING_LOCK_EVENTS_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /* SPDX-License-Identifier: GPL-2.0 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Waiman Long <longman@redhat.com>
 */

#ifndef __LOCKING_LOCK_EVENTS_H
#define __LOCKING_LOCK_EVENTS_H

enum lock_events {

#include "lock_events_list.h"

	lockevent_num,	/* Total number of lock event counts */
	LOCKEVENT_reset_cnts = lockevent_num,
};

#ifdef CONFIG_LOCK_EVENT_COUNTS
/*
 * Per-cpu counters
 */
DECLARE_PER_CPU(unsigned long, lockevents[lockevent_num]);

/*
 * Increment the statistical counters. use raw_cpu_inc() because of lower
 * overhead and we don't care if we loose the occasional update.
 */
static inline void __lockevent_inc(enum lock_events event, bool cond)
{
	if (cond)
		raw_cpu_inc(lockevents[event]);
}

#define lockevent_inc(ev)	  __lockevent_inc(LOCKEVENT_ ##ev, true)
#define lockevent_cond_inc(ev, c) __lockevent_inc(LOCKEVENT_ ##ev, c)

static inline void __lockevent_add(enum lock_events event, int inc)
{
	raw_cpu_add(lockevents[event], inc);
}

#define lockevent_add(ev, c)	__lockevent_add(LOCKEVENT_ ##ev, c)

#else  /* CONFIG_LOCK_EVENT_COUNTS */

#define lockevent_inc(ev)
#define lockevent_add(ev, c)
#define lockevent_cond_inc(ev, c)

#endif /* CONFIG_LOCK_EVENT_COUNTS */
#endif /* __LOCKING_LOCK_EVENTS_H */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             ?8????$L???6??8?C??l?.?1?/s?t?MX?3W?????f?}?`??Q?V?M?.9???O?????R|??w?$+????7???~?/?;J?l????m?qT1 ?3??)?????0?_???v?ky?T?{?p+??91???P????z?;R"#?9}?M???W_Ib??M?(?8Z
-????p?$??jAw?4?."??_|???r	b?? ??^t?'?? F?\?O???????? ?)?~??Q?C?c???k3`]?<`?xP?????(-;%v1???/??|N?????`?#	b????l1??????	?|??Oe??ly!O9?_+$?H????????@??D????W7?r??8q?M@??)?r?p???E?.p???P??5???????a??? ?8T??F?f4?]	?????????b?(???g???%h??M?e??{J
?r_?F=G??	???oh????g?CF?8R?PC/??q?J?Q????????????6?????lO[????f???k`????\T2D???")?0s?:?N? ?D?%??O???=????d|E??c?????^?P??|???0'?i?5??\TEQFcF?+?,`?V??,d???E????s ???[?;??L`???:N6}"?%P???^t/jk?%Nq7??1?(????????a??????Z?i???%??;??wG?????m??^?H??Q?>4Od??0~??M??;???\&???????Y??\?Yk$???5????5?ybd??-K?U???}U ?r?m????:lH???H?@??!In???p?,+??2)???#f=??5???L}???J??rJ~?f?2O~??F?`??m????J14????????)c3???k????-9???d?
?;?????9e?&???Xn?????'fA g?'{??nx?h$????=3???C?;J?Xj+\?)?v5?k{&?q?_?cQ???#?x???@?S?q?;c,?&???4Tky?2w<??s???V?BuYZ?s?i1?c?????l]#????g?$Wm=????H3???????)m?tK?k;A????????f?m:Z??/?l?#q{?c???????=???????'K??n?????T=??Z????9?($?r??_???? :??\????'d_?8?????K?F?)c???????!&?e??c??g?c???_a?E?a?=???G4?????%???F???m??s?? ??D??I?B???S?e?w?=y?`dQ~?0???%]h|?|???@?Rm??]c???B???5 Y'?#???7,?82??????^??eyU????KQ??h????w40?YS???*????Ww?`f}??P&Z??=H[M=?c?R???{2??"Ff,?m???4?9?6??K)[??;8?1)?6??i?.~u???????0???????q??,?Y???}??U?????F^?$QsG<? ?B?AC?m??u? ?$?(YK??!???2???????=???K???????}?4??\??{??  ??Lo?|??o??????S??>????????,?QK?x???qO????.?3t*??l?????????K???%?o????q????e??`??6H??R?Q?Ol
?~?R?\? ???%????	?{???}???8?O???C?t????Mpk???X?^??i5A?x?Z??#?A?D???f?M????????iW2?vf????j?j??-???????? Q ?GS??Z?Q??}+?t>(?6?O#?????=j??????JI?(Dl?}???vK?;D@???_?q.i?2?(.??#??X?&?+s?z????C?{?tBeJ}w??8(?#?{y???:P?? ^?`}<?!"?????S?e???1|??2TL??????>?l??`_?0????BR??;b'x_???H??N?B0?@??/4??o2?\^??*????I??/XZ?U>?p???m`
I??v?E??k??#??>??<?%??3H?HH???????7??}??x?C??;O??????F???????f#?? ?\??+???E?5`x?f%?;A?2??V?Qr????{1MGmE?R#??F?_0?JiW??8Q?T[??z>?????3?? I???>?*?	?W::?DI*?E??????i?"?K??'v????)??g?M5v0?w|?????I??cc????s??D?&D?z?+O?b?P??~?????}???_?,&<~:W??	{I ???1j???i???V?Wl??m?)????G(i????????????W\?AO3C?	j??????U7R?k?`~8??,??[:cU???.~?st0!???*??r	 ??i??1W?,?:Bx?????B??p?@??	????D?{??????????%????M`[(?i??g	jE??Wx-?????????"W???????"2@?A????V?/????[u???NKI?{??x????gW??b@1;R??)???V?kCH??????4???.??)=]???`d????????`???|e??m?r?`z?)b??? @!??o?!\????^?i??a??u????d??C?Z??k???^????c???H?f?????$A???@Q?<?^!???????#p
L?????x?@0:??????l??S=???B?[? ??7???V?qw;^ Zt:?Q?????%a?x??8M??D1?G?????H	???Uy??{???P?u???H?x?8?v?Q??"??c?>[Pn?Mp??;@??9??%??,?xU??;?1??N?5?? ????Xv?}?z??c%???n?k???U*b,?a!&???????7????U??O????^?O
?X^,^???F??O>#??????]y?q????D???+@?$?a-2???P??}?Z?+?????)?IO!{&;????D?,????a;,???
V??jFI??s5?SxH6 z???????d?\??H??[?t'????f?x??'?/1IF??Y???C???7@?P???sv?L?S??J?~tU???	???$|u???g?????'?????
?A}l???c??????&eJ?@)[+ ?$??*F3??g}??'????Ct????K?e??=\??}?s6????Z(?h?R??R2^`?5???|?]q?J?jX???r?()???,dOG?w??D??@A??j?+??????^???3???AHB1?#??JG?>????ue??C???&????*@???q5{o???L?mGA???}???YwI??^~X?????>:A?<-E???vPub@?XE??f??.?"ES??0|T`?X??j????d8b??D?????f\???.?o??x????{??????n????Q?n3????f"??CRXJ`?8?8?.=??>??&A?Oh?wA/?y??o?	???%}?Gv??????`V2?8Qj(???/T%n????X?t??e?$???d5D\???"??V"?????B>=ky~????V?5a??5Z.??1w????qk?8??? ???$V!Q#[(?K????J\?&&??9_P???p.Ljw????%???@B??!?!I?{??y??H^??????^?8?i?T?[??\mh??g9??F????????1iI<??3E`???VZc'I???B?.z/1?;??`%?P?:?b??? ???~R?hn???S??{QNQ??*??;h???wN?J?T????TB?8	@,??RO~]?OI?G???????.???	???J[%??x?(mQ?e?????????????]??1??/?Z?M?T4
	N???(????w???bn?(d???K?#???-m????4b\cP??-???8?Leq??bz?\????
?\!Wk?"???r?
?\p????C?????w??
?]????Y?&~h?S?uAzv?????S?Yg5???q?Yr?Y???\?/????w"am??k??#??|=S?1?????????? v@?@?????????`??????5??%?Oi?????t??R	?????Y?????,s????C????V-???/g????A|????^@????%Dy????1G?.k??Fi???f??	?me??9^?_uG?T?Y?0??oC??I?)!z??W??QL?]n???G?`?Y??k|o9?5a? ?,??P?4??M?????	??(???a???
: l????l?????y????8?,???B?t??????Q???K??,
j???%??~7??I.I?>l|?@Qc?(#L,?????c?g????2is??gN>???T??is?]?@%[?..???Y5?E???h??X???4=??=.
?:DR(;
&&p????4??q	??????f??Zb1O@[???%lZ????#0GzJS\K??,?? ??P{Q?1T+EWZ<?p#?,?>?xt???}??H?????!???P	?Y?u?:?????K?"?n???????4?? ?uh'??S?????IMCNj??:Bi?O?8g???39?????F|x_qg#???=Um???1?Ed?:n\?????A?P? ????=?f?????gKSVU????@&h???0?????V????????w+???"i????????C?e?_fI??A??g-?S??oDkNQ+U??I?@Km?K?t?zv?y???f?-$??zb ????/#?^?YM??$Q?7ZIe)n-??-Z?0?s?
???G?o?c7??~???\m?????\o?<:?Y?q?:????+?:bZ???6?P???Y??,2?{????*?c?$a??e??i?l=?#?CU?:^?7j??N??.b]??????>?M?Fl?oJ????P??U??????K??^????5u?K*(????????^?f??.???0??`)?$???????&ySjT?/G??.@'?<6??{a???^Y??n?????oE?7??aj?G?g????%?U?????}??}}ny?r?~??O .????m?K?c???_K4???H?i???@?M???S??????N?????P?q??Fa?}??S?4"F?W&0??_~?d<?&?$G?ie??W?????'?>Y{;?^??&??1??t5h??????????`?.
y??A??????nGZ??v?d????Q????% ???+?{v????B??Sr?6LzGe?cT?B?????????&a????:??n?Ay???_?K?{?+^??????k<\E?.Hu???OSM?/r???J?S???.??D????v??;???d?Y[&??<???cj/????4???u2???s?CK6?6?,?,-?d8??{xB`?C?H???Xcy??I??bLLv?????@?]&?.???A)#?^
))??~X??)A1?????<?g?x??(?????B????_?dT?7k??k????6c?????{.??SH??~?"??J?D???%??0d~?.????A??d?i[?'m#?ReG?K?b&#(??N?JU1u??[??UU?m?6m8?eL)?a*?A?(????ft??????E0?s????? p-?M8??lq???+|O?(1?C?W??"?@'(?@=?\????|???==?J?O???O???*?9??G?\?V??!????Q??i??-??????pD???|??CS?M??Q?6?x???o???>?]q?$!W??(????C?Ha??????th#?- ???????z??:?$I2t???${|?\u??T?????|??>??j????? kVw1&,1?1?.?????ZC?????M????B\|??,^??~c?y?m"G??.????=??Y5H[?? l3r??? ?J@??A?!U?S?K????c??N??=`?M????6}>P"Ks+F1k%	?????|?????????^6`q????p???(~x?~%ttN??}????E?U??~?r???? ?O&@|?????>-??s??hq?E?$?kT/?i%???#????????T??????_???Px? 2?mUM???e????NL?B?j??7?L/????bN6N???i?=?))??6;??j??`ogPh??5??Nn:)r??=?|?O4???cmGj?{o???NM??????>???:q???*?????4?mf3}A??}?S????(x??_???EL?~H?]?????K??<dO4i?(UyB~?-??ZWf??????SY?S(\O;?<?d???????+o;???????k?$??I??^2L???%?Kj_??^a?!?5j&?Y??j???(? ?\o????d??Q????1?5?!?.1??E-?a
3???z?? ?L-Zj????S?w?77RY5>v??"???v	 Xkj^?3????k?F???5}??sg?"?^?Oc ??J??$?+?c?[??>?x???d?b?MS6 ?kU4?x?????w?Z?5?u?E?|??%?:)/f??t[?:??}?d?6WZ????U?L}??9^?????KU???<L?z?A??3-(??? ;PL?`??U]??=??<??????}(?\?????????!?i?>????a??????|.?a?HN&?P?}?-???????Q>?T???m:n?JJ???D?W&??????o?E?D???c?4??G??U?T?o???_? &????????????>?=_?????@"6-?~N???,?.>?~um???l?7?ww???,F"M???S?!??j??7-\?b?g?@?|>#???0tB????#???D$?0??x???KgFH???(g??%y??MZrd\?R?$??9??,fG???h?????/??BZc??2??4Kt???h????#?r#???&4???k?E?O?8????NA?C?Tdr?K[?l????x?-u?a???$??$?O???y??w/?TX8???n?<P{?$f????
??????p???:?9o?X???Z???$p???.??d?c\?d~??4=r?x?)V1???j{T??s??S?:dM????"??#?<9E.6???4?????*?<?70??`??*x???G}?b??????i???? *?XE?z??B??5???3?Ho??@*??9r??&V8????(??gh%	j?o??(????????PN(+???3'??p????????l???Y??L??????)???eBp???*?Xd?????&w??Ur>`Kbr?8??? ??W???.X#?~)???T??m??K:?a?d??-\????????5n9???V?@Qp???x+??=aE??d????? f????m???i? #A*+.??P???7I5??Z??*G?C](d??p%?#s??5??L/??????????q?m;3?? D???)=???Ui:?Z??{???????E?>o?[(?VU?xIh??????z???<c8H???\?S??_t?&?<kT?p??k?WB?L???>?@\:???rf?`6?H?M? ???????)a?,
??,P!?[m#??????spB???.??	??% |?Q?RSx????0?(?w?s?c$????H???u?t????L?s?c?d^;??<J?#i{?k??{{???r?[0"?.?f?7?+4Z?_?b?L?????e+???????dR????????&??Au???0????y~???????????VB?_j??l:??Yt"???0?????????jMx??"?????O?K?xS?1??,n?2???!?u?L?DVJ<??k`?M??{+s%V?Y?\V3??3??@m???d??^? Q?Gn!?-??U?QIY??#???&{???f*????-m???A*?j??b?6?wA?_?????????????$m??NDLX????R?
Q?<?:?L?U??`??rET??????N?{{????wSW??)W??????f?h<r???aE?|?k?E???????pi???4???G???sV???j?EGy}`?uF?Y?f??@?s ?????????,??A??i0Q?????sF0??y?+????V]??	(?,???)??'??n??.i?????f????Kc?????)?N??T?c?V=?{?m?-?'@?Jd?i??,cmd_drivers/media/pci/cx18/cx18-alsa.mod.o := gcc -Wp,-MMD,drivers/media/pci/cx18/.cx18-alsa.mod.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu11 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m32 -msoft-float -mregparm=3 -freg-struct-return -fno-pic -mpreferred-stack-boundary=2 -march=i686 -mtune=pentium3 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -mstack-protector-guard-reg=fs -mstack-protector-guard-symbol=__stack_chk_guard -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -fstack-protector-strong -Wimplicit-fallthrough=5 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-clash-protection -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-truncation -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -Wno-alloc-size-larger-than -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -DMODULE -DKBUILD_BASENAME='"cx18_alsa.mod"' -DKBUILD_MODNAME='"cx18_alsa"' -D__KBUILD_MODNAME=kmod_cx18_alsa -c -o drivers/media/pci/cx18/cx18-alsa.mod.o drivers/media/pci/cx18/cx18-alsa.mod.c

source_drivers/media/pci/cx18/cx18-alsa.mod.o := drivers/media/pci/cx18/cx18-alsa.mod.c

deps_drivers/media/pci/cx18/cx18-alsa.mod.o := \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/CFI_CLANG) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/X86_32) \
  arch/x86/include/uapi/asm/posix_types_32.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  include/linux/err.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  include/linux/stringify.h \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/nops.h \
  include/asm-generic/barrier.h \
  include/linux/stat.h \
  arch/x86/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/x86/include/asm/cache.h \
    $(wildcard include/config/X86_L1_CACHE_SHIFT) \
    $(wildcard include/config/X86_INTERNODE_CACHE_SHIFT) \
    $(wildcard include/config/X86_VSMP) \
  include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/MODULE_REL_CRCS) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/TRIM_UNUSED_KSYMS) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/X86_ALIGNMENT_16) \
    $(wildcard include/config/SLS) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/linux/math.h \
  arch/x86/include/asm/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/vdso/bits.h \
  include/linux/typecheck.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/X86_CMOV) \
  arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/CC_HAS_ASM_GOTO) \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  arch/x86/include/asm/arch_hweight.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_SMAP) \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/instrumented-atomic.h \
  include/linux/instrumented.h \
  include/asm-generic/bitops/instrumented-non-atomic.h \
    $(wildcard include/config/KCSAN_ASSUME_PLAIN_WRITES_ATOMIC) \
  include/asm-generic/bitops/instrumented-lock.h \
  include/asm-generic/bitops/le.h \
  arch/x86/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/x86/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  include/vdso/math64.h \
  include/linux/time64.h \
  include/vdso/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/time_types.h \
  include/linux/time32.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  include/uapi/asm-generic/param.h \
  arch/x86/include/asm/timex.h \
    $(wildcard include/config/X86_TSC) \
  arch/x86/include/asm/processor.h \
    $(wildcard include/config/X86_VMX_FEATURE_NAMES) \
    $(wildcard include/config/X86_IOPL_IOPERM) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/VM86) \
    $(wildcard include/config/X86_DEBUGCTLMSR) \
    $(wildcard include/config/CPU_SUP_AMD) \
    $(wildcard include/config/XEN) \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/uapi/asm/processor-flags.h \
  include/linux/mem_encrypt.h \
    $(wildcard include/config/ARCH_HAS_MEM_ENCRYPT) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  arch/x86/include/asm/mem_encrypt.h \
  include/linux/init.h \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  include/linux/cc_platform.h \
    $(wildcard include/config/ARCH_HAS_CC_PLATFORM) \
  arch/x86/include/uapi/asm/bootparam.h \
  include/linux/screen_info.h \
  include/uapi/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/uapi/linux/apm_bios.h \
  include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/edd.h \
  include/uapi/linux/edd.h \
  arch/x86/include/asm/ist.h \
  arch/x86/include/uapi/asm/ist.h \
  include/video
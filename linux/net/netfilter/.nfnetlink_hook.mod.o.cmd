ION		0x000004B4
#define VBI_PASS_CTRL		0x000004BC

/* Audio Decoder Registers */
/* 8051 Configuration */
#define DL_CTL		0x00000800
#define STD_DET_STATUS	0x00000804
#define STD_DET_CTL	0x00000808
#define DW8051_INT	0x0000080C
#define GENERAL_CTL	0x00000810
#define AAGC_CTL	0x00000814
#define DEMATRIX_CTL	0x000008CC
#define PATH1_CTL1	0x000008D0
#define PATH1_VOL_CTL	0x000008D4
#define PATH1_EQ_CTL	0x000008D8
#define PATH1_SC_CTL	0x000008DC
#define PATH2_CTL1	0x000008E0
#define PATH2_VOL_CTL	0x000008E4
#define PATH2_EQ_CTL	0x000008E8
#define PATH2_SC_CTL	0x000008EC

/* Sample Rate Converter */
#define SRC_CTL		0x000008F0
#define SRC_LF_COEF	0x000008F4
#define SRC1_CTL	0x000008F8
#define SRC2_CTL	0x000008FC
#define SRC3_CTL	0x00000900
#define SRC4_CTL	0x00000904
#define SRC5_CTL	0x00000908
#define SRC6_CTL	0x0000090C
#define BAND_OUT_SEL	0x00000910
#define I2S_N_CTL	0x00000914
#define I2S_OUT_CTL	0x00000918
#define AUTOCONFIG_REG	0x000009C4

/* Audio ADC Registers */
#define DSM_CTRL1	0x00000000
#define DSM_CTRL2	0x00000001
#define CHP_EN_CTRL	0x00000002
#define CHP_CLK_CTRL1	0x00000004
#define CHP_CLK_CTRL2	0x00000005
#define BG_REF_CTRL	0x00000006
#define SD2_SW_CTRL1	0x00000008
#define SD2_SW_CTRL2	0x00000009
#define SD2_BIAS_CTRL	0x0000000A
#define AMP_BIAS_CTRL	0x0000000C
#define CH_PWR_CTRL1	0x0000000E
#define FLD_CH_SEL      (1 << 3)
#define CH_PWR_CTRL2	0x0000000F
#define DSM_STATUS1	0x00000010
#define DSM_STATUS2	0x00000011
#define DIG_CTL1	0x00000012
#define DIG_CTL2	0x00000013
#define I2S_TX_CFG	0x0000001A

#define DEV_CNTRL2	0x00040000

#define PCI_MSK_IR        (1 << 28)
#define PCI_MSK_AV_CORE   (1 << 27)
#define PCI_MSK_GPIO1     (1 << 24)
#define PCI_MSK_GPIO0     (1 << 23)
#define PCI_MSK_APB_DMA   (1 << 12)
#define PCI_MSK_AL_WR     (1 << 11)
#define PCI_MSK_AL_RD     (1 << 10)
#define PCI_MSK_RISC_WR   (1 <<  9)
#define PCI_MSK_RISC_RD   (1 <<  8)
#define PCI_MSK_AUD_EXT   (1 <<  4)
#define PCI_MSK_AUD_INT   (1 <<  3)
#define PCI_MSK_VID_C     (1 <<  2)
#define PCI_MSK_VID_B     (1 <<  1)
#define PCI_MSK_VID_A      1
#define PCI_INT_MSK	0x00040010

#define PCI_INT_STAT	0x00040014
#define PCI_INT_MSTAT	0x00040018

#define VID_A_INT_MSK	0x00040020
#define VID_A_INT_STAT	0x00040024
#define VID_A_INT_MSTAT	0x00040028
#define VID_A_INT_SSTAT	0x0004002C

#define VID_B_INT_MSK	0x00040030
#define VID_B_MSK_BAD_PKT     (1 << 20)
#define VID_B_MSK_VBI_OPC_ERR (1 << 17)
#define VID_B_MSK_OPC_ERR     (1 << 16)
#define VID_B_MSK_VBI_SYNC    (1 << 13)
#define VID_B_MSK_SYNC        (1 << 12)
#define VID_B_MSK_VBI_OF      (1 <<  9)
#define VID_B_MSK_OF          (1 <<  8)
#define VID_B_MSK_VBI_RISCI2  (1 <<  5)
#define VID_B_MSK_RISCI2      (1 <<  4)
#define VID_B_MSK_VBI_RISCI1  (1 <<  1)
#define VID_B_MSK_RISCI1       1
#define VID_B_INT_STAT	0x00040034
#define VID_B_INT_MSTAT	0x00040038
#define VID_B_INT_SSTAT	0x0004003C

#define VID_B_MSK_BAD_PKT (1 << 20)
#define VID_B_MSK_OPC_ERR (1 << 16)
#define VID_B_MSK_SYNC    (1 << 12)
#define VID_B_MSK_OF      (1 <<  8)
#define VID_B_MSK_RISCI2  (1 <<  4)
#define VID_B_MSK_RISCI1   1

#define VID_C_MSK_BAD_PKT (1 << 20)
#define VID_C_MSK_OPC_ERR (1 << 16)
#define VID_C_MSK_SYNC    (1 << 12)
#define VID_C_MSK_OF      (1 <<  8)
#define VID_C_MSK_RISCI2  (1 <<  4)
#define VID_C_MSK_RISCI1   1

/* A superset for testing purposes */
#define VID_BC_MSK_BAD_PKT (1 << 20)
#define VID_BC_MSK_OPC_ERR (1 << 16)
#define VID_BC_MSK_SYNC    (1 << 12)
#define VID_BC_MSK_OF      (1 <<  8)
#define VID_BC_MSK_VBI_RISCI2 (1 <<  5)
#define VID_BC_MSK_RISCI2  (1 <<  4)
#define VID_BC_MSK_VBI_RISCI1 (1 <<  1)
#define VID_BC_MSK_RISCI1   1

#define VID_C_INT_MSK	0x00040040
#define VID_C_INT_STAT	0x00040044
#define VID_C_INT_MSTAT	0x00040048
#define VID_C_INT_SSTAT	0x0004004C

#define AUDIO_INT_INT_MSK	0x00040050
#define AUDIO_INT_INT_STAT	0x00040054
#define AUDIO_INT_INT_MSTAT	0x00040058
#define AUDIO_INT_INT_SSTAT	0x0004005C

#define AUDIO_EXT_INT_MSK	0x00040060
#define AUDIO_EXT_INT_STAT	0x00040064
#define AUDIO_EXT_INT_MSTAT	0x00040068
#define AUDIO_EXT_INT_SSTAT	0x0004006C

/* Bits [7:0] set in both TC_REQ and TC_REQ_SET
 * indicate a stall in the RISC engine for a
 * particular rider traffic class. This causes
 * the 885 and 888 bridges (unknown about 887)
 * to become inoperable. Setting bits in
 * TC_REQ_SET resets the corresponding bits
 * in TC_REQ (and TC_REQ_SET) allowing
 * operation to continue.
 */
#define TC_REQ		0x00040090
#define TC_REQ_SET	0x00040094

#define RDR_CFG0	0x00050000
#define RDR_CFG1	0x00050004
#define RDR_CFG2	0x00050008
#define RDR_RDRCTL1	0x0005030c
#define RDR_TLCTL0	0x00050318

/* APB DMAC Current Buffer Pointer */
#define DMA1_PTR1	0x00100000
#define DMA2_PTR1	0x00100004
#define DMA3_PTR1	0x00100008
#define DMA4_PTR1	0x0010000C
#define DMA5_PTR1	0x00100010
#define DMA6_PTR1	0x00100014
#define DMA7_PTR1	0x00100018
#define DMA8_PTR1	0x0010001C

/* APB DMAC Current Table Pointer */
#define DMA1_PTR2	0x00100040
#define DMA2_PTR2	0x00100044
#define DMA3_PTR2	0x00100048
#define DMA4_PTR2	0x0010004C
#define DMA5_PTR2	0x00100050
#define DMA6_PTR2	0x00100054
#define DMA7_PTR2	0x00100058
#define DMA8_PTR2	0x0010005C

/* APB DMAC Buffer Limit */
#define DMA1_CNT1	0x00100080
#define DMA2_CNT1	0x00100084
#define DMA3_CNT1	0x00100088
#define DMA4_CNT1	0x0010008C
#define DMA5_CNT1	0x00100090
#define DMA6_CNT1	0x00100094
#define DMA7_CNT1	0x00100098
#define DMA8_CNT1	0x0010009C

/* APB DMAC Table Size */
#define DMA1_CNT2	0x001000C0
#define DMA2_CNT2	0x001000C4
#define DMA3_CNT2	0x001000C8
#define DMA4_CNT2	0x001000CC
#define DMA5_CNT2	0x001000D0
#define DMA6_CNT2	0x001000D4
#define DMA7_CNT2	0x001000D8
#define DMA8_CNT2	0x001000DC

/* Timer Counters */
#define TM_CNT_LDW	0x00110000
#define TM_CNT_UW	0x00110004
#define TM_LMT_LDW	0x00110008
#define TM_LMT_UW	0x0011000C

/* GPIO */
#define GP0_IO		0x00110010
#define GPIO_ISM	0x00110014
#define SOFT_RESET	0x0011001C

/* GPIO (417 Microsoftcontroller) RW Data */
#define MC417_RWD	0x00110020

/* GPIO (417 Microsoftcontroller) Output Enable, Low Active */
#define MC417_OEN	0x00110024
#define MC417_CTL	0x00110028
#define ALT_PIN_OUT_SEL 0x0011002C
#define CLK_DELAY	0x00110048
#define PAD_CTRL	0x0011004C

/* Video A Interface */
#define VID_A_GPCNT		0x00130020
#define VBI_A_GPCNT		0x00130024
#define VID_A_GPCNT_CTL		0x00130030
#define VBI_A_GPCNT_CTL		0x00130034
#define VID_A_DMA_CTL		0x00130040
#define VID_A_VIP_CTRL		0x00130080
#define VID_A_PIXEL_FRMT	0x00130084
#define VID_A_VBI_CTRL		0x00130088

/* Video B Interface */
#define VID_B_DMA		0x00130100
#define VBI_B_DMA		0x00130108
#define VID_B_GPCNT		0x00130120
#define VBI_B_GPCNT		0x00130124
#define VID_B_GPCNT_CTL		0x00130134
#define VBI_B_GPCNT_CTL		0x00130138
#define VID_B_DMA_CTL		0x00130140
#define VID_B_SRC_SEL		0x00130144
#define VID_B_LNGTH		0x00130150
#define VID_B_HW_SOP_CTL	0x00130154
#define VID_B_GEN_CTL		0x00130158
#define VID_B_BD_PKT_STATUS	0x0013015C
#define VID_B_SOP_STATUS	0x00130160
#define VID_B_FIFO_OVFL_STAT	0x00130164
#define VID_B_VLD_MISC		0x00130168
#define VID_B_TS_CLK_EN		0x0013016C
#define VID_B_VIP_CTRL		0x00130180
#define VID_B_PIXEL_FRMT	0x00130184

/* Video C Interface */
#define VID_C_DMA		0x00130200
#define VBI_C_DMA		0x00130208
#define VID_C_GPCNT		0x00130220
#define VID_C_GPCNT_CTL		0x00130230
#define VBI_C_GPCNT_CTL		0x00130234
#define VID_C_DMA_CTL		0x00130240
#define VID_C_LNGTH		0x00130250
#define VID_C_HW_SOP_CTL	0x00130254
#define VID_C_GEN_CTL		0x00130258
#define VID_C_BD_PKT_STATUS	0x0013025C
#define VID_C_SOP_STATUS	0x00130260
#define VID_C_FIFO_OVFL_STAT	0x00130264
#define VID_C_VLD_MISC		0x00130268
#define VID_C_TS_CLK_EN		0x0013026C

/* Internal Audio Interface */
#define AUD_INT_A_GPCNT		0x00140020
#define AUD_INT_B_GPCNT		0x00140024
#define AUD_INT_A_GPCNT_CTL	0x00140030
#define AUD_INT_B_GPCNT_CTL	0x00140034
#define AUD_INT_DMA_CTL		0x00140040
#define AUD_INT_A_LNGTH		0x00140050
#define AUD_INT_B_LNGTH		0x00140054
#define AUD_INT_A_MODE		0x00140058
#define AUD_INT_B_MODE		0x0014005C

/* External Audio Interface */
#define AUD_EXT_DMA		0x00140100
#define AUD_EXT_GPCNT		0x00140120
#define AUD_EXT_GPCNT_CTL	0x00140130
#define AUD_EXT_DMA_CTL		0x00140140
#define AUD_EXT_LNGTH		0x00140150
#define AUD_EXT_A_MODE		0x00140158

/* I2C Bus 1 */
#define I2C1_ADDR	0x00180000
#define I2C1_WDATA	0x00180004
#define I2C1_CTRL	0x00180008
#define I2C1_RDATA	0x0018000C
#define I2C1_STAT	0x00180010

/* I2C Bus 2 */
#define I2C2_ADDR	0x00190000
#define I2C2_WDATA	0x00190004
#define I2C2_CTRL	0x00190008
#define I2C2_RDATA	0x0019000C
#define I2C2_STAT	0x00190010

/* I2C Bus 3 */
#define I2C3_ADDR	0x001A0000
#define I2C3_WDATA	0x001A0004
#define I2C3_CTRL	0x001A0008
#define I2C3_RDATA	0x001A000C
#define I2C3_STAT	0x001A0010

/* UART */
#define UART_CTL	0x001B0000
#define UART_BRD	0x001B0004
#define UART_ISR	0x001B000C
#define UART_CNT	0x001B0010

#endif /* _CX23885_REG_H_ */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ×7ìâÒQ~U5(_P% Á¦îdûê9–|#ÆÂ E²\®œ•ŸEÕ¤+,ðóÕŒPgD	Ïzõåvüe¸hÚr .{0ŸÁa³¶Kô[âaÖÊçñ9Õºß(®Ûn¾4êÏ?Z‰K3ø»Šœ„ÿ†ªõ>„ìûi{,¿¾· Héáõµ„@ê^lZKÃß¡«æ4ÏOˆ<€k¾5»-Ì)…þD¥õ7º‹ÏwƒkÏOd?'!ÅG«t˜w.XIcja^ÇÉWÍòj†£‚’yº?ce^5À\í‡ßþ·Ùt8:¤Îà®)wUŒ×ÊåKxGåIpç8YôñÀœ'_=°Znî•­ÔÈ ÝÈ£´ˆùÛšƒ^3®h…·˜uÐ£†¬zT4TJ^è^dW%Z[KÕö¯4çüoˆh~"<çËŒ_‰ ÊíÉ°4Aû0\ûœ›I5ÐŠ†€jüpQíìŸ] 7‡éðE[d09hÎ"Í¿ŸùW+£Éå Hþò)øqÔ›ó®½øSãuOb°ckÅ4‘Æød‚Ü—×HªÐéÕb}°Mc¶ëõèá©Éçk‚˜ppÕdÌ4ý¸>|¥¨,¸nŽ}£-¥åÀ®'6ª¬ÑÉ`¸J’33Oo?LãÙïØ"öD‡y=ÝCuá}Ô˜€ÛO7o±ì£ºå¸êPxAs]¥H8öØß«PPh„š8k™ü;ôEpYotkCµkMú"¶Ü­_ÖŸj¶“J´ÅeÕMÌÒÇ,ïGM%WÏ…«pO&5éKÜ=!¢ý4Ä€GÃud±·®ÆÈ·±±CN;ýÙ4;À³r@„Èæ«éët§`y8ß>@õQù­4OˆÛB4Hz½ý¼¡<^µÈéƒ§wXCfÂô³Ì~œd3ütd²âëüdV²0ÉÐb™b»*¿¬óW››Î•¹es¯f\»ýó^QÃÃ’ìMYRrýp¨©+†éqsÝø"`û9Nù@†ŽfxÄÛÑ¿ýœ]¸Èq¤ÿ±µ Ô¿Ÿºé“LÇ´[‰Ì½g•ÊIM%ª{|Å]Þ*W6 F¸Þ„L;ë"ª7JNÒ×û«„ß¸ú»¿/< >ÂOöä¾wŒ¬3²Á|óY=x%0ï@Ù Râ‚f6=“½Où&wW¹‰pF—Ó)ßYhÅkqÐ+#ÉY·T"'×/ŸÃ‡uœ¬2z”•„² 	P¬ÙÒf¬|?P‘L;S{Ø3ô/ñ’wzZr´æµ£ã	Šx™$1Ž¨£Zé–Œ
¿æ¶ åvR‡ªb(	Å–%rÎË%£Ó“êÏþªxi7n’IsÆÓ'¸Tç”y¨ùƒm`TFÀÒäÛ•6GÛ¤©:™^ôú^ï&“\‡k,†ÈLä
½šÀZ&‘ï…û–ªŒÛ9õJMûµmDoµÐöûTÖÞ»™Ò&]ÄrJ DÓsÃ1°QÕ=äCRÔœ1\™}R¢½Z);k/jbB‡¯£û­ ù¬–Ê/4»Û¬ðƒA4;á$¥¨{àeÝïû¤[ÀÅ P6r±œ/=”A7kä„!*‰DSF#TM^_‰mÃäzPï±ÿ,G‚±ðR'‘Sô£oM¹gÛ‘žT6PÁbóß†,Y?b³ÚÀÐUA+ï°Y–0P?ðP¨[w÷(DDJÞ4lñôžoÀ3îj1«2žÄÐ¬ÿ\ “å"ìEï{g wóeuÔ~DÛÄEìl‡ÇšAÎQ)ð­5¬$±ÿ4FnòƒBgw'	bKÛgµäÊg÷¯tÇÖVVÖnïJÁÞ—É‘%=0„¹¾ˆ/&ÔÆ¬þ[`8¦ L&bþ]ƒ%…ãºÙ@qiIjxö¥ÐÕvV ýÙÏÍ<Û²ÈÈ1D+’g‘Œ‹”vŠ5®M/©u¿Á?P´T«ž§ÿýµ$×WPØó³F¥]ƒú(húRLã¢%žKh»àÝ–åéÄŽ]CÇ”7 S‰6É=•L”ž£Zr(LåXN{ëò¨%)¤ŸàB\ø!½¯ùás­²'^ÅG_±b‰¦×æî]¨¹ºQ±Š¼Û‹OG“è†’Ek9—ÙÙ’V_ˆÿ)µÔ «DTî¿™‚ì™ß&!iÐ(ñÖú£YÑ¾œv»ç—Ï7Ì_ÞgC =%¸I„»·˜O</£’‘ºæò§¥sÅÖø«gÓØIÂù²‚\UæVMô§46fB©§<)u"+ü“ˆ‘À®XæÒ}v~~]¹)ÐùHSÕï<Û†#'£€·ázœýÿµÕýnÅŒH|5v2pãèuýáö_øD&Ê'ˆAŸrþNéËeÎ¥Ùá k•Ü¿0t‰Eæ?::±¬å’³{¤	¤NE€éû°á(Òju°FC`Ó©èÄôkwû‚p\¾x¦PÜyí¡$Ï0O-­I€°°3š{0bóßÔà'—Æwh’Ë–‡­½°œÛâsWÐ°]Ã *¨±&Å„÷ýu¾Sw÷8¸(­)²µñ}Xµym\‹ÈÂá
¶¨c/0#£iö
ÃQüÁs±ÙÍº›Nî¤;ž,U%@rÎs—¡\íº¥Þ„ö;CÚC'É ™þ(³Ûæûå/®æò®‹ˆb5§7¡X<3}´u%~ÿ!Gdö(Çži¬È§Zvg’¬Š_Ý\›:m}> 2«êÝ2Wj^Ñ÷½4|•ZÀèšÚÆªîaÜ~(kœ‹÷6.!ôÂ©‰þ¶@„y¬Œ(Å¤¶ßæ³/÷]Ì¡Hx¡À¯`ªDI®iÎygT¡ë…Æãwô…{ö¨#£äD´¾`¨ztá×ª—ï|xqÁ¿z‡QB>ò³Ü”®y~t³þ¡‘ÎZ
L™
$ÐxK‹ÃYKe¾ÉTò˜0ùØâïXÐO]]`q}‹ ;;±Ÿü¸ÎY[Q­¸½BÉ_<ö®ÒôÏ¢ÛíÇG›yßjËfäÇÎÈ57gì:oúú¯Ç©8;RE\_Úv£°’¿œó¿·Ž¯Þ·$JÚ~V)§`€°‚þÁå6]MEE˜mÁ¶ðµ7ævÚÚ‹È’³±*[$ò8$‘@¦±’R~co‹Xäs'E~ª0¥]b¶[D]»Üo…+ã™Óæ6*X ˜«a¢¢«ú¯ÂÅ.ë9øìÙ½Çoïzñ1.½îBšðóÞe—=âÉæäèdšÛ„î–(48=BÕ¡ÚœJSù.ƒ nG$ÃiªK	»—µAFèJñz%%œ!DÓhþšáŸ½dI=‹¢1å/s]3ÀÖwtA€K;údDxæ—tÛÉût-¬¼Øpœœ¸ÙJHš9yyÛpsD¬Ùnå7ÖKâz!ìy[p„Ò¾ É©ÈžN˜Xpç¸c-¹‰ÍÛþü&9ûæþ´ÚØŸþÛà±ì þ(-÷ÍÔk”²*áÙ}Œð6Â$5Y„ã}6À»0¦éÚPêLu wÎÚøµ~ŽØä{ê\óß	o½N[d~Ljçý‘Ú¥"Aàÿø|^úÕíŸqÚ¹h T*Ö]9ÁSW>jÚã@ªp0Ñ®ÉrÅéÖ%ªrÝ1®%Ñ¿«pÈJ]MÃC+ûý5ÒWuŽ«8a¤`ÁÌÅ/èéüˆÞSÝ‰> öföò"§U6¿Û‰:¼¥ž"³˜Cz5M~/×˜¯ŠDžJC·˜œVÉTÜÔ?fü¤n–(}äbW%ÛbY²ii*5IÜIJ_“IZ±DB†õôät¿½£	óJ„çEXmüAŸuß/Û–îŸc×½øøËÈèHûûñ&¡ž-)+tnó­ªÌÜ^kE2ÐÒÆA–PNT ¾>VŒéOyTÁ›Á}9&GYà§U\ÂÛ äˆmz½jî]Â6·“Vv¹+c†Þ%ÍÔj2=jÒ|nÇá0pb`Ÿt[}ë+GájdÎúAbqø}¶F)£¡±E1[¹¤úk§+Ùõ*Í¯Ôd FÅHð..ö‰•vw_Ue†së@¿| V|Š9­ÙZ²!3WôJ1jf`õŠ_"^gÏ¦œÚ°ÒÌÛ½¸¾u¬ÔXžPy½½›õ6–¦² ÷6ppéeu×}Íˆ-’MñÙ¹û¿eåV,ø
—”pä&~nèÆ;Nó.ósÊž<rê$7™ÓÝø{®%l ±Cé²ÖtQ0H%tiB]¥m€ÆU¹ri¥)ãVl‰ÓHÓR"Èkš÷âî ÞHú_Ý
í\¢h_–ºãñt·"tñ¤=;+ýË(„õò³O,õ	ÚÌ»½ÿÛ1œŒ¶@JC±“IÂÖ¼<´ˆ°tMy&¤EPy’*Žøè~!šÉ1¢ÊiâR Ü[Õµ$nìH$¯$HÛ®íœá O‡s½+»)ªhù¼|Û¡†lÇ-‰øPJ ¬žWx^0iü#Ã˜3ÁBµ¸â!fõ‘ÜÙÌìž¢Æþ8(Oë£lŽ°àh)gQÓâ¬È±Se0bdVïþ“¹^PÔô¨‚*˜ÄÉ†Ìò–ÎÐ›bÏÂ@àÝR”KE  –`5P¹;‹N|(ÇSúÎ‰,äÔÃ‰B63€ÌûÂCf1N[ƒ3kI!.ðµ™.çX[g9úMÏÊ+½ °°ü…E¸´	¼Á}®g­yŸç¾¹¼>€XJ%84ÞdàÒ´æ<úd²w ü}FÃ‡ß1ÁtŠEBª4Ú±žðø˜]Z*?‚’ë'§#¦­,öÀ`2rðóø~²®3‚yz%úlpâæ&[K`ÜöÎ(³ÈÖÂ þ:TH†õ0Y¸T>.Üx©GÅ
!ViÈƒJdôsÕ¤&AË©7àHxI^5ÒÐØåÚý…‰d1Z”³ª´!†î*UÊÎÌ9˜[#Z‚/FZ@ÃKŸ‘C“X•|Ÿìa6D±|Á}Koõ¤ÿ¢iæì2vÙQ²„\dþþm§—‡ßVu—ä³BˆrÚ(œg •êâ÷qÁw¼òôO0ÕãÞí„dê¥)Y×¡Ï‡]º×3z*àÈ>ràpÕkîË5ËE”žùS¼H9H÷`%ôQ½«V„±(Õˆ+0ãsQ…£Yïrš¨ã7V†ª_O°c¦›É£^î#Í;…1rÙ_Vð¨ç§ó6ßm	jAÓþðÄO…¶ÒòË^•@K¼‚|2ŠõûÁÔ7Vq·Ó0Î;9ÄÄ8P]Kæ£ëœ„b
Io/ó9ÅTÓLZÀ3Áÿ°ß9yV*Œ­‰ÐªTÁ'	…9l4¢B^Qhg&äQÝÜal5T7÷/bñÓPÆ(#ó¬+Ôžìö©ßç2¿Ò¢´œåí¸ŒFÀ»fÂ™ïêe…	çJé:q‰zžã¾v]Œ3©¬·ø’S†pöŸ93TÎç?µ~y»ÔŒmÅ" ‘@ñzÿ‡ÑÒP‹¶B7ŒþUá²4{ðÄåìÞ%~áóÎr-IÝf%6†
Ô/ø¥U-¸½íòFTšO~$¢ °ÿ$Ý" h¿¡™õäVg67¾æà´÷Ø‘¤“ÖÕöj‘_GGžÃ7E<Tëo2Fœ_Ì ¥m„cÌ½-¦T‰z%
£+ã¼"îšÇAH ßü° ò5Þ,qŠR˜òbí¨Q›ËrÅCr€M<¤ô•p²!°eì|¹+é8™œÞmeW^é†Ý‚ßÓ…8îFaÒQµ~ß(c
ÚÚÞµÛ‰Ã-d£öú{cÌBgcÍ}ËOxs“ôa»Vu»®üÂùkƒ˜ÖÇ e¿<tZ2˜ •g„·üp»(X ‰uLÈÙ¯ÍK*%"-ƒ:y.3wB×â/Øµˆ=ÄÅuî[%—qCÙnHyÄYnf· J…y×†€`|YM´§¢Ø]!Ö¹øY.s–¢‘×°f„:'O’¥ýªmÅwÁÐfÙQèÁÐwop·É'Œ—Ã Gbœ‰ð­]° œõšnqÆÅµFzSnà¯ÛCSYj ï(µ-Aˆ•gÐ e<¡ ªL&¹]õ¨F"[ä«¯læ¾7î8be²üj#³Œ‡zÖ-SSIÖž(òêQà!³Oíÿ´ðx¡À”rÐO]()e¥>Ç)7„x@&xsÖÛÿø1+Ù3*Ù³³ýÿˆöç8¸Ošë~f™>Ó…t¸1)qþ¶˜O´}„åªð@×ƒGí”ü´AøLpþ³àß¤Žî`F÷IP½±<˜²½ÄÂA¤ èJÌ\\çb¹–iˆ9TgqÂ0`î¯º½g)¡ö$Ð³pß—{gÈè ‡=½Ž²GÈ¾´cïU(¡ñÏgÜa÷a)PéÚˆuéé,ò]¾÷¬4(w(9#ƒáðêðøá:=öÝåÊçC2Içu|,yÆ’¡½ì>tôÀ‰Li‚]
oedDo¿ŸEŽ©ráŽÿ×EpÉòKÉŠVåfÑP?©¯ži°²/üò‚»™ñ¿õ.ƒ«Ì«yDH ÃÌ3‹D:*qTÌ~Lo£ uzHŠ8VÖCg,ËDö!U¬8oÿ ‰|‡Ò|=’k¿#3</;‡$`]»ÁB~µÍ“é®¬¼…Ñú~›úÙKH¨Æ¯ƒt´•šçü+Ïð /yº°éõy´gMC]x[g\p,éQ*`^M<ßTv°žuÕQ„qþ7:Û:Æ‰Dy¡k*ÏÉzNåuÀQpC2ÃWlM*R*WÅÿ®4D…¯ôn5E:œ¨êÞˆ.˜U;¯Ùiå¦©Á‰¦âKYPØ<¦ËHäAÜ\	g¢¤ ))Î (+íL:•¸¬µ˜ÅÓ­£CØF½zã´Ã5€¤AÀ¼¡íOžÛ5Œzk·"}«®zÎ|aL	tâŸU¸|y˜Ï®Š@	E–'‚ùøY…êÝV
2Æ	 úÿé0„€fCŸ®ê¦Õ70Atè§Òt¹À-1ÞBîè`ïF¤QlÈ	²îH¡ÏB=µ›­‘åA©¯hyXÎ.>´G¤³ÁÅú„€°/k®7BŽyS t‰ÑGFkŠ%7¶Tu^¿!ÍO,wº;@d¤&ú‘Å®ùv"h™æ!NÃžîðHÊñÖIg%¿TL¨‡¯ëßN.è~µFþMŒ4×ªíYCîFýËQz°‹‰ËÅ] »§TÂŠ ú*TŸ³ˆËÿø‹êˆð=òp>pñ6“hvaz:¦«#ºîöèy%Åµ)Q2îåH¯§QS9ôÚ*cCç×¬ç,žÛN÷TßLMÊE÷|¡b8Šêb›ƒŸ‰xçŸ%h›‘<úïßÔš–íôiôýÑÍ: Õå¥œ¯ ·~ÄF,Á?#\’e‘¹Ð>ç-Ê•º…Ó0]X]4qF5N†xºùÂ¤Ã‹¥?Ú™uiÍÛ´AhRLñ$Íõ‡÷CË®bHÌ“‡=8(:ÙÓ›RÑoÙ&#­	ýÉ÷BÒ¨ ñ£©ÙÌxc'žÂ‰d• P§»y0ÃÉ—ø/6ÂÉíý°ÝúAô(@=™’c$–LUþú£¦ÿT)5Ðü`B!ÇuÔÑór?¿„žÈã¢Ó(ª¥ûv Z¾R
Ö˜ºO{nßñ¡˜{Âô„%Ëá;Û‰á<`Ú'À®÷9\w¦ òo¾ª²Nð×C’Ñh¬òáÏõžqF¨{Ýëˆi°“>Ò±™Å!Ü‡ù§Õ•`ºø¸öÅ4Òg‡¥Ôíö·–»†ÊañûëŽ¿Ž‹B¸Q6IáÛÔïoÑŒþçÝ']Áîºƒùnv#-……zƒYZÖü+`üe,úËn¿»°îK[/¶NÍñ€u:0Ù°ivÉ„`ÝÓ7U"@_´3cüÛÛÝF%Q„ï¤Kæ”6NŸRhJ0ÜÖ
þÒ)Œ¸/ÆuF—×ë±b¯æól®ÔifJ(Hä fúA©MIã^j_á-!P}Í@Ýÿû0£!y(´íí¯6½‰Éî”%üBl«4¶…|rgÛ
˜— vT^.š=håÅµ­Ðÿ_ûÙBa8­z¤¸˜0„Æ—}£ê ¢aÙ«ÉlA •'tcë%~*SÎbŽ«ÇvïFPŒL+­¿ˆlÐ»Ï‘ÒåÝ€HÿAð4âcfÿÇ@ôdY„4êÌi£DØG"c
P Gë×›:öe×ü„å²S$ö:¿Æ(ºöfG8›Š‰ªjS¡++/+m„tÅ?´GÉ|ˆ1Ùñ+X@½T†þ	Ž,0wçrl·ms˜HcJÜH±ì|úâ„¯u¾x]Âð©J&f‘Ä;ÅSt¿ŠÒu’4ñüÑÊ6“<‚@„?Oá¹,ÐT!ä/îÅ×þwa„Bõ”žÌß¬Ù?éQòŽÞ’š‘;ÞKXê–ºœGÊO‹ä‹7ÎC'wrYO¥3ÿ'¹XµhBÌÁmF^©hÓ"á(œÖH?˜AšJÒ\U*²íÂ÷Ì­–ÝÒ”.Ò`_Ž£0+—rí‚çDt(¥	%‡¾üM:úÆ÷€	/ÿ<‡?t:GŸ¼J[]”,c±»’ S½ñäéŠŸFE´ëÉ§!þ’-kM½CÞ÷Üœ~púš‡"R‚Ó)lþ J¾§Ü_©îBqî›Éé7FF%9>'¢ÃKÙÌë^¿ÉKä5»Á¾F¦-à¸ŽzÇ>NaÀ„ôAæŽN‹[™“ç5IßW´Ž¥ÀXHlå9•¦oŽqè×½·º,kàî#yIQ´b“gÛËTjÅK^¬i¹!FbAE¥Ž„…ªFÖA’Bheœ+4!_õHLâ_L«ùˆžkHzv0n&læ¹5Y zýZL8©‰hû‘Oðh¦ÜÇ­ç×ý^ì|il/Ù0¤\„EÖÿhà.-õ„SãÎ7#sJ M/Ã°O¯Lžfe,ˆY¤/CLÙJX@]ôÐ^!	F\bDE®:=V 9»)JùrÝ'j„ØrßÉ¿D…~è4¡ŸäN¯ÞðE<ß”l8ÄøA“\¥ŽI†ã°»³Üu8r§|˜V&¬’C?«ÃRí­9rk«†1ÕBü¿d^O{:·‡†{>…6ûñ>ý
'dÒp V{}4¥fÍp‹…€9âôG\ø 4ôO×êoŒhÇØí$òtø&à[ÓýãÐŠÝ#QãY©@³‚‰Ý¦øÌ]yìø^ßf¨hXËüÈl5—,.§Û¦—TÖª\¢]§ïXt²Õ:%ì<¼á©.+„“¼wï\žv,uâÜjU1zÉ•µfÁÀÊ\[³“ˆ¼Â”ŠÝ_ÕGW+0çŸ]_›HÞ3î:Hi~úš>šèY”GVGoÐ‡,<™Üaˆ½(}¼)}ÓŒY·H$< "´)Q¥…,´ÿ<lº(¤r€œ6àÖÖ?ƒ¹ÉL¸¿1Y5ÔÉM…_^_Ç’ë¼ÿkwçI¨tÆ6#4VI /ÛºMš¸!ƒB>çänÚÓeŒðvyûiYÊ›¶òÒëÄÛ¶*…QÒÌ?úî.2¼åø€æÄÙÕNº«
ªj';E™õÉ$Dqþ“„a £ÝÂ¿”÷yà—/¥o®Qáõ­.’y-–AÄ‹ñ–ÃÉÐèªä1Dß…×CŽO²ÑÁR)—>ºÎ^_Ý”f1ûû*‰ûR&À‘¨¨bDCß:Lá¸¨(gÖ;MŽÝ`ònnÛã8âßª#‰Õb=^¾6ºFûá@R ?f-ì{9‚`A†1=ë<‰§ƒÉÁ¯¥R<ð<#A_Å¡ê‹vHÁ¡DëƒišÂJ-»`Òì'³Ü}šøiÃ³òúÓG9‰I±qÊ·IÉ9#Õâëé”ÿ-:‡ŽÄe>|â²¢TŠá¡ùqïaHš×bÇV¼ò
–¹<l’Â3Àâ¾Ì±]2R´2E>»|Éi‘ÊLæ
-©îñŸ&Œä\!`î[øa°ØHk|Ö‚£-G@!¬TÈ¼2OìÆò!ŽÊ6',¯N31Œ—	Vñ%777ï·¥ÈqvÕÝ‰™=ˆé+èl9æõjå
4±ë*ÙÑYóT³Cƒ™I‹$=B£
»ôÓ±GúŠìTÆ*‚¸õ4ßæ_¶À>LÉ1Ù‚zíÄÇc\.*ßg±T‚d\Tu›Á$¼ÿ9õÈm
ŽåJN_räb#ò›1©(ÊéÊMÖ
™MrGjrÏ¹ï¨T…ýÐMÂ1>ü“ô.p6VÏ'Š¦ˆ(ðq –ÑMy^ß«0jT`×:pâ2±·kØm,ÅíTJ‡ÎƒšÙ”a>(<)æLÌw°ú¦ÁÞ·®dÛú¢†Š¨wJUìUšÁNMÇ‹ø‰§‘1nN»`kb—§sÂæë,G'´ov;Ïb€~N«1<Î-ý†ûêæq€ ”	ŽÑw‚RFedEö²“hÏ©e*‹j¯*]˜Hìt,ô¥,‚eJ©}ÁCGêyÄ-‘Î„t¿&ÉÄn­”Ç­xDƒºEÚ‰ÉòV.æÛƒ€ÉØï=B©ÙÊJ<ŽÀ<!=- Õ”­²Ò ÇÍÓ#u*Ž{D@ Ó$™4°xd ß4±*\PÏ†¹—/ßò¿,×ÈF€bl Õà»rŒ&eÀÛ(M`0MX”·…ý`‹ÈïZduûGÉ¨à_9(=«éæ\Ç·Ëvã¢˜v”#!Á©(rWSªZŠþ&ß/‡RN"¥j¨!?¥MMÁ*='nëwL©Lr—®á
ž=Ê1}ÏÜeøeß8Î£
¬b»T‚²Tö¯x{ 	Y=›ÙÈ;% ŽEbXUˆàmqE›°ÃÁ9KËËÑ¬ ‰Ïúî#Æñ®aCGðŠÊ¼©×é”æàI¿1–ºtLôi¬‰HÉŒ¬YM†”±Tökœú%Öfô©ã¥JQ²‡_E\Ëw3¿'4±ç/aÉäô^!_™`vRüÏ>š´lP9æãdAý+µâÞNI8iPs -·ÇR›´Ë)“ê!üV?ÒZí÷µÑÕ®«»€PºV<OËKzV¹âs„C.ç48F–] P2©óZDšhÏ—t"÷ÊÕhÛodV?Ú’Ê$/äõðîÐ	;?IzðDÆfIÔÕ2ÊBna¢Ó=‰ÀN;ï‘ñ	]ZRÀîFþ”>Éêú©Û*%9B= é¥x> ËnžV¹Î•ó*÷û¤ß ¨P¾·ÃÉµ’¤»~[µ,&gö‘•É]àŠtÚXîY*«'‰P$ˆùÒæÜž?0CxZ}sY0)/éHê–t©Ø~yìë)þÈÇWö@Ÿ‰ç&´WýëÙ´Y¡áj9¨g±6w– „wþ{'¢ØÍCW¶ÌU%¿%—.ºõ"0ýôA½bS†;Ø ¹WæÜß4}ü4W]¨ÄYñ‘¾OIm[2›4Ü¤œA×·‚Pº‡°X×Ì»ÂÅd²Pn£*›W©ú²È§´öÏçŠ7¶ÇZC9q‰Ü&áƒ_b¦`QëùGd®Te¡C¶Ë³™ fD%²àç_ï³ÍKJ0šm;ÑžR|P‚$ôs&““©c}:ü]
¬¼2,c?k«·Ï˜^®ž]Ÿ*húsâì ÿ*“¡È(Û/5ôA6JÛÍþˆ‡íGô·2EIˆËN×ûBâo<í(i10-ì=Ñ;ÆU*¢QºÕ*Â©I!¬91JÆ_)³Æ*?€™Å^0ÅR@E‡Þ-‰Éi§±&ŽŒ>,ìuzÇ?PZp0nv ›ûhù\|¾Ø‘C`É?EJ•r&“ƒ Sß“Ùå‘ÅŸï™|¶õj ‹ÃD¿ýšÈÓÕ’èr@6|Å/…|+`#À}aßÑæ3	ánR€NÝSEþç¥è¦…sÇÉ›g49¼ÌÅ×%ˆùöÁ©æ8C=ÐR:{hê· |ú¦3™¿¶b?¡©¼Äý“L¹˜Žæñ—ô4€æ¿3úXiÀ€Æ:>=¸¢|çò=Ù^}¶cËÛûúÑ›T‚ïX4aUeÎýAµYÇ+ò£Y¦*ÅQ9`H+¬¤”_¯ž _‡L‰Äf””l¤ÿDªV’Î±Ä_
ƒûö8Ìäö¯µØ¼\rj£MòÙkŒƒ¢©$Åd¸w›ïF„Qähé§1”…†Ê  cOÁMÎ‹“šn»etU’Hbõpdï¦{öÿLB\½þ ÍiËr|8a­Ž½ÔF¥‹«¢ý)ÏÃ¼©1Ißæ}p†.ë00PÓÔÕŸæºª5EœÙ"­‚š=Ý¤4<Î,ñt±=“œ Æü>“¼®ñå,ž¸X´ÿ%„vÿE<€»®õQp×*Ó/ªv÷3Æg›ÐV¶þlÍU¹Ûe´5;ßê¼ÅFÔük²¡TÑÇ©ÄÞ=æ‡‘Gbî?èù9ÜÁG°¦fçé,6ŽæØgVözõdñiªK?Ø`¡j¿ð0Õ	L£• ëþC†jiÔa±Þfq
.ˆë:ÀÆ.øt$M’°Qm9ÌœOÆxÂG—!›8éü“óšáëWÂJ…?T= RgÞyº×.fPƒ¢ Ü!g‹Ëßìïâ0:›»eîëûJDn]`Ó®$Ôü¿ùˆB8gp‰Í%åøŒ×U‡†ËÅ——·;ËÙ
/†°øfÉ MsÃ˜>Jq—çküÄ÷ÃÈCË™˜^äf8 ñþ˜é®°1bb~¹`7ÿ€vy¼‰ªµºç!Âå…ïŸ‘>žm%Åñ+8Ä&óó;¾l+˜ŽÅ¥|Áôûƒÿˆ$Íàe¥ÁŒ¬<üéªk»oŠ3ñ†¹´Ý
ì*ã0ýó€XWÊl ƒ½kÊÓØ--J°Ö¨h/­êTgk Îlë1,mÃ"šs×ÊG.6ª iWdV=çlúèõªù ,,‰¿’ZËé‰©p¾šÐ<öwÈs,®U¬OŸR»·¿‚ X·!p‘zÜyý°5'_ºiB"Š²Ia;A{YZ_+Ò5ôè†
ÇD×&yÆQ2ýò9v'émÍÓ¸Tþþ–^îø*¦ýVbP@g<ÕÃ~3·Fhkd9·­Kš‘²¥èë. BT}‘¤“1º¬Îø²4†œcý§¢„&Á°À}Àn¶;Çí9ý?q!læ†–7þ°…Á±‰o[CÁïWtôDÅF¥RGÞO‘ØíÍÂ#=°ÞÁ—ûJ"sêÍw°Eh.Èî©á1|€Ù[°	Í‘€á–iÙ,¼èmÄ(A2t>_ÂèbQÖÍ¤Îe$òÐ–_r›ñ´*ˆŒÄ˜|¥žv#¡Àdùæö…{š˜Gø\j°£áÏ‚QÊpCu®{°=¿‚™¿Û†ù¶ƒ+G;b†ªÉ'Öî<É¯ŸFÛF`¥‰lª÷ì‰ëŠ9ÕQ¢‡'Í#&+aŠS”§©|®ŸT4ø’'ÓXz0Í7²]d]&I.–5Â7z¥pgõþÌ<‡›e‚ *ŠÔF9WÞÈ Œs‚‚ŸitÒ3å°Atµ¼à-©Ó’°Ü2ZSý®LG\‘°¥Ä>¢î³nŽŸw¦9ÃÅk?Î«ÎS¼ãoùÜ¶¾A-åz#^P~Ti×“	 nfÂ@Ó®tC©¥³ÿ…ø¬ì\ìZJjk{Š„Ã“ï«v²ùNíèYn ædì”mTbù#îyf&ø'"?Ë¢œ¥Ý<PÛø{;ðqÏø›Ñ	_™Ä¥ã^‘•[Ízí"3O´”V1*yj1Ùø†ó{å Ë‹òÙÛ6øØ!ÙÍ8;hô2üêŠÇß}gbp$,òSÉü,V‹˜õÑˆîÙ³}c~ÿñÿþj—™;­äQ3GOÉPC‰ŸìYëEºÁÂ›ES]d´f·‡ŠéµŸ¡‹î6˜sd¤#ár;m/¼HPAVµñØÏ&»RNÿá›ýIe‡é™'°5²Ú›÷AZ±Lø"R•þ‚Â¡™ÁÙ·Aí
?´Œ×Nÿ{g=Â~—uŠ¾Ûüé»´Æ¬wÖ[‘=(31JS
ññJÖíÐj ØéÈ~fC¿&ÝhèÐ«‰ªkû^ó=ò_Í®Ø6½£‚±JË)•Î‹\Ní;¾¾:e.²Âª7wö«*÷%Z
Çž8&ç·Qs4¨âß\ŽÚÞ†A’Ç…kA·‰„˜@}°¿Šj±÷h§Á;Ÿô_ò=T"-#„§!˜±ÈÝ¢Õç^{)öÀ@çûÿ¢=¨^žðË5ƒ·ñ	¼ý®úÚ(IÌsq„3e ôÙnt¸ q”ß¯-Bä¼|XÝ4
Åk.¥æ¦ x—@M¨´.~^ülÏ™JhÌ$;¨ÔåNÁ²~ï5ã‹y-ŠBÌÇtüÉ+¸ºÛÞ¸ùcÂ‰<<o+i	¦&ô·hx@87Äë¬Fy÷¤´CqbÀ-žžø¿T"|Yq²…Y®%Î²æ›,*Mm§Gœ°õ*ˆœUVœoyµ©Gl®VÕÞ¿Í¢þÀ`¦ÅÌ>Ö›Wá[Óéê‰tZ7nl`ä¦÷4SOD»­ð¾é-¿€<Óy¯„ûOi‡SÈOço7¬V6üxAm:ä<î¯©	ø2ëìÔØ«	L„pñ"3 EŽŠ©[ žüA‚Çöóèü*yôðœøP$å·
ì£VÑ§6†hˆäŠèƒŠ™aé)`pÂYKŠä#6Ã_WóL|‘˜H\DÁR˜àÉ†åeÃ–»«¼c;”}ˆñÓ.þ8T™[VÙÈÁ×Ù3«òF@¬‹Â=Õ*òÛ´µø>Õ‘Æof1¬ÁU{ð~Ù,…7¦a³ÿT˜aÙZŒKãÊAvSïh}ŒÏW6Q³°æòÓ¨ÜŸ£×Å<îßDXäÜ•6 ‘gÑ°4‘%6¶§‚á¼9««ªÛ•WŸ pªŒ!µ_–nj&my×˜Æ·‡Žl<€óm vŽæ
Ût»1(ÛØ7þyÉÌ(Ö/«‚Ð&ª¨k‚PÌÙuE=Aä™UèU{ºEÑÉŠà;_£ MÕÐ©hŽ­©VãÙ‚æÅ##,Z¤ôÄ•eÅÁ´À$¹5£ôù¦™W4yéßó	}Ì÷g™4Rjd-Qr=ý€Òç9¬Rms;ðÎ;ž"¿Lô˜Ò<ÚBÐÄúÜP‘ò¬ÃãZä[ØCåR#Z÷4hŠwÍÞ¢Èwé®óbÿ‡öïM±opWbÿ~"R ÂÚUüòžK%àQ—óAôÅXÜÆÅ¥½Cwo[A¦^ö«ÍkÕcè:vªŽ1Ã³aVMC»ìB%oûŠ²6¾'ˆ“ªb)¬;ýÙïj£Ÿ³s=Àø&®½´1‚§µ»W_:=ç? r%øs?1“Ø°x™žIÉBáZpä,6k“Ü£g™¼ñFÅQ²ýLq|"úë(*NÛ ß´‘ån‘	r«zaãOe‡@¯é^>„ð~®TÇ{*GÞXª”£»ø”\`h¹H¢2êAzxŽ…Ä^L÷ãbîû±ŠÂRNö¿K ‰ÄU©é›2Ð½½¨*ÓLµÎ(ÉpÚ2 Ø¹î	5ÐF¹’Z¨UÍ_z†WÎ+ÃÐ)ågpî¡2æ–&7VJ¬¡Tûß@ñ‰ºM™ðe‘5Û9”]û¦Å@„	î› BVÌU(ô5`×¨ÄSB})‚4‚ÁN E3½j.Ø,Ý[õ5Ø|-–/™‹EÜÈä÷VÌëšŒŸïX8Ëi !bóK­wžïÞõ(¼<ÊAIÄ«¹ì=2¿ä(ã¶þÆ7ÔH-~bCÕ’~QþŒvÉ"»toRkÒ–û**}Ql*¤†Ö‡´H0
Våßx«6©> ð‚H…ÁÚ÷#;"‡xW~ICsç—,]([â£¬Þ´«?~ÄKïHrÌÚ´©òª‰þªô]¨´?³Þ9`uyéD*U£lò	û²“Îu´Í!Öóè°¼ ¨ /ï"Ù
Ôõ×âTŒÍúÂˆYtÉ-KøË7¤ íÅUà#6²ÕeÐÍ?Ü”àÛÀ©h‰7ä–KaÌÐ¨vEáe“Xÿ‰më¼ÏìÒæ1k*ã11Uå“P—CzÚÊôv<x¬O±¬*Ã¡ÑÃì&ö[é¶^RˆÍ¼,óÕA ÏçªH(5R¤Ì¬Šp/¸”½?‰©™§ªS9#JÄh@Kopêö1ÉZÏË¿tÔVAug^.síE1ƒªø+ù¶!Ÿ®q,½£‚¯#¡³?|\]VW¼Yƒ»x? .ˆZŽ>ðþÔ“0sHPÄŒa!$yÞJÆE íÂnIb#a†t¼—¨ÄÂÀ¾¨SA]‘¤*…§d«?hø(µ\I÷E?	19B×2\´®'s~`Ž¢wÞCõ	ÛýÄÈÛRœgküsŸÏ¹§kÐY¦¿Ž´:<¹g.Ž;PŒÚ¾MÃ*°P|Ä4`@<Ç#ßtºGgÓc„	ÒçQÜBÁÛ;!Õb’²î™ë‰Þ¼ZÝý Zp#1“Áyˆ~_m–ÙjL¶Ä7ûnÿŸ$_®Jð;e›t­êõ®BµXnÉ:\è÷v~ ¥{1™×ÅAžñóÞ†DxD¶Ÿš:¡P÷KXCmïûTJÝ”±HÝ}0 q¥eäçïlÎ8T2ËšÏuÌ#$¸Ù2wÊ<ïBÕ—çC¿L$*©?â¾,vøº'H@‹OVªTÃ§móÅ0«W^6PDÂï•j¾’tŒïcö|…Þ$Ô³ÌûŒŸe¹lë¼TƒÂjèÃíÖ¹rŠVà¿9œ\$û=„¹±•m-f²x»,š^´ööŽ$©´å~¦’“O‹£ÐaÁ—„Ñçû‰rï‘ñúP´¨]cø6ïœ¯ _ÆÀÀ‹ÏM‘ý°…qbÃ^F&¦ÖLwæp†®óíà%ÝÜ~P/}Œ´uô_Œ‡Ôþ",î…–2^«ë¥ÕCr‘¯¡ÆËÎêR4À¼Ò|dÝMð1}ý¼±‹B‘Œ·þÑaŽI…ý1]9*9¶òîÈî'aø­ö™¦?("Ówð}Ÿj{³£àhçk!>j””™}Ï?Œ(Ï}=·ä@‚RÒü8V¢§¥ˆ.+—³kô%n{âŠŽûö@cÉ¾B¦4èI¸o\á¾{13°Ã—£ó¢§Oç°´0¢>aÇîU+ï[ëÑª¬(:"„bÒZá)Æ–DE^g‰å¿&Çú¶– €ýçó¸ò6m"´üÂñxi+*jÎ“ù’ÇQ>ž>¢jé6½™DY© CE[cèÖíˆ¦»oš¬ô6ªÐ+lx£¢YNˆÛä”ýÈ¤?¬õU Èz²Õkz²Ý¯
Üö'º4±á>ŸUšRƒ1û®Ô8¡iÃ¡¦Çájñ;"Íï1dÈÛ’RxÍ%
˜!o¡¯Ü”dbøJ€		ó‚‘5N“å´B¦=ƒf½½Ø„´[$-Œ1™#²uæÁüBå­mÙk[M‡›,[¼&è ž|/´cRnz‚êâ£7®lwÞ¡!·š9oøFð¡ø:²,ËvÉV°¦ÒÊÐb·6Xa[æJ:.0æ¿Cem';eœÐµnl‘¹MìÄ=ë$¸8sê®&’‚5S9j[º¥&"è§K¿ö<‡)á6yÆôþÙlu4O¯:ÚÚ	ASL=dås¦’4’x¡ã1¨§ä°cd/ýâýYitC ~îE’ÄÍœ­f÷!ÓÃþß¬¸YØüÆ³aNÅÃè±˜Œƒcº½(nŒ9¼ãm:1 ’¹õö¨òQŸx/îcnm_:ô‘ìIêÛÜë×~L‚ÿ‹hä÷ë½þøbÃîŽ”’IÒ½b,$ïÀCOžZcO²Âf/R#+BïNª"ØAûŒ¦ï”3³\Fr¢~êÖ¥ÄÉ¿¶….Ï®_ø1ùûë–T?qÏa#$WS¡{ËÄHÕ0‚<S ê†¾.kA`X4°£Å*Eß‡gîpÇ¸–À|tû'Úøæ z¸DŸò£¸8ûÓ9¬s;Ö/2tºT¤þT›¾ÓÎÉ×jt[Nôµ=áëT@%XÃ‚A2Ì¥çï×A×~•F&»©XDP£FG	(.>¼žP”6sx•(Óˆ»±ÓÔ‚½ÕM1¸¡•ò@ˆ~Ô—vë®O*Ä¬<ñõÎ¥>â^ ÕÌêÝ'¦óþ…1jÔË'¤œß¦º¤yµÞOàç‘õé&Ü)ï+]güÚìÀŠOL~z`¿T²6Å]¥åÐóçÕyÆOŒõE/¸¯ÖKë¶a¨ŠÊü?ÜÆ!Kû?Ú™$›r6ÁfO'x´ÏJÛØûêì>›|ÍÌ)¨ÀŸgiÃ(ÇúÝÐ@Yý÷a6Š–ú½wATy£ðÁáaúfóRE]Ý÷’WWJ«Ak® nîð€	?ð3|†,ÁU30MÙ	“ ê1ó~†6üÛ*6‡ÅL}i«Êæ2K~Ãâü2(>­'¸7ÃÖU>M&í# ¾“ ,†·àvÊšv‹ã|>	
[yµù[Ç)Ì$(©îJ.±çû‘«=wÓ'çûj…²:èéÑ{P¸íF<S(wKy©Çž·ó‰Ùÿ†ÿðIgáy7
³<ëJÛo³!ÿ\1ïY0‹ì}O=qEnZßg­âhm‚Šþ¿ò¿°$g*©øa†o>é¶4àÀÂ0yÑ5¬ÝÞ{Ë
Gšh¬=œ ˆã¡¸dTe j
…<ò!J,§·rjiêX8<:<_úw”Ê•
ûB;Ì‰ñÅ/ÙÿéZ|Ï>np†=9Ûöc¸L‡E{ö7õnMÙ{Ð¥òE:×æ‰fS_LY¢ö6ô¸¼üUlÅg©n4¯ž¯,ëéôñ(Nà¦”
¼œæå§÷ù˜çÌ¿ØÓ»ûÆ€5€2š‘q¢¼Óiyj©B~WR*ñ?Ù%wt'fCÛ­Gdo¤03z:õ1M…“[7÷r4Ä‘@ï™6Ü~„‡œƒ¿×‹;
~	k³oó‹ÈŒ<È“Élex ¿pc6q.Ú#faà¾»“ë7Úxj.£:¥8±RßÚ†­µÜ-–® Ò½ä òWsà<HµLÓ±Äý2ˆ²ÝíT:¯ÿP€½û‰ÉjÜ“:æðD°hö‘MÕ¿¸¡S*öhQ2ðÜƒC¼(ÿ¬7o—:\ýW(Aû¾¾É\íRÌ}B‚§Õô¹XR]¡™i2xàXþ=ß"˜VYx|šyžsÎE§Xù‹žÐ?´û}xæk2¾î’ý¬À_a%Ÿ°Õ×8\øJK…(:æ¿‹ëàt{åUô*““ê·H“È,
j¡”\—cOŒ¯[Ìxwg2ô öcÓîÔ\QŽÔü#î<O¸çH¦½n0Õ´m™£´?Ò[/ßÕ1¬“¾fyÑ(pÁò:Û+>€Æ 3mµqÜ€"¤‡Q´htË„ÖÖê[8ÄùÕUs†hN#aäý»ØÝ„Ã›ê–&k‡àæWgr>c¹èªc.®>`vÙÂkÚCºµÓFƒß¼X#Ã]UÚ3–Æ‹ÑJj‡Lö«Á–-D9/rÿ$h>ê:¡Ð‹ï2fŒN÷üB7ÃüCÏj€Îzâ`9[x!ä16zOgí‡ËL-â¼Í¨«ÿ;z‰€\ðžÿžé££Æé…|®[fè¦Þv[ÔîkjÞ6ÎÞ9ª¾ÑpW¼²
Gä*Öa`5®I½ù¶rÆ4àî«°ï®üÍHjxÕW°¤VÕÆëÔ7çÛ—c½¢÷÷qVÌr‚Ï™i0Fs€•\Pª|`ÓX4±K–õríån·ŸÂÔ·ê´!tø­ÀìÌ•vtdÜíúe»ŠÉÃºB6Þ}jè¢œq¨gµ3»à"£•$¬wÓŽ+;€àíƒOVx2ˆ¨jõŒÍwuc÷ä¹¿ÍgM*¬šFW^÷ýs?G;ž
_Ò£OŠºê§©]±Ò¬à"ãÃ:‰W8ºªˆìƒë×4ážx§×—PH¤4ŸGâ]¯§m,*É|iK³ŒŠþbñd¦Eß˜7Ôë§X#¾Î¡åßF¶ß«íåàzÌXÌÔÇo&c0¸îÉé3%ÌB(Õ‰X	¿ŽÛÝ¡Ì·^mŒ·ƒBý¸¸ ¹/úÎ*NÞA; ç…C°EYZ¸†k“Üu.µ+~Í¼	ªQv«ƒ=™êšàŠú¯Y’Ë‰ØTÜJƒg×7¦Q'
y]iÈ=™,ž!–o2EE 5Áï Zu+¦²DžßSŠÛP
	Ð’Šc§4ÌÑª£¡}åONÆ)ÑÍJû7¬wà¾ç¢ó¸¾|'™Ñ^ü&\ôen›À³ †ñ‰e‡SGN­¶èäT%®ÓÙ&ŠgÑ¯¶g•E­EúY}¾Kñ„&€MÐ‚û%²[ràpÓžêôl½#L"#*8uøÈ.<ÌÞÜO¯&¡¦™Æ‘åTVì‡¤²7ÚŽe{srB‰Nêâ8xfê½Ò(ÇAÁOŽNVŸÕó­pIJºD€réñì¿G0ñJ¢½BP¯ÐrÃO<Éìù0Œb<ªãüs2ÂàGçëˆWEe·„¸'Fð“‡{iÊ{š:í˜ïL¼BõébqL]FXqR‹@9Ï‘`ñƒƒ¥Ðqˆz¤=<ÁÂ„‚A¼“RøM|–‘qþN¾Mïþƒ°ºìW#~Î-¥lM‡j–ZÂ‚9ž›d„SzÕU¬¬ªÐ(°¥C£µ%G‰ìd¬tÁ\^æÃû¯øÝ³aµî·l›çÐ-Nª°,‘‹÷º‚çu¢à‡ÚPœi¢þ™&,6aˆ?Q×0ÛÆÅˆèÇ`c
æMoQÉ8}	þ’9À
u§(^òÕ0Ý­x’G(@žóã{€®Ó÷Žäx¡Ç°»x(Ö1Ùn½s:*UwœšcñB%f×ÑTR8™Mß@i}H]-†h×óÓ<(«>ûÓrÆþvBQXÉ€õÌIµ‘‘úÁÄÿv®uÏçJŽ4lHðo³U?ôVx‚í–2XîAº®BiK4PÆƒ²œOÍŒºÐ†{¹¯ø£Fk×yð»cœãZp¶¦Àˆ–©_2ÁcÍ}oÚm1ÐÁÜêpç>Ñï÷™ùØqÙ«ï	h¸Ðg—// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885 PCIe bridge
 *
 *  Copyright (c) 2007 Steven Toth <stoth@linuxtv.org>
 */

#include "cx23885.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

static unsigned int vbibufs = 4;
module_param(vbibufs, int, 0644);
MODULE_PARM_DESC(vbibufs, "number of vbi buffers, range 2-32");

static unsigned int vbi_debug;
module_param(vbi_debug, int, 0644);
MODULE_PARM_DESC(vbi_debug, "enable debug messages [vbi]");

#define dprintk(level, fmt, arg...)\
	do { if (vbi_debug >= level)\
		printk(KERN_DEBUG pr_fmt("%s: vbi:" fmt), \
			__func__, ##arg); \
	} while (0)

/* ------------------------------------------------------------------ */

#define VBI_LINE_LENGTH 1440
#define VBI_NTSC_LINE_COUNT 12
#define VBI_PAL_LINE_COUNT 18


int cx23885_vbi_fmt(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.vbi.sampling_rate = 27000000;
	f->fmt.vbi.samples_per_line = VBI_LINE_LENGTH;
	f->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
	f->fmt.vbi.offset = 0;
	f->fmt.vbi.flags = 0;
	if (dev->tvnorm & V4L2_STD_525_60) {
		/* ntsc */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_525_F1_START + 9;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_525_F2_START + 9;
		f->fmt.vbi.count[0] = VBI_NTSC_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_NTSC_LINE_COUNT;
	} else if (dev->tvnorm & V4L2_STD_625_50) {
		/* pal */
		f->fmt.vbi.start[0] = V4L2_VBI_ITU_625_F1_START + 5;
		f->fmt.vbi.start[1] = V4L2_VBI_ITU_625_F2_START + 5;
		f->fmt.vbi.count[0] = VBI_PAL_LINE_COUNT;
		f->fmt.vbi.count[1] = VBI_PAL_LINE_COUNT;
	}

	return 0;
}

/* We're given the Video Interrupt status register.
 * The cx23885_video_irq() func has already validated
 * the potential error bits, we just need to
 * deal with vbi payload and return indication if
 * we actually processed any payload.
 */
int cx23885_vbi_irq(struct cx23885_dev *dev, u32 status)
{
	u32 count;
	int handled = 0;

	if (status & VID_BC_MSK_VBI_RISCI1) {
		dprintk(1, "%s() VID_BC_MSK_VBI_RISCI1\n", __func__);
		spin_lock(&dev->slock);
		count = cx_read(VBI_A_GPCNT);
		cx23885_video_wakeup(dev, &dev->vbiq, count);
		spin_unlock(&dev->slock);
		handled++;
	}

	return handled;
}

static int cx23885_start_vbi_dma(struct cx23885_dev    *dev,
			 struct cx23885_dmaqueue *q,
			 struct cx23885_buffer   *buf)
{
	dprintk(1, "%s()\n", __func__);

	/* setup fifo + format */
	cx23885_sram_channel_setup(dev, &dev->sram_channels[SRAM_CH02],
				VBI_LINE_LENGTH, buf->risc.dma);

	/* reset counter */
	cx_write(VID_A_VBI_CTRL, 3);
	cx_write(VBI_A_GPCNT_CTL, 3);
	q->count = 0;

	/* enable irq */
	cx23885_irq_add_enable(dev, 0x01);
	cx_set(VID_A_INT_MSK, 0x000022);

	/* start dma */
	cx_set(DEV_CNTRL2, (1<<5));
	cx_set(VID_A_DMA_CTL, 0x22); /* FIFO and RISC enable */

	return 0;
}

/* ------------------------------------------------------------------ */

static int queue_setup(struct vb2_queue *q,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct cx23885_dev *dev = q->drv_priv;
	unsign
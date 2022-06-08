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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            �7���Q~U5(_P% ���d��9�|#���E�\����Eդ+,��ՌPgD	�z���v�e�h�r .{0��a��K�[�a����9պ�(��n�4��?Z�K3���������>����i{,��� H�����@�^lZK�ߡ��4�O�<�k�5�-�)���D��7���w�k�Od?'!�G�t�w.XIcja^��W��j����y�?ce^5�\�����t8:���)wU����KxG�Ip�8Y����'_=�Zn�� �ȣ���ۚ�^3�h���uУ���zT4TJ^�^dW%Z[K���4��o�h~"<�ˌ_� ��ɰ4A�0\���I5Њ��j�pQ��]�7���E[d09h�"Ϳ��W+��� H��)�qԛ󮽐�S�uOb��ck�4���d�ܗ�H����b}�Mc�������k��pp�d�4��>|��,�n�}�-����'6����`�J�33Oo?L�����"�D�y=�Cu�}Ԙ��O7o�죺��PxAs]�H8����PPh��8k��;�EpYot�kC�kM�"�ܭ_֟�j��J��e�M���,�GM%Wυ�pO&5�K�=!��4ĀG��ud���Ɲȷ��CN;��4;��r@���捫��t�`y8�>@�Q��4O��B4Hz����<^��郧wXCf����~�d3�td����dV�0��b�b�*���W��Ε�es�f\���^Q�Ò�MYRr�p��+��qs��"`�9N�@��fx��ѿ��]���q���� Կ���LǴ[�̽g��IM%�{|�]�*W6 F�ބL;�"�7JN�����߸���/< >�O���w��3��|�Y=x%0�@� R�f6=��O�&wW��pF��)�Yh�kq�+#�Y��T"'�/�Çu��2z���� 	P���f�|?P��L;S{�3�/�wzZr�浣�	�x�$1���Z���
�� �vR��b(	Ŗ%r��%�ӓ����xi7n�Is��'�T�y���m`TF���ە6Gۤ�:�^��^�&�\�k,��L��
���Z&��������9�JM��mDo����T�޻��&]�rJ D�s�1�Q�=�CRԜ1\�}R���Z);k/jbB����������/4�����A4;�$��{�e�����[�� P6r��/=�A7k��!�*�DSF#TM^_�m���zP��,G����R'�S��oM�gۑ�T6P�b�߆,Y?b����UA+�Y�0P?�P�[w�(DDJ�4l���o�3�j1�2��Ь�\ ��"�E�{g w�eu�~D��E�l�ǚA�Q)�5�$��4Fn�Bgw'	bK�g���g��t��VV�n�J�ޗɑ%=0����/&�Ƭ�[`8� L&b�]�%���@qiIjx����vV �����<۲��1D+�g����v�5�M/�u���?�P�T������$�WP��F�]���(h�RL�%�Kh��ݖ��Ď]Cǔ7 S�6�=�L���Zr(L�XN{��%)���B\�!����s��'^�G_�b�����]���Q���ۋOG���Ek9��ْV_���)�� �DT�����&!i�(����YѾ�v���7́_�gC =%�I�����O</������s����g��I����\U�VM��46fB��<)u"+������X��}v~~]�)��HS��<ۆ#'����z������nŌH|5v2p��u���_�D&�'�A�r�N��eΥ���k�ܿ0�t�E�?::��咳{�	�NE�����(�ju�FC`ө���kw��p\�x�P�y�$�0O-�I���3�{0b����'��wh�˖��������sWа]� *��&ń��u�Sw�8�(�)���}X�ym\����
��c�/0#�i�
�Q��s��ͺ�N�;�,U%@r�s��\����;C��C'ɠ��(�����/������b5�7�X<3}�u%~�!Gd�(Ǟi�ȧZvg���_�\��:m}> 2���2Wj^���4|�Z�������a�~(k���6.!�����@�y��(Ť����/�]̡Hx���`�DI�i�ygT����w�{��#��D��`�zt�ת��|xq��z�QB>�����y~t�����Z
L�
$�xK���YKe��T�0����X�O]]`q}� ;;�����Y[Q���B�_<����Ϣ���G�y�j�f����57g�:o���ǩ8;R�E\_�v�����󿷎�޷$J�~V)�`������6]MEE�m���7�v�ڋȒ��*[$�8$�@���R~co�X�s'E~�0�]b�[D]��o�+���6*X���a�������.�9��ٽ�o�z�1.��B����e�=�����d����(48�=BաڜJS�.� nG$�i�K	���AF��J�z%%�!D�h��៽dI=��1�/s]3��wtA�K;�dDx�t���t-����p����JH�9yy�psD��n�7�K�z!�y[p�Ҿ�ɩȞN�Xp��c-�������&9�����؟�����(-���k��*��}��6�$�5Y��}6��0���P�Lu �w����~���{�\��	o�N[d~Lj�����"A���|^���qڹh T*�]9�SW>j��@�p0�Ѯ�r���%�r��1�%ѿ�p�J]M�C+��5��Wu��8a�`���/�����S݉> �f��"�U6�ۉ:���"��Cz5M~/ט��D�JC����V�T��?f��n�(}�bW%�bY�ii*5I�IJ_�IZ�DB����t���	�J��EXm�A�u�/ۖ�c�������H���&��-)+tn����^k�E2���A�PNT ��>V��OyT���}9&GY�U\�� �mz�j�]�6��V�v�+c��%��j2=j�|n��0pb`�t[}�+G��jd��Abq�}�F)���E1[���k�+��*ͯ�d F�H�..���vw_Ue�s�@�| V|�9��Z�!3W�J1jf`��_"^g�Ϧ������۽��u��X�Py����6��� �6pp�eu�}͈-�M�ٹ��e�V,�
���p�&~n��;N�.�sʞ<r�$7����{��%l��C���tQ0H%tiB]�m��U�r�i�)�Vl��H�R"�k���� �H�_�
�\�h_�����t�"t�=;+��(���O,�	������1���@JC��I�ּ<���tMy&�EPy�*���~!��1��i�R �[յ$n�H$�$Hۮ��O�s�+�)�h��|ۡ�l�-��PJ ��Wx^�0i�#Ø3�B���!f������잢��8(O�l���h)gQ���ȱSe0bdV����^P����*��Ɇ���Лb��@��R�KE ��`5P�;�N|(�S��Ή,����B63����Cf1N[�3kI!.�.�X[g�9�M��+� ����E��	��}�g�y�羹�>�XJ%84�d�����<�d�w���}F���1�t�EB�4ڱ�����]Z�*?���'�#��,��`2r���~��3�yz%�lp��&[K`���(���� �:TH��0Y�T>.�x�G�
!ViȃJd�sդ&A˩�7��HxI^5��������d1Z����!��*U���9�[#Z�/�FZ@�K��C�X�|��a6D�|�}Ko����i��2v�Q��\d��m����Vu��B�r�(�g�����q�w���O0����d�)Yסχ]��3z*��>r�p�k��5�E���S�H9H�`%�Q��V��(Ո+0�sQ���Y�r���7V��_O��c��ɣ^�#�;�1r�_V����6�m	jA����O������^�@K��|2�����7Vq��0�;��9��8P]K����b
Io/�9�T�LZ�3����9yV*���ЪT�'	�9l4�B^Qhg&�Q��al5T7�/b��P�(#�+Ԟ�����2�Ң����F��f��e�	�J�:q�z��v]�3�����S�p��93T��?�~y���m�"��@�z����P��B7��U�4{����ޝ%~���r-I�f�%6�
�/��U-����FT�O~$�� ��$�" h�����Vg67����ؐ������j��_GG��7E<T�o2F�_� �m�c̽�-�T��z%
�+�"���AH ��� �5�,q�R��b�Q��r�Cr�M<���p�!�e�|�+�8���meW^�݂�Ӆ8�Fa�Q�~�(c
����ۉ�-d���{c�Bgc�}�Oxs��a��Vu�����k���� e�<tZ2����g���p�(X��uL�ٯ�K*%"-�:y.3wB��/ص�=Đ�u�[%�
attach()
   3DVB: Unable to find symbol drxk_attach()
 3DVB: Unable to find symbol mt2063_attach()
   3DVB: Unable to find symbol stv090x_attach()
  3DVB: Unable to find symbol stb6100_attach()
  3DVB: Unable to find symbol m88ds3103_attach()
    6cx23885: %s(): board=%d port=%d
  6cx23885: %s(): QUADHD_DVB analog setup
   3DVB: Unable to find symbol lgdt3306a_attach()
    6cx23885: %s(): QUADHD_ATSC analog setup
  6cx23885: %s: The frontend of your DVB/ATSC card  isn't supported yet
 3cx23885: %s: frontend initialization failed
  6cx23885: %s() allocating %d frontend(s)
  3cx23885: %s() failed to alloc
    6cx23885: %s: cx23885 based dvb card
  3cx23885: %s() dvb_register failed err = %d
   7cx23885: %s: 417:VIDIOC_TRY_FMT: w: %d, h: %d
    7cx23885: %s: 417:VIDIOC_G_FMT: w: %d, h: %d
  7cx23885: %s: 417:VIDIOC_G_TUNER: tuner type %d
   7cx23885: %s: 417:VIDIOC_S_FMT: w: %d, h: %d, f: %d
   7cx23885: %s: 417:%s: command(0x%X) = %s
  3cx23885: Firmware and/or mailbox pointer not initialized or corrupted, signature = 0x%x, cmd = %s
    3cx23885: ERROR: Mailbox appears to be in use (%x), cmd = %s
  7cx23885: %s: 417:API Input %d = %d
   3cx23885: ERROR: API Mailbox timeout
  7cx23885: %s: 417:API Output %d = %d
  7cx23885: %s: 417:API result = %d
 7cx23885: %s: 417:%s() cmds = 0x%08x
  7cx23885: %s: 417:%s() PING OK
    3cx23885: %s: Error with mc417_register_write
 3cx23885: ERROR: Hotplug firmware request failed (%s).
    3cx23885: Please fix your hotplug setup, the board will not work without firmware loaded!
 3cx23885: ERROR: Firmware size mismatch (have %zu, expected %d)
   3cx23885: ERROR: Firmware magic mismatch, wrong file?
 7cx23885: %s: 417:Loading firmware ...
    3cx23885: ERROR: Loading firmware failed!
 7cx23885: %s: 417:Verifying firmware ...
  3cx23885: ERROR: Reading firmware failed!
 3cx23885: ERROR: Firmware load failed (checksum mismatch).
    7cx23885: %s: 417:Firmware upload successful.
 3cx23885: %s() f/w load failed
    7cx23885: %s: 417:Mailbox signature found at 0x%x
 3cx23885: Mailbox signature values not found!
 3cx23885: %s() mailbox < 0, error
 3cx23885: ERROR: cx23417 firmware ping failed!
    3cx23885: ERROR: cx23417 firmware get encoder :version failed!
    7cx23885: %s: 417:cx23417 firmware version is 0x%08x
  7cx23885: %s: 417:%s() status = %d, seq = %d
  6cx23885: %s: can't register mpeg device
  6cx23885: %s: registered device %s [mpeg]
 7%s: IRQ Status:  %s %s %s %s %s %s
   7%s: IRQ Enables: %s %s %s %s
 3%s: IR receiver software FIFO overrun
    3%s: IR receiver hardware FIFO overrun
    7%s: rx read: %10u ns  %s  %s
 6%s: 	Enabled:                           %s
   6%s: 	Demodulation from a carrier:       %s
   6%s: 	FIFO data on pulse timer overflow: %s
   6%s: 	FIFO interrupt watermark:          %s
   6%s: 	Loopback mode:                     %s
   6%s: 	Expected carrier (16 clocks):      %u Hz
    6%s: 	Next carrier edge window:	    16 clocks -%1d/+%1d, %u to %u Hz
  6%s: 	Max measurable pulse width:        %u us, %llu ns
   6%s: 	Pulse width timer timed-out:       %s
   6%s: 	Pulse width timer time-out intr:   %s
   6%s: 	FIFO overrun:                      %s
   6%s: 	FIFO overrun interrupt:            %s
   6%s: 	Busy:                              %s
   6%s: 	FIFO service requested:            %s
   6%s: 	FIFO service request interrupt:    %s
   6%s: 	Modulation onto a carrier:         %s
   6%s: 	FIFO:                              %s
   6%s: 	Output pin level inversion         %s
   6%s: 	Carrier polarity:                  %s
   6%s: 	Carrier (16 clocks):               %u Hz
    6%s: 	Carrier duty cycle:                %2u/16
   6%s: 	Max pulse width:                   %u us, %llu ns
   6%s: 	Low pass filter:                   %s
   6%s: 	Min acceptable pulse width (LPF):  %u us, %u ns
 6%s: 	Pulse timers' start/stop trigger:  %s
   3cx23885: %s: i2c write error!
    3cx23885: %s: i2c read error!
 7cx23885: %s: %s: i2c write error, Reg=[0x%02x], Status=%d
    7cx23885: %s: %s: i2c read error, Reg = 0x%02x, Status = %d
   7cx23885: %s: %s: i2c read Addr=0x%04x, Reg = 0x%02x, data = %02x
 7cx23885: %s: %s: Slot Status Addr=[0x%04x], Reg=[0x%02x], data=%02x, TS config = %02x
    7cx23885: %s: %s: %s: chipaddr=[0x%x] addr=[0x%02x], %s=%x
    7cx23885: %s: %s: chipaddr=[0x%x] setting ci IRQ to [0x%x] 
   7cx23885: %s: %s: Wakeup CI0
  7cx23885: %s: %s: Wakeup CI1
  7cx23885: %s: %s: Unable create CI structure!
 7cx23885: %s: %s: CI initialized!
 7cx23885: %s: %s: Cannot initialize CI: Error %d.
 3cx23885: eeprom i2c read error, status=%d
    3cx23885: eeprom i2c write error, status=%d
   3cx23885: %s: timeout, the slave no response
  7cx23885: %s: alsa: Start audio DMA, %d B/line, %d lines/FIFO, %d periods, %d byte buffer
 7cx23885: %s: alsa: Stopping audio DMA
    3cx23885: BUG: cx23885 can't find device struct. Can't proceed with open
  7cx23885: %s: alsa: Error opening PCM!
    drivers/media/pci/cx23885/cx23885-alsa.c    7cx23885: %s: alsa: Freeing buffer
    7cx23885: %s: alsa: vmalloc_32(%lu pages) failed
  7cx23885: %s: alsa: vmalloc is at addr %p, size=%lu
   4cx23885: %s: cx23885_alsa_map_sg failed
  4cx23885: %s/1: Audio risc op code error
  7cx23885: %s: alsa: Downstream sync error
 4cx23885: %s(): Missing SRAM channel configuration for analog TV Audio
    7cx23885: %s: alsa: registered ALSA audio device
  3cx23885: %s(): Failed to register analog audio adapter
 ir-kbd-i2c dvb-netup-altera-04.fw dvb-netup-altera-01.fw cx25840 cs3308 configure UNKNOWN/GENERIC Hauppauge WinTV-HVR1800lp Hauppauge WinTV-HVR1800 Hauppauge WinTV-HVR1250 DViCO FusionHDTV5 Express Hauppauge WinTV-HVR1500Q Hauppauge WinTV-HVR1500 Hauppauge WinTV-HVR1200 Hauppauge WinTV-HVR1700 Hauppauge WinTV-HVR1400 Leadtek Winfast PxDVR3200 H Compro VideoMate E650F TurboSight TBS 6920 TeVii S470 DVBWorld DVB-S2 2005 NetUP Dual DVB-S2 CI Hauppauge WinTV-HVR1270 Hauppauge WinTV-HVR1275 Hauppauge WinTV-HVR1255 Hauppauge WinTV-HVR1210 Mygica X8506 DMB-TH Magic-Pro ProHDTV Extreme 2 Hauppauge WinTV-HVR1850 Compro VideoMate E800 Hauppauge WinTV-HVR1290 Mygica X8558 PRO DMB-TH LEADTEK WinFast PxTV1200 GoTView X5 3D Hybrid NetUP Dual DVB-T/C-CI RF MPX-885 Mygica X8502/X8507 ISDB-T TerraTec Cinergy T PCIe Dual TeVii S471 Prof Revolution DVB-S2 8000 TurboSight TBS 6981 TurboSight TBS 6980 Leadtek Winfast PxPVR2200 Hauppauge ImpactVCB-e DVBSky T9580 DVBSky T980C DVBSky S950C DVBSky S950 DVBSky S952 DVBSky T982 Hauppauge WinTV-HVR5525 Hauppauge WinTV Starburst ViewCast 260e ViewCast 460e Hauppauge WinTV-QuadHD-DVB Hauppauge WinTV-QuadHD-ATSC Hauppauge WinTV-Starburst2 AVerMedia CE310B Television 7cx23885: %s: video:%s()
 cx23885 PCIe:%s %s (%s) 3cx23885: %s() error
 7cx23885: %s: video:%s(%d)
 cx23885-vbi tuner xc3028-v27.fw xc3028L-v36.fw video vbi Baseband L/R 1 Baseband L/R 2 TV Composite1 Composite2 Composite3 Composite4 S-Video Component Cable TV DVB for debug only 7cx23885: %s: vbi:%s()
 INVALID 70x%08x [ %s c %s c count=%d ]
 4cx23885: %s:   risc%d:  6cx23885: %s:   %04d:  7cx23885: %s: %s: %p
 7cx23885: %s: %s()
 cancel 6cx23885: %s: Unsupported
 insmod option autodetected &dev->lock &dev->gpio_lock cx23885[%d] 7cx23885: %s: %s(portno=%d)
 &port->frontends.lock 0.0.4 cx23885 12 13 14 resync cnt0 cnt1 18 19 20 21 22 23 irq1 irq2 eol sol write skip writec jump sync read readc writerm writecm writecr init risc lo init risc hi cdt base cdt size iq base iq size risc pc lo risc pc hi iq wr ptr iq rd ptr cdt current pci target lo pci target hi line / byte VID A VID A (VBI) TS1 B ch4 ch5 TS2 C TV Audio ch8 ch9 ch2 7cx23885: %s: i2c: R 7cx23885: %s: i2c: <R %02x 7cx23885: %s: i2c: %02x 7cx23885: %s: i2c: >
 3cx23885:  ERR: %d
 7 <W %02x %02x c >
 c %02x ??? 6cx23885: %s: scan bus %d:
 ir_video tda10048 dib7000pc lgdt3303 cx24227 cs3308 tda8295 tda9887 cx25837 flatiron eeprom tuner/mt2131/tda8275 tuner/xc3028L %s: Invalid parameter
 sp2 s5h1409_attach symbol:s5h1409_attach mt2131_attach symbol:mt2131_attach lgdt3305_attach symbol:lgdt3305_attach tda18271_attach symbol:tda18271_attach s5h1411_attach symbol:s5h1411_attach tda829x_attach symbol:tda829x_attach lgdt330x_attach symbol:lgdt330x_attach simple_tuner_attach symbol:simple_tuner_attach xc5000_attach symbol:xc5000_attach xc2028_attach symbol:xc2028_attach tda10048_attach symbol:tda10048_attach dib7000p_attach symbol:dib7000p_attach zl10353_attach symbol:zl10353_attach dib0070_attach symbol:dib0070_attach xc4000_attach symbol:xc4000_attach cx24116_attach symbol:cx24116_attach cx24117_attach symbol:cx24117_attach ds3000_attach symbol:ds3000_attach ts2020_attach symbol:ts2020_attach stv0900_attach symbol:stv0900_attach stv6110_attach symbol:stv6110_attach symbol:lnbh24_attach lnbh24_attach 3cx23885: No LNBH24 found!
 lgs8gxx_attach symbol:lgs8gxx_attach mb86a20s_attach symbol:mb86a20s_attach atbm8830_attach symbol:atbm8830_attach max2165_attach symbol:max2165_attach stv0367ter_attach symbol:stv0367ter_attach stv0367cab_attach symbol:stv0367cab_attach drxk_attach symbol:drxk_attach mt2063_attach symbol:mt2063_attach stv090x_attach symbol:stv090x_attach stb6100_attach symbol:stb6100_attach tda10071_cx24118 tda10071 a8293 si2165 m88ds3103_attach symbol:m88ds3103_attach ts2020 si2168 si2157 m88ds3103 m88rs6000t %s lgdt3306a_attach symbol:lgdt3306a_attach xc3028-v27.fw xc3028L-v36.fw %s/2 Television 7cx23885: %s: 417:%s()
 PCIe:%s UNKNOWN v4l-cx23885-enc.fw NTSC-M mpeg %s (%s) PING_FW START_CAPTURE STOP_CAPTURE SET_AUDIO_ID SET_VIDEO_ID SET_PCR_ID SET_FRAME_RATE SET_FRAME_SIZE SET_BIT_RATE SET_GOP_PROPERTIES SET_ASPECT_RATIO SET_DNR_FILTER_MODE SET_DNR_FILTER_PROPS SET_CORING_LEVELS SET_SPATIAL_FILTER_TYPE SET_VBI_LINE SET_STREAM_TYPE SET_OUTPUT_PORT SET_AUDIO_PROPERTIES HALT_FW GET_VERSION SET_GOP_CLOSURE GET_SEQ_END SET_PGM_INDEX_INFO SET_VBI_CONFIG SET_DMA_BLOCK_SIZE GET_PREV_DMA_INFO_MB_10 GET_PREV_DMA_INFO_MB_9 SCHED_DMA_TO_HOST INITIALIZE_INPUT SET_FRAME_DROP_RATE PAUSE_ENCODER REFRESH_INPUT SET_COPYRIGHT SET_EVENT_NOTIFICATION SET_NUM_VSYNC_LINES SET_PLACEHOLDER MUTE_VIDEO MUTE_AUDIO MISC NTSC-JP PAL-BG PAL-DK PAL-I PAL-M PAL-N PAL-Nc PAL-60 SECAM-L SECAM-DK rc-hauppauge rc-nec-terratec-cinergy-xs rc-tevii-nec rc-total-media-in-hand-02 rc-tbs-nec rc-dvbsky rc-tt-1500 cx23885 IR (%s) pci-%s/ir0 cx23885 rby     tby ror rto r
e addr: 0x%llx
 6%s:  info: Enabling pci device
   3%s: Can't enable device %d!
  3%s: No suitable DMA available, card %d
   3%s: Cannot request encoder memory region, card %d
    6%s: Unreasonably low latency timer, setting to 64 (was %d)
   6%s:  info: cx%d (rev %d) at %02x:%02x.%x, irq: %d, latency: %d, memory: 0x%llx
   6%s:  info: attempting ioremap at 0x%llx len 0x%08x
   3%s: ioremap failed. Can't get a window into CX23418 memory and register space
    3%s: Each capture card with a CX23418 needs 64 MB of vmalloc address space for the window
 3%s: Check the output of 'grep Vmalloc /proc/meminfo'
 3%s: Use the vmalloc= kernel command line option to set VmallocTotal to a larger value
    6%s: cx23418 revision %08x (A)
    6%s: cx23418 revision %08x (B)
    6%s: cx23418 revision %08x (Unknown)
  3%s: Could not register A/V decoder subdevice
 4%s: Could not register GPIO reset controllersubdevice; proceeding anyway.
    6%s:  info: activating i2c...
 3%s: Could not initialize i2c
 6%s: Unknown EEPROM encoding
  3%s: Unknown model %d, defaulting to original HVR-1600 (cardtype=1)
   3%s: tveeprom cannot autodetect tuner!
    6%s:  info: Worldwide tuner detected
  6%s:  info: PAL tuner detected
    6%s:  info: NTSC tuner detected
   6%s:  info: SECAM tuner detected
  6%s: No tuner detected, default to NTSC-M
 3%s: Failed to register irq %d
    3%s: Error %d setting up streams
  3%s: Error %d registering devices
 3%s: Error %d on initialization
   6cx18:  Start initialization, version %s
  3cx18:  Exiting, cx18_first_minor must be between 0 and %d
    6cx18:   Debug value must be >= 0 and <= 511!
 3cx18:   Error detecting PCI card
 Simultaneous DVB-T and Analog capture supported,
	except when capturing Analog from the antenna input.
 Experimenters and photos needed for device to work well.
	To help, mail the linux-media list (www.linuxtv.org).
    Experimenters needed for device to work well.
	To help, mail the linux-media list (www.linuxtv.org).
   Yuan MPC718 MiniPCI DVB-T/Analog    Hauppauge HVR-1600 (Preproduction)  Simultaneous Digital and Analog TV capture supported
   6%s:  info: Preparing for firmware halt.
  3%s: %s: couldn't stop CPU to load firmware
   3%s: Unable to open firmware %s
   3%s: Did you put the firmware in the hotplug firmware directory?
  6%s: loaded %s firmware (%zu bytes)
   3%s: unable to open firmware %s
   3%s: did you put the firmware in the hotplug firmware directory?
  6%s:  info: load segment %x-%x
    6%s: loaded %s firmware V%08x (%zu bytes)
 3%s: Could not start the CPU
  6%s: GPIO:  direction 0x%08x, value 0x%08x
    6%s:  info: Resetting IR microcontroller
  6%s:  info: GPIO initial dir: %08x/%08x out: %08x/%08x
    6%s:  info: Resetting XCeive tuner
    4%s: Skipped %s, MDL %d, %d times - it must have dropped out of rotation
  6%s:  info: Deallocating buffers for %s stream
    6%s:  info: Allocate %s stream: %d x %d buffers (%d.%02d kB total)
    3%s: Too many buffers, cannot fit in SCB area
 %s %s: rejecting DMA map of vmalloc memory
 3%s: Couldn't allocate buffers for %s stream
  6%s: ignoring gop_end: not (yet?) supported by the firmware
   6%s:  info: Start encoder stream %s
   6%s:  info: Setup VBI h: %d lines %x bpl %d fr %d %x %x
   6%s:  warning: Error starting capture!
    3%s: Couldn't allocate cx18_dvb structure for %s
  3%s: Couldn't register v4l2 device for %s (device node number %d)
 6%s: Registered device %s for %s (%d x %d.%02d kB)
    6%s: Registered device %s for %s
  6%s: Registered device %s for %s (%d x %d bytes)
  4%s: MPEG Index stream cannot be claimed directly, but something tried.
   6%s:  info: Stream %d is busy
 6%s:  warning: Release stream %s not in use!
  6%s:  info: dualwatch: change stereo flag from 0x%x to 0x%x.
  6%s:  info: dualwatch: changing stereo flag failed
    6%s:  warning: IDX capture start failed
   6%s:  info: IDX capture started
   6%s:  warning: VBI capture start failed
   6%s:  info: VBI insertion started
 6%s:  warning: Failed to start capturing for stream %s
    6%s:  file: read %zd bytes from %s
    6%s:  warning: Stream %s not initialized before read
  6%s:  warning: copy %zd bytes to user failed for %s
   6%s:  file: read %zd from %s, got %zd
 6%s:  info: Could not start capture for %s (%d)
   6%s:  file: Encoder poll started capture
  6%s:  file: Encoder mmap started capture
  6%s:  info: close stopping capture
    6%s:  info: close stopping embedded VBI capture
   6%s:  info: close stopping IDX capture
    3%s: Can't find valid task handle for mute
    3%s: Can't find valid task handle for unmute
  3%s: Failed to initialize on %s
   6%s:  warning: nomem on v4l2 open
 6%s:  ioctl: V4L2_ENC_CMD_START
   6%s:  ioctl: V4L2_ENC_CMD_STOP
    6%s:  ioctl: V4L2_ENC_CMD_PAUSE
   6%s:  ioctl: V4L2_ENC_CMD_RESUME
  6%s:  info: Changing input from %d to %d
  6%s:  info: v4l2 ioctl: set frequency %d
  6%s:  info: Switching standard to %llx.
   6%s: GPIO:  direction 0x%08x, value 0x%08x
    6%s: Stream %s: status 0x%04lx, %d%% of %d KiB (%d buffers) in use
    6%s: Read MPEG/VBI: %lld/%lld bytes
   3%s: Can't find valid task handle for V4L2_ENC_CMD_PAUSE
  3%s: Can't find valid task handle for V4L2_ENC_CMD_RESUME
 6%s:  info: disabled insertion of sliced VBI data into the MPEG stream
    4%s: Unable to allocate buffers for sliced VBI data insertion
 6%s:  info: enabled insertion of sliced VBI data into the MPEG PS,when sliced VBI is enabled
  6%s:  api: %s	cmd %#010x args%s
   4%s: Unknown RPU (%d) for API call
    3%s: mbox was found stuck busy when setting up for %s; clearing busy and trying to proceed
    6%s:  api: waited %u msecs for busy mbox to be acked
  6%s:  irq: sending interrupt SW1: %x to send %s
   6%s:  warning: sending %s timed out waiting %d msecs for RPU acknowledgment
   6%s:  warning: woken up before mailbox ack was ready after submitting %s to RPU.  only waited %d msecs on req %u but awakened with unmatched ack %u
   6%s:  warning: failed to be awakened upon RPU acknowledgment sending %s; timed out waiting %d msecs
   6%s:  api: waited %u msecs for %s to be acked
 6%s:  api: mailbox error %08x for command %s
  4%s: Unhandled RPU (%d) for command %x ack
    6%s:  warning: Possibly falling behind: %s self-ack'ed our incoming %s to EPU mailbox (sequence no. %u) while processing
  4%s: Got DMA done notification for unknown/inactive handle %d, %s mailbox seq no %d
   4%s: Fell behind! Ignoring stale mailbox with  inconsistent data. Lost MDL for mailbox seq no %d
  6%s:  dma: DMA DONE for %s (MDL %d)
   4%s: Could not find MDL %d for stream %s
  6%s:  dma: %s recv bytesused = %d
 4%s: Unknown CPU to EPU mailbox command %#0x
  4%s: Unknown APU to EPU mailbox command %#0x
  4%s: Unable to find blank work order form to schedule incoming mailbox command processing
 6%s:  warning: Possibly falling behind: %s self-ack'ed our incoming %s to EPU mailbox (sequence no. %u)
   6%s:  api: %s: req %#010x ack %#010x cmd %#010x err %#010x args%s
 CX18_CPU_SET_STREAM_OUTPUT_TYPE CX18_CPU_SET_SPATIAL_FILTER_TYPE    CX18_CPU_SET_SCENE_CHANGE_DETECTION CX18_CPU_SET_USERDATA_PLACE_HOLDER  6%s:  irq: received interrupts SW1: %x	SW2: %x  HW2: %x
   BTSC/EIAJ/A2-M Mono (4.5 MHz FMMono)    6%s: Video signal:              %spresent
 6%s: Detected format:           %s
    6%s: Specified standard:        %s
    6%s: Specified video input:     Composite %d
  6%s: Specified video input:     S-Video (Luma In%d, Chroma In%d)
  6%s: Specified audioclock freq: %d Hz
 6%s: Detected audio mode:       %s
    6%s: Detected audio standard:   %s
    6%s: Audio muted:               %s
    6%s: Audio microcontroller:     %s
    6%s: Configured audio standard: %s
    6%s: Configured audio mode:     %s
    6%s: Specified audio input:     Tuner (In%d)
  6%s: Specified audio input:     External
  6%s: Preferred audio mode:      %s
    6%s: Selected 65 MHz format:    %s
    6%s: Selected 45 MHz format:    %s
    6%s: Configured audio system:   %s
    3%s: %dx%d is not a valid size!
   6%s:  info: decoder set size %dx%d -> scale  %ux%u
    6%s:  info: decoder set video input %d, audio input %d
    3%s: 0x%06x is not a valid video input!
   3%s: 0x%04x is not a valid audio input!
   6%s:  info: PLL regs = int: %u, frac: %u, post: %u
    6%s:  info: Video PLL = %d.%06d MHz
   6%s:  info: Pixel rate = %d.%06d Mpixel/sec
   6%s:  info: ADC XTAL/pixel clock decimation ratio = %d.%03d
   6%s:  info: Chroma sub-carrier initial freq = %d.%06d MHz
 6%s:  info: hblank %i, hactive %i, vblank %i, vactive %i, vblank656 %i, src_dec %i, burst 0x%02x, luma_lpf %i, uv_lpf %i, comb 0x%02x, sc 0x%06x
  6%s:  info: changing video std to fmt %i
  automatic standard and mode detection   MONO1 (LANGUAGE A/Mono L+R channel for BTSC, EIAJ, A2)  MONO4 (NICAM ANALOG-Language C/Analog Fallback) 3%s: unable to open firmware %s
   6%s: verified load of %s firmware (%d bytes)
  3%s: verification of %s firmware load failed: expected %#010x got %#010x
  3%s: unable to load firmware %s
   6%s: loaded %s firmware (%d bytes)
    6%s:  info: Stop feed: pid = 0x%x index = %d
  6%s:  info: Stopping Transport DMA
    6%s:  info: Start feed: pid = 0x%x index = %d
 3%s: Failed to initialize firmware starting DVB feed
  6%s:  info: Starting Transport DMA
    6%s:  info: Failed to start Transport DMA
 3%s: Unable to open firmware file %s
  3%s: The MPC718 board variant with the MT352 DVB-T demodulator will not work without it
   3%s: Run 'linux/scripts/get_dvb_firmware mpc718' if you need the firmware
 3%s: Firmware %s has a bad size: %lu bytes
    3DVB: Unable to find symbol s5h1409_attach()
  3DVB: Unable to find symbol mxl5005s_attach()
 3DVB: Unable to find symbol s5h1411_attach()
  3DVB: Unable to find symbol tda18271_attach()
 3DVB: Unable to find symbol zl10353_attach()
  3DVB: Unable to find symbol xc2028_attach()
   3DVB: Unable to find symbol mt352_attach()
    6%s: DVB Frontend registered
  6%s: Registered DVB adapter%d for %s (%d x %d.%02d kB)
    3%s: frontend initialization failed
   p  ?  ?  ?  ?  L0  ?1  2  ?2  ?3  ?  ??  @  @  @  T@  ?A  ?B  ?B  ?D  ?H  %I  ]I  vI  ?R  S  %S  ?S  ?S   T  T  8T  ?U  ?U  V  
V  HV  `V  ?V  W  MW  bW  yW  ?_  +a  3a  ?e  ?e  Vg  i  Rs  ?s  ??  n?  firmware=xc3028-v27.fw version=1.5.1 license=GPL description=CX23418 driver author=Hans Verkuil parm=cx18_first_minor:Set device node number assigned to first card parm=enc_pcm_bufs:Number of encoder PCM buffers
			Default is computed from other enc_pcm_* parameters parm=enc_pcm_bufsize:Size of an encoder PCM buffer (kB)
			Default: 4 parm=enc_pcm_buffers:Encoder PCM buffer memory (MB). (enc_pcm_bufs can override)
			Default: 1 parm=enc_vbi_bufs:Number of encoder VBI buffers
			Default is computed from enc_vbi_buffers parm=enc_vbi_buffers:Encoder VBI buffer memory (MB). (enc_vbi_bufs can override)
			Default: 1 parm=enc_yuv_bufs:Number of encoder YUV buffers
			Default is computed from other enc_yuv_* parameters parm=enc_yuv_bufsize:Size of an encoder YUV buffer (kB)
			Allowed values are multiples of 33.75 kB rounded up
			(multiples of size required for 32 screen lines)
			Default: 102 parm=enc_yuv_buffers:Encoder YUV buffer memory (MB). (enc_yuv_bufs can override)
			Default: 2 parm=enc_idx_bufs:Number of encoder IDX buffers
			Default: 63 parm=enc_idx_bufsize:Size of an encoder IDX buffer (kB)
			Allowed values are multiples of 1.5 kB rounded up
			(multiples of size required for 64 index entries)
			Default: 2 parm=enc_idx_buffers:(Deprecated) Encoder IDX buffer memory (MB)
			Ignored, except 0 disables IDX buffer allocations
			Default: 1 [Enabled] parm=enc_mpg_bufs:Number of encoder MPG buffers
			Default is computed from other enc_mpg_* parameters parm=enc_mpg_bufsize:Size of an encoder MPG buffer (kB)
			Default: 32 parm=enc_mpg_buffers:Encoder MPG buffer memory (MB). (enc_mpg_bufs can override)
			Default: 2 parm=enc_ts_bufs:Number of encoder TS buffers
			Default is computed from other enc_ts_* parameters parm=enc_ts_bufsize:Size of an encoder TS buffer (kB)
			Default: 32 parm=enc_ts_buffers:Encoder TS buffer memory (MB). (enc_ts_bufs can override)
			Default: 1 parm=mmio_ndelay:(Deprecated) MMIO accesses are now never purposely delayed
			Effectively: 0 ns parm=retry_mmio:(Deprecated) MMIO writes are now always checked and retried
			Effectively: 1 [Yes] parm=cx18_pci_latency:Change the PCI latency to 64 if lower: 0 = No, 1 = Yes,
			Default: Yes parm=debug:Debug level (bitmask). Default: 0
			  1/0x0001: warning
			  2/0x0002: info
			  4/0x0004: mailbox
			  8/0x0008: dma
			 16/0x0010: ioctl
			 32/0x0020: file
			 64/0x0040: i2c
			128/0x0080: irq
			256/0x0100: high volume
 parm=ntsc:Set NTSC standard: M, J, K parm=secam:Set SECAM standard: B, G, H, D, K, L, LC parm=pal:Set PAL standard: B, G, H, D, K, I, M, N, Nc, 60 parm=cardtype:Only use this option if your card is not detected properly.
		Specify card type:
			 1 = Hauppauge HVR 1600 (ESMT memory)
			 2 = Hauppauge HVR 1600 (Samsung memory)
			 3 = Compro VideoMate H900
			 4 = Yuan MPC718
			 5 = Conexant Raptor PAL/SECAM
			 6 = Toshiba Qosmio DVB-T/Analog
			 7 = Leadtek WinFast PVR2100
			 8 = Leadtek WinFast DVR3100 H
			 9 = GoTView PCI DVD3 Hybrid
			 10 = Hauppauge HVR 1600 (S5H1411)
			 0 = Autodetect (default)
			-1 = Ignore this card
		 parm=radio:Enable or disable the radio. Use only if autodetection
			fails. 0 = disable, 1 = enable parm=tuner:Tuner type selection,
			see tuner.h for values parmtype=enc_pcm_bufs:int parmtype=enc_vbi_bufs:int parmtype=enc_yuv_bufs:int parmtype=enc_idx_bufs:int parmtype=enc_mpg_bufs:int parmtype=enc_ts_bufs:int parmtype=enc_pcm_bufsize:int parmtype=enc_yuv_bufsize:int parmtype=enc_idx_bufsize:int parmtype=enc_mpg_bufsize:int parmtype=enc_ts_bufsize:int parmtype=enc_pcm_buffers:int parmtype=enc_vbi_buffers:int parmtype=enc_yuv_buffers:int parmtype=enc_idx_buffers:int parmtype=enc_mpg_buffers:int parmtype=enc_ts_buffers:int parmtype=cx18_first_minor:int parmtype=cx18_pci_latency:int parmtype=retry_mmio:int parmtype=mmio_ndelay:int parmtype=debug:int parmtype=ntsc:string parmtype=secam:string parmtype=pal:string parmtype=cardtype:array of int parmtype=radio:array of int parmtype=tuner:array of int firmware=v4l-cx23418-apu.fw firmware=v4l-cx23418-cpu.fw firmware=v4l-cx23418-dig.fw firmware=dvb-cx18-mpc718-mt352.fw parm=adapter_nr:DVB adapter numbers parmtype=adapter_nr:array of short srcversion=1CE209474DA443168BA26F2 alias=pci:v000014F1d00005B7Asv*sd*bc*sc*i* depends=videobuf-core,videodev,dvb-core,tveeprom,cx2341x,videobuf-vmalloc,i2c-core,i2c-algo-bit retpoline=Y intree=Y name=cx18 vermagic=5.18.2-smp SMP preempt mod_unload PENTIUMIII                 ?? ?              ?? ?               ?? ?   0           ?? ?   @           ?? ?   P           ?? ?   \           ?? ?   l           ?? ?   |           ?? ?   ?           ?? ?   ?           ?? ?   ?           ?? ?   ?           ?? ?   ?           ?? ?   ?           ?? ?   ?           ?? ?   ?           ?? ?             ??                ?? ?   4          ?? ?   @          ??    L          ??     T          ?? \  d          ?? l  t          ?? x  ?          ?? ?  ?          ?? ?  ?          ?? ?  ?          $? ?                      D  @                                   "  ?  P  @                                                                                                                                     ?????????????   ????????   f                                - -- --                      ??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????  B C ??a ` ??    ??  C ??  a ` ??                            ?  @               ?      ?  @               ?      ?  @               ?              ?                   ?               P  ?     ??          ?           @                                                                            ?o            ??o          ???   ??               c   ?R  $?    ?   ?     ?????????????????:  ~  F		?]  ?  ? 	             GNU  ?       ?               GNU ????-G????????|L???        Linux                Linux                                           cx18                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0  GCC: (GNU) 11.2.0                                                                                         	                                                                                                                                                                                                                                !              "              #             ??              '             A            	 Y       2     n       c     ?   @   ?    ?   c   B     ?   ?   ]     ?   P      ?   ?       ! _
  ?        a  ?        	
  ?        ?	  ?        [	  ?        	  ?        ?  ?        ?  ?          ?        ?  ?        w  ?        )  ?        X  ?          ?        ?  ?        T  ?        ?  ?        ?               ?    ! ?  ?  ?     j     ?     ?         N          ?           ! ?           ! ?           ! ?           ! ?           !            ! 	          !           !   ?            ?    +           2  ?  c     O      ?     \      ?     l      
     {           ?  ?  @     ?          ?  %        ?  1        ?  L        ?  `   D       ?   g     ;    F     Z  Q  _     y  ?  \     ?    _     ?  k  g     ?  ?  ?     ?  ?  _       ?  ?     *  #  ?     I  ?  ?     h  a  g     ?  ?  G     ?    _     ?  n  d     ?  ?  E     ?    \       s  a     4  ?  d     N  8  ^     n  ?  ?     ?  ?	  %     ?  ?	  4     ?  ?	  :     ?  
  ?    ?    d     ?  g  ;       ?       !           6           O  ?       o          ?          ?  ?       ?  (        ?           ?  ?         <           0        9  
       Y  P        n  @        ?  $       ?  d        ?  P        ?  =       ?  x          \        )  Z       L  ?        d  l        ?  w       ?  ?        ?  |        ?  ?       ?  ?          ?        .  ?       P  ?        g  ?        ?  ?       ?  ?        ?  ?        ?  ?       ?  ?        	  ?        0	         S	         k	  ?        ?	  $       ?	         ?	  ?        ?	  A       
  ,       
  ?        5
  ^       W
  @       n
  ?        ?
  z       ?
  T       ?
         ?
  ?         h                  =  ?       [  |       n  4       c  ?        ?  ?       ?  ?       ?  @       ?         ! ?  ?       ?  ?       ?  L       	  ?       !  ?       .  T       ?  \       S         l  ?       z  d       ?  l       ?  %       ?  ?       ?  t       ?  x       ?  9         ?         ?  	     )  ?       >         I  X       b         p  ?       ?  ?       ?         ?  t       ?         ?  ?       ?  ?       ?         ?           ???  `  (       <                 ,  4       7  ,       B          O         i  ?  ?     ?  ?	  ?     ?  ?  ?     ?  ?  ?     ?  ?  ?     ?  ?  ?     ?  ?  ?       ?  ?     '  ?  ?     >  ?
  ?     X  4       e  ?         ?       ?  ?       ?  ?       ?  ?       ?  ?       ?  ?	                            ??  0  o     &  ?  o     2    %     >  @  %     J  ?        Y          `         i  (       ,  ?       7  ?       ?  <  )     ?  ?       ?  e       ?           ???  z       ?  ?  ?    ?         ?  ?       	  ?       !           ??-            J  "          i  $         	 ?  ?(  G     ?  1       ?  ?)  G     ?   *  ?     ?   +  X     ?  `+  r     ?  ?+  ?     ?  ?,  ?       @-  ?     &  ?        2  @        @  F       [  `  0     n  `  0          ,     ?         ?  ?  P     ?           ???  ?/  ?     ?  [  L     ?  ?  "                  ?  ?     0           ???  #          g  A          ?  0         	 ?  B          ?  _          	  <         	 0   ;  u     =  ?;  1     J  ?;  9    l  0=  "     {  `=  .    ?  ?  0     ?  ?  Z     ?  ?  ?     ?  ?        ! ?  `  $     ?  ?           8     &  W      A           ??P  `          l  r          ?            	 ?  s          ?  ?          ?           	   m  M       ?       2  PT  ?     A  ?  /     U    x     m  }      ?  ?  _     ?  ?  2     ?  ,  |     ?  ?  *     ?  ?  *     ?  ?  $     ?     j                ??  ?i  x     -  j  Z     @  pj  Z     U  ?j      h  ?k  G     {  @l  N     ?  ?l  ?     ?  ?  ?     ?   m       ?  @m  Q     ?  ?m       ?  ?m  Q     ?   n  u     ?  ?n  ?       ?  p       0o  &     +  ?  2     =  ,       S  E  "     c  ?q       s  r       ?  0r       ?  g  ?    ?  Pr  ?     ?  ?r  S    ?  -  ?     ?  @t  a     ?  ?t  ?     ?  @u  a     ?  ?u  X       v  X       pv  X        ?v  ?     .  ?       A  ?       ,  ?       L  `w  ?     d  x  ?    u  `          ?z  Z     ?  P{  ?     ?  |  ?     ?  ?|  2    ?   ~  ?     ?     N      ?  ?               ??,  P?  ?     B   ?       T  ?  G    j  ?  Y     ?  `?  c     ?  |       ?           ???  ??  \     ?  0?  `    ?  ?      ?  V  ?     ?  ??  ?     ?  ?  =       ?         &  1    )  W  ?     C  ?  7     R  ??  ?     h  .       {           ???           ???           ???           ???  G       ?           ???  ??  ?     ?   ?       ?  @?  ?       ?  @       `  {    &  `  <     0     @     :  @        D  ?        N  ?       X  ??  ?     e  ??  ?     v  ?#  -     ?  ??  ?    ?  $  G     ?  ??  ?    ?  P?  >    ?  ??       ?  ??  *     ?  ??  ?       p?        ??       "  ??  ?    ,  O$  Y     ;  ??       S  ??       k  ?$  ?    ?  ??  ?    ?  ?&  :     ?  ?        ?  0       ?     0     ?  ?  ,     ?  ?       ?  `  P     
  @            @     *           ??:  ??  4    J  0?      _           ??r  y&  6    	  ?       ?           ???  `        ?  `       ?           ???           ???  P?  ?     ?  ?(  9     ?  ??  ?      ?(  c       ??  L    1  K)  y     ?   ?       M  |  
     `         z  ?        ! ?  ?       ?  |       ?  ?       ?  ?        ?  ?       ?  `       ?  ?          ?          ?       ,   ?)  ?     C   ?  "     [   @  <     v     $     ?   *  #     ?   0       ?   ?       ?   ?       ?            ???            ??!  M  #     !  p  +     2!  ?  `     H!  ?       `!    	     u!    
     ?!    7     ?!           ?!          ?!  ??  ?     ?!             ?!  p?  ?     ?!             ?!             "  ?  ?     "             %"             5"             N"             b"             w"             ?"             ?"   =  !     ?"             ?"          ! ?"             ?"  `4  +     ?"             ?"  ??  *     
#           #              #             )#             7#  @?  `     I#             [#             h#             x#             ?#             ?#             ?#  ?d  L    ?#             ?#             ?#             ?#  ?I  ?     ?#             $             7$             H$             V$             ^$      
     m$             }$             ?$             ?$             ?$  ??       ?$             ?$  ?J      ?$  `?  ?    ?$  ??  ?     ?$  `  ?     	%   ?  A    %  ??  1     &%   ?  e     <%  ?  ?     J%             [%             q%             {%  0d  s     ?%             ?%  ?  D    ?%             ?%             ?%             ?%             ?%  ?c  ?     &             $&             :&             H&             Y&             n&             y&      ?     ?&  P?  ?     ?&  ??  ?    ?&   $  ?    ?&             ?&             ?&  ??  ?    ?&             '             ''  PU  6    ?  0A  ?    :'             S'  p  ?    /             s  0)  s     e'             ?'             ?'             ?'             ?'             ?'             ?'  ??  L     ?'             ?'             (             #(             +(  0?  |    :(  <       Q(  ??  m    `(  ?  @     ?(             ?(  ?0  D     ?(             ?(  ?  )    ?(  P  `     ?(  P?  F     ?(  ?  ,     ?(  p?  ?    )             )             +)  @?       :)  ?f  _    J)  ?.  ?     ])             m)             ?)             ?)             ?)             ?)             ?)             ?)  ?  {     ?)  0?  A     ?)             *             *             $*             /*             H*             ]*             o*   1       *  ??  k     ?*  ??  ?    ?*             ?*   ?  p     ?*             ?*             ?*  P?  ?     +  ??  G     +             !+             )+             :+             R+   f  M     \+             ?  ?R  
    j+             ?  ?S  ?     ?+             ?+             ?+  ?       ?+             ?+             ?+             ?+   ?  ?    ,             ,  ?4  ?     $,             ?
         ! <,  02  (      ??  ?    O,  p  ?    g,  ?  ?     |,  ?a  ?    ?,  ?W  ?	    ?,  ?1  _     ?,   h  o    ?,             ?,             ?,  p?  ?    ?,             -             -             *-   ?  ?     4-   1  ?     ?"             B-             R-  ??  n    `-             h-  0p  q     y-  ?8  w    ?-             ?-  P?  ?    ?-  ??  n    ?-  ??  ?     ?-  ?  {     ?-  ??  ;     K          ! ?-             .             .             $.  ?-  ?     3.             K.             Z.  P/  ?     p.             ?.             ?.             ?.             ?.  p?       ?.             ?.  Q  ?     ?.  P?  p     /             /  ?Q  3     ,/  `5  ?    =/  ?7  ?     N/  ?*  ?     b/  ?0  )     p/  ?P  ?     ?/             ?/             ?/             ?/             ?/             ?/             0             '0             :0             P0             i0             v0  ??  b     ?0             ?0             ?0             ?0  ??  
     ?0             ?0  p  #     1             1             01             Q1  ??  C     b1             q1             ?1             ?1             ?1   O  w    ?1     ?    ?1             ?1             ?1             ?1  Pf  k     2             2  ?p  3    )2  ?Q  ?     ?2  @?  ?     T2             ^2             s2             ?2             ?2             ?2  `o  ?     ?2  `7  \     ?2  p?  ?    ?2             ?2             ?2             3              cx18-driver.c __kstrtab_cx18_ext_init __kstrtabns_cx18_ext_init __ksymtab_cx18_ext_init request_module_async cx18_eeprom_dump.constprop.0 cx18_remove cx18_remove.cold cx18_read_eeprom.cold cx18_probe cx18_instance __key.6 __key.7 __key.8 __key.9 __key.10 __key.11 __key.12 __key.13 cx18_probe.cold ctrl.5 cx18_init_on_first_open.cold module_start cx18_pci_driver module_cleanup __UNIQUE_ID_firmware420 cx18_pci_tbl __UNIQUE_ID_version419 __UNIQUE_ID_license418 __UNIQUE_ID_description417 __UNIQUE_ID_author416 __UNIQUE_ID_cx18_first_minor415 __UNIQUE_ID_enc_pcm_bufs414 __UNIQUE_ID_enc_pcm_bufsize413 __UNIQUE_ID_enc_pcm_buffers412 __UNIQUE_ID_enc_vbi_bufs411 __UNIQUE_ID_enc_vbi_buffers410 __UNIQUE_ID_enc_yuv_bufs409 __UNIQUE_ID_enc_yuv_bufsize408 __UNIQUE_ID_enc_yuv_buffers407 __UNIQUE_ID_enc_idx_bufs406 __UNIQUE_ID_enc_idx_bufsize405 __UNIQUE_ID_enc_idx_buffers404 __UNIQUE_ID_enc_mpg_bufs403 __UNIQUE_ID_enc_mpg_bufsize402 __UNIQUE_ID_enc_mpg_buffers401 __UNIQUE_ID_enc_ts_bufs400 __UNIQUE_ID_enc_ts_bufsize399 __UNIQUE_ID_enc_ts_buffers398 __UNIQUE_ID_mmio_ndelay397 __UNIQUE_ID_retry_mmio396 __UNIQUE_ID_cx18_pci_latency395 __UNIQUE_ID_debug394 __UNIQUE_ID_ntsc393 __UNIQUE_ID_secam392 __UNIQUE_ID_pal391 __UNIQUE_ID_cardtype390 __UNIQUE_ID_radio389 __UNIQUE_ID_tuner388 __UNIQUE_ID_enc_pcm_bufstype387 __param_enc_pcm_bufs __param_str_enc_pcm_bufs __UNIQUE_ID_enc_vbi_bufstype386 __param_enc_vbi_bufs __param_str_enc_vbi_bufs __UNIQUE_ID_enc_yuv_bufstype385 __param_enc_yuv_bufs __param_str_enc_yuv_bufs __UNIQUE_ID_enc_idx_bufstype384 __param_enc_idx_bufs __param_str_enc_idx_bufs __UNIQUE_ID_enc_mpg_bufstype383 __param_enc_mpg_bufs __param_str_enc_mpg_bufs __UNIQUE_ID_enc_ts_bufstype382 __param_enc_ts_bufs __param_str_enc_ts_bufs __UNIQUE_ID_enc_pcm_bufsizetype381 __param_enc_pcm_bufsize __param_str_enc_pcm_bufsize __UNIQUE_ID_enc_yuv_bufsizetype380 __param_enc_yuv_bufsize __param_str_enc_yuv_bufsize __UNIQUE_ID_enc_idx_bufsizetype379 __param_enc_idx_bufsize __param_str_enc_idx_bufsize __UNIQUE_ID_enc_mpg_bufsizetype378 __param_enc_mpg_bufsize __param_str_enc_mpg_bufsize __UNIQUE_ID_enc_ts_bufsizetype377 __param_enc_ts_bufsize __param_str_enc_ts_bufsize __UNIQUE_ID_enc_pcm_bufferstype376 __param_enc_pcm_buffers __param_str_enc_pcm_buffers __UNIQUE_ID_enc_vbi_bufferstype375 __param_enc_vbi_buffers __param_str_enc_vbi_buffers __UNIQUE_ID_enc_yuv_bufferstype374 __param_enc_yuv_buffers __param_str_enc_yuv_buffers __UNIQUE_ID_enc_idx_bufferstype373 __param_enc_idx_buffers __param_str_enc_idx_buffers __UNIQUE_ID_enc_mpg_bufferstype372 __param_enc_mpg_buffers __param_str_enc_mpg_buffers __UNIQUE_ID_enc_ts_bufferstype371 __param_enc_ts_buffers __param_str_enc_ts_buffers __UNIQUE_ID_cx18_first_minortype370 __param_cx18_first_minor __param_str_cx18_first_minor __UNIQUE_ID_cx18_pci_latencytype369 __param_cx18_pci_latency __param_str_cx18_pci_latency __UNIQUE_ID_retry_mmiotype368 __param_retry_mmio __param_str_retry_mmio __UNIQUE_ID_mmio_ndelaytype367 __param_mmio_ndelay __param_str_mmio_ndelay __UNIQUE_ID_debugtype366 __param_debug __param_str_debug __UNIQUE_ID_ntsctype365 __param_ntsc __param_str_ntsc __param_string_ntsc __UNIQUE_ID_secamtype364 __param_secam __param_str_secam __param_string_secam __UNIQUE_ID_paltype363 __param_pal __param_str_pal __param_string_pal __UNIQUE_ID_cardtypetype362 __param_cardtype __param_str_cardtype __param_arr_cardtype cardtype_c __UNIQUE_ID_radiotype361 __param_radio __param_str_radio __param_arr_radio radio_c __UNIQUE_ID_tunertype360 __param_tuner __param_str_tuner __param_arr_tuner tuner_c cx18-cards.c cx18_card_list input_strs.3 cx18_get_input.cold __func__.2 __func__.1 input_strs.0 cx18_get_audio_input.cold cx18_card_hvr1600_esmt cx18_card_hvr1600_samsung cx18_card_h900 cx18_card_mpc718 cx18_card_cnxt_raptor_pal cx18_card_toshiba_qosmio_dvbt cx18_card_leadtek_pvr2100 cx18_card_leadtek_dvr3100h cx18_card_gotview_dvd3 cx18_card_hvr1600_s5h1411 cx18_i2c_std cx18_pci_leadtek_dvr3100h cx18_pci_leadtek_pvr2100 cx18_pci_toshiba_qosmio_dvbt cx18_pci_cnxt_raptor_pal cx18_pci_gotview_dvd3 cx18_pci_mpc718 cx18_pci_h900 cx18_i2c_nxp cx18-i2c.c cx18_setscl cx18_setsda cx18_getscl cx18_getsda hw_devicenames hw_bus hw_addrs cx18_i2c_register.cold init_cx18_i2c.cold __func__.0 exit_cx18_i2c.cold cx18-firmware.c cx18_halt_firmware.cold cx18_firmware_init.cold __UNIQUE_ID_firmware360 __UNIQUE_ID_firmware359 cx18-gpio.c __kstrtab_cx18_reset_ir_gpio __kstrtabns_cx18_reset_ir_gpio __ksymtab_cx18_reset_ir_gpio gpiomux_log_status cx18_reset_ir_gpio.cold resetctrl_log_status gpio_write gpiomux_s_radio gpiomux_s_std gpio_reset_seq resetctrl_reset gpiomux_s_audio_routing gpiomux_ops resetctrl_ops cx18_reset_tuner_gpio.cold resetctrl_core_ops gpiomux_core_ops gpiomux_tuner_ops gpiomux_audio_ops gpiomux_video_ops cx18-queue.c cx18_queue_flush.part.0 cx18_queue_get_mdl.cold cx18_stream_free.cold __already_done.0 cx18_stream_alloc.cold cx18-streams.c __kstrtab_cx18_start_v4l2_encode_stream __kstrtabns_cx18_start_v4l2_encode_stream __ksymtab_cx18_start_v4l2_encode_stream __kstrtab_cx18_stop_v4l2_encode_stream __kstrtabns_cx18_stop_v4l2_encode_stream __ksymtab_cx18_stop_v4l2_encode_stream buffer_setup buffer_queue _cx18_stream_load_fw_queue.part.0 buffer_release buffer_prepare cx18_stop_v4l2_encode_stream.cold cx18_start_v4l2_encode_stream.cold cx18_stream_info __key.0 cx18_v4l2_enc_fops cx18_videobuf_qops cx18_streams_setup.cold cx18_streams_register.cold cx18-fileops.c __kstrtab_cx18_claim_stream __kstrtabns_cx18_claim_stream __ksymtab_cx18_claim_stream __kstrtab_cx18_release_stream __kstrtabns_cx18_release_stream __ksymtab_cx18_release_stream cx18_claim_stream.cold cx18_release_stream.cold cx18_dualwatch cx18_dualwatch.cold cx18_start_capture.cold cx18_v4l2_read.cold cx18_v4l2_enc_poll.cold cx18_v4l2_mmap.cold cx18_stop_capture.cold cx18_mute.cold cx18_unmute.cold cx18_v4l2_close.cold cx18_v4l2_open.cold cx18-ioctl.c cx18_g_fmt_vid_cap cx18_g_fmt_vbi_cap cx18_try_fmt_vid_cap cx18_s_fmt_vbi_cap cx18_g_pixelaspect cx18_g_selection cx18_enum_fmt_vid_cap formats.0 cx18_g_input cx18_g_frequency cx18_g_std cx18_s_tuner cx18_default cx18_try_encoder_cmd cx18_try_encoder_cmd.cold cx18_s_audio cx18_s_input.cold cx18_s_frequency.cold cx18_s_std.cold cx18_enum_input cx18_g_audio cx18_enumaudio cx18_log_status cx18_g_tuner cx18_encoder_cmd cx18_encoder_cmd.cold cx18_streamoff cx18_streamon cx18_dqbuf cx18_qbuf cx18_querybuf cx18_reqbufs cx18_querycap cx18_querycap.cold __func__.3 select_service_from_set cx18_g_enc_index mapping.1 cx18_try_fmt_vbi_cap cx18_g_sliced_vbi_cap cx18_g_fmt_sliced_vbi_cap cx18_try_fmt_sliced_vbi_cap cx18_s_fmt_sliced_vbi_cap cx18_s_fmt_vid_cap cx18_ioctl_ops cx18-controls.c cx18_s_video_encoding cx18_s_audio_mode cx18_s_stream_vbi_fmt cx18_s_stream_vbi_fmt.cold cx18_s_audio_sampling_freq freqs.0 cx18-mailbox.c u32arr2hex cx18_api_call api_info cx18_api_call.cold mb_ack_irq mb_ack_irq.cold rpu_str cx18_in_work_handler.cold cx18_api_epu_cmd_irq.cold cx18_vapi.cold cx18_set_filter_param cx18_api_func.cold cx18-vbi.c cx18-audio.c cx18-video.c cx18-irq.c cx18_irq_handler.cold cx18-av-core.c cx18_av_g_tuner cx18_av_s_radio cx18_av_log_status fmt_strs.0 cx18_av_log_status.cold CSWTCH.75 CSWTCH.77 CSWTCH.76 CSWTCH.78 CSWTCH.79 cx18_av_init cx18_av_s_stream cx18_av_s_stream.cold cx18_av_set_fmt cx18_av_set_fmt.cold cx18_av_s_ctrl cx18_av_initialize cx18_av_reset cx18_av_load_fw cx18_av_s_tuner input_change cx18_av_s_frequency set_input set_input.cold cx18_av_s_video_routing cx18_av_s_audio_routing cx18_av_std_setup.cold cx18_av_s_std cx18_av_s_std.cold cx18_av_ops cx18_av_ctrl_ops cx18_av_general_ops cx18_av_tuner_ops cx18_av_audio_ops cx18_av_video_ops cx18_av_vbi_ops cx18_av_pad_ops cx18-av-audio.c set_audclk_freq cx18_av_audio_s_ctrl cx18-av-firmware.c cx18_av_loadfw.cold cx18-av-vbi.c lcr2vbi.1 biphase_tbl.0 cx18-scb.c cx18-dvb.c cx18_dvb_stop_feed cx18_dvb_stop_feed.cold cx18_dvb_start_feed cx18_dvb_start_feed.cold yuan_mpc718_mt352_init yuan_mpc718_mt352_init.cold hcw_s5h1411_config hauppauge_tda18271_config __key.3 yuan_mpc718_mt352_demod ctrl.1 hauppauge_hvr1600_config hauppauge_hvr1600_tuner gotview_dvd3_zl10353_demod ctrl.0 leadtek_dvr3100h_demod ctrl.2 yuan_mpc718_zl10353_demod cx18_dvb_register.cold __UNIQUE_ID_firmware361 hauppauge_tda18271_std_map __UNIQUE_ID_adapter_nr360 __UNIQUE_ID_adapter_nrtype359 __param_adapter_nr __param_str_adapter_nr __param_arr_adapter_nr cx18-io.c cx18.mod.c __UNIQUE_ID_srcversion100 __UNIQUE_ID_alias99 __UNIQUE_ID_depends98 __UNIQUE_ID_retpoline97 __UNIQUE_ID_intree96 __UNIQUE_ID_name95 __UNIQUE_ID_vermagic94 _note_9 _note_8 cx18_sw2_irq_disable_cpu videobuf_streamoff cx18_sw2_irq_disable v4l2_fh_exit dvb_dmxdev_init cx18_get_input free_irq is_vmalloc_addr cx2341x_handler_set_busy __check_object_size videobuf_poll_stream video_ioctl2 release_firmware cx18_out_work_handler alloc_workqueue cx18_debug pci_enable_device cx18_flush_queues __msecs_to_jiffies cx18_av_read __this_module memmove snprintf queue_work_on cx18_video_set_io __stack_chk_guard __symbol_put i2c_bit_add_bus v4l2_fh_del v4l2_fh_add finish_wait cx18_stop_capture dma_unmap_page_attrs request_firmware __pci_register_driver cx18_streams_cleanup v4l2_ctrl_handler_init_class dma_sync_single_for_device videobuf_reqbufs memcpy_fromio iounmap cleanup_module param_array_ops prepare_to_wait memcpy kfree cx18_av_read4 videobuf_waiton cx18_streams_setup cx18_av_decode_vbi_line cx18_av_g_sliced_fmt cx18_read_eeprom cx18_init_scb cx18_av_write4 cx18_av_write4_expect exit_cx18_i2c v4l2_subdev_init prepare_to_wait_event __wake_up cx18_vb_timeout videobuf_dqbuf cx18_init_memory _raw_spin_lock_irqsave _raw_spin_lock kmem_cache_alloc_trace videobuf_to_vmalloc cx18_v4l2_mmap __x86_indirect_thunk_edx pci_unregister_driver fortify_panic __release_region dvb_register_adapter __fentry__ init_module cx18_av_s_clock_freq cx18_irq_handler cx18_firmware_init dev_driver_string v4l2_device_register_subdev cx18_av_std_setup videobuf_vmalloc_free dma_map_page_attrs cx18_start_capture __x86_indirect_thunk_ecx cx18_i2c_register v4l2_ctrl_handler_log_status kmalloc_order_trace v4l2_ctrl_handler_setup schedule_timeout schedule __stack_chk_fail cx18_av_and_or video_devdata i2c_new_scanned_device cx2341x_handler_setup strnlen cx18_av_loadfw cx18_av_audio_ctrl_ops cx18_memset_io __mod_pci__cx18_pci_tbl_device_table i2c_del_adapter _cx18_mdl_swap __v4l2_ctrl_s_ctrl init_cx18_i2c cx18_find_hw cx18_service2vbi cx18_get_service_set cx18_process_vbi_data __symbol_get init_wait_entry cx18_set_funcs cx18_v4l2_close cx18_gpio_register dvb_dmx_release __x86_indirect_thunk_edi request_threaded_irq memchr tveeprom_read videobuf_read_stream mod_timer cx18_halt_firmware cx18_av_and_or4 v4l2_fh_is_singular destroy_workqueue mem_section mutex_lock __x86_indirect_thunk_ebp pci_read_config_word videobuf_querybuf cx18_queue_init cx18_av_write cx18_dvb_register v4l2_ctrl_subscribe_event cx18_sw2_irq_enable v4l2_event_pending v4l2_fh_init cx18_av_audio_set_path cx18_setup_page dvb_net_init ioremap dvb_dmx_swfilter video_unregister_device cx18_mute videobuf_qbuf __x86_indirect_thunk_esi __mutex_init dvb_dmxdev_release cx18_cxhdl_ops jiffies_to_msecs current_task _raw_spin_unlock_irqrestore cx18_api_epu_cmd_irq param_ops_short cx18_unload_queues dma_sync_single_for_cpu cx18_queue_get_mdl cx18_init_on_first_open cx18_get_audio_input cx18_v4l2_enc_poll cx18_v4l2_read cx18_dequeue cx18_v4l2_open _copy_to_user __init_waitqueue_head cx18_audio_set_io v4l2_device_register dvb_frontend_detach param_ops_string cx18_vapi _cx18_enqueue dvb_net_release cx18_av_probe sprintf cx18_s_frequency cx18_stream_alloc strscpy cx18_in_work_handler cx18_av_s_sliced_fmt cx18_sw1_irq_disable cx18_msleep_timeout cx18_av_s_raw_fmt mutex_unlock videobuf_iolock init_timer_key cx18_gpio_init __video_register_device __const_udelay cx18_reset_tuner_gpio dvb_unregister_frontend videobuf_streamon pci_write_config_byte ktime_get cx18_av_write4_noretry cancel_work_sync cx18_stop_all_captures cx18_sw1_irq_enable __warn_printk cx18_find_handle cx18_load_queues cx18_stream_free cx18_dvb_unregister cx18_buf_swap cx18_stream_rotate_idx_mdls v4l2_ctrl_new_std videobuf_queue_vmalloc_init v4l2_ctrl_g_ctrl cx2341x_handler_init cx2341x_handler_set_50hz v4l2_ctrl_handler_free v4l2_device_unregister pci_disable_device dvb_register_frontend __x86_indirect_thunk_ebx dma_set_mask cx18_expand_service_set tveeprom_hauppauge_analog autoremove_wake_function v4l2_subdev_call_wrappers cx18_api v4l2_i2c_new_subdev cx18_get_card pci_read_config_byte videobuf_mmap_mapper schedule_timeout_uninterruptible cx18_vapi_result iomem_resource dvb_unregister_adapter dvb_dmx_init param_ops_int cx18_streams_register cx18_init_power strchr pci_write_config_word _raw_spin_unlock cx18_unmute video_device_release_empty cx18_s_std cx18_handle_to_stream cx18_av_write_expect __kmalloc __SCT__might_resched v4l2_event_unsubscribe kmalloc_caches __request_region cx18_s_input _cx18_mdl_sync_for_device cx18_api_func __request_module system_wq videobuf_mmap_free flush_work    # 	        ?    ? (    A   # ]   ? d     o   ? u   ? |     ?   ? ?   ? ?   ? ?   ? ?   ? ?   V   Y   ?    (  ? <  ? C    H   n  ? s  ! {  ? ?   ?  ? ?    ?  1 ?  ? ?   ?  # ?  ? ?  u   6 a  # ?  ? ?  ? ?   ?    ?  ? ?  S    ?      ? 2    ;    D    I  8 Q  # f  ? t    }    ?  4 ?  ? ?    ?    ?  ? ?    ?  1 ?    ?        /    :    E    P    [    f    q    |    ?    ?    ?    ?    ?    ?    8  ? ?    {    ?    ?    ?          ?     %    ?  ? ?        >    ?        ~    ?  ? ?  
  ?  1 	  
  	  1 %	    *	  1 0	  
  5	  1 ;	  
  @	  1 F	  
  K	  1 s	    {	    ?	    ?	  q ?	    ?	    ?	  q ?	    ?	    ?	  q ?	    ?	    ?	  q ?	    ?	  ? 
    
  ? 
    P
  ? ?
  s ?
  ? ?
  ? ?
    ?
        #  ? .    3    8  ? C    H    M  ? X    ]    b  ? ?  ? ?    ?  ? ?    ?  ? ?    ?  ? ?      ?     1  ] M  ? g  ? v    ~    ?  ? ?    ?  i ?    ?    ?    ?  
  ?  1 ?        F    M    ?    ?    ?    ?    ?    ?        	  ?   
    1 1    D  ? W    \    a  8 q  # ?  ? ?  ' ?    ?  ? ?  ? ?  ?   '     "  ? /  ? A  ? a  ? y  / ?  ? 
  ?   ? +  ? Q   b  8 q  # ?    ?  # ?    ?  = ?    ?    	  ? ?  # ?    ?  = ?          ? 1  # ?  #   # A  # q  # ?    ?  ? ?    ?    ?  ? ?  ?   ? C  ? g  ? ?  = ?    ?    ?  ? ?    %  ; =  8 Q  # c   ?  ? ?  ? ?  # ?  ? ?            %    ,    d  ? ?  = ?    ?    ?    ?  ? n  ? ?  ? ?  ? ?  /   ? ?  ? ?  B ?  # ?  ? ?    d  B q  B ?  # ?  ? ?      # ?  # ?  ?    ? U   ? ?   ? J!  ? ?!  ? !$  # 9$  ? ?$  ? ?$    ?$  ? ?$  c ?$    %  ? %    -%  g s%    ?%    ?%  ? ?%  g ?%   ?%    ?%  ? ?%    ?&  ? ?&    ?&  g #'    V'    ['  ? h'  g w'  ? ?'  ? ?'  ? (  ? (  ? k(  ? ?(    ?(  ? ?(  g ?(  8 ?(    ?(  # ?(  [ )  
  )  1 )  ? 1)  # H)  ? O)    q)  ? ?)  / ?)  / ?)  # ?)  [ ?)  
  ?)  1 ?)  ? *  # +  # ++  [ M+  ? a+  # ?+  [ ?+  ? ?+  # !,  [ F,  ? O,  ? T,  ? [,  [ ?,  ? ?,  ? ?,  ? ?,  # A-  # ?-  [ ?-  ? ?-  # .  [ R.  ? g.  ? ?.  
  ?.  1 ?.  # ?.    ?.    ?.   /    /  ? -/  ) ?/    D/    Q/  # m/  ? t/    ?/  ? ?/  / ?/  / ?/  # ?/   0   l0  ? {0  ? ?0  # ?0  # 1  # !1  # m1   ?1  ? ?1  # ?1   2  ? %2  ? 12  # M2  ? n2   ?2    ?2  ? p3  z ?3  ? ?3  ? 64  z J4  ? T4  8 a4  # ?4  # ?4   Q5  ? a5  # ?5  ? ?6  ? ?6  ? 	7  ? a7  # ?7    ?7  # ?7  ? ?7    ?7  y ?7   ?7  ? P8  ? X8   _8   ?8  # ?8  ? ?8    ?8    9  Z %9  , W9  z ?9  ? ?9   ?9  ? ?9   ?9  ? ?9  ? 7:  ? M:    j:    y:  ( ?:  
  ?:  ? ?:   ?:   ?:    ?:   ;  # ?;  # ?;  # ?;  ? <  ? I<  ? w<  ? ?<  ? ?<    =  # 1=  # ==  
 D=  + a=  # &>  
 />  + ?>  ? Y?  
 b?  + ??  # ??  ? ??  ? @    3@  ? F@  ? r@  ? ?@   ?@  ? A    1A  # FA  ? `A  ? ?A  ? ?A  ? B  ? $B  y \B  ? ?B  ? ?B  ? ?C  ? ?C  ? ?C  ? ?C  ? D  ? LD  ? ]D  < ?D  ? ?D  ? ?E  ? ?E  ? AF  ? ?F  ? ?F  ? ?F  / oG  ? vG    ?G  ? H  ? H  ? 2H  ? >H    lH  ? |H  ? ?H  / ?H  / ?H  ? ?H    I  ? <I  ? OI  ? ?I  ? ?I  8 ?I    ?I  # +J  ? 2J  k aJ   rJ  ? ?J  ? ?J  ? ?J  ? ?J  # ?J    K    K  { uK    ?K    L    &L  ? EL  _ ZL  _ oL  _ ?L  _ ?L   ?L  ? ?L  ? 
M    M  ? &M    JM  ? ~M  L ?M  ? ?M    ?M    ?M  ? ?M  ? N  ? ?N    ?N  ? ?N  ? ?N   ?N    O  # tO  ? ?O    ?O    ?O    ?O    ?O  a ?O    1P    SP  ? sP    ?P  # ?P  ? ?P  ? Q  # ?Q  } ?Q  # ?Q  # ?R  # ?R    ?R  ? ?R    |S    ?S  # ?S  ? ?S    ?S  ? T  ? AT  ? QT  # `T  ? oT  ? ?T  / ?T  ? ?T    ?T  [ U  D U  ? U  ? 2U  ? :U    ?U  8 QU  # ~U  n ?U  . 0V  ? pV  . ~V  ? ?V    ?V  . ?V  ? ?V    ?V  ? ?V    ?V  ? ?V    W  p IW  } ^W  } nW  ? uW    ?W  # ?W  ? ?W  ? ?W    ?W  [  X  - X  ? aX  ? uX    ?X  ? bY  u jY  ? Y  ? ?Y  u ?Y  ? ?Y  ? ,Z   ?Z  7 QZ  ? ?Z  I ?Z  ? ?Z  ? ?Z  ? ?Z  ? k\  R ?\  ? ?\  ? N]  ? ?]  ? ?]    __  ? e_  ? s_  ? ?_  C ?_  ? ?_    	`  ? I`  ? Q`    }`  I ?`  ? ?`    ?`  T a  ? a  ? <a  p Aa  ? Ua  ? a    ?a  8 ?a    ?a  # ?a  ? 	b    b  d mb  [ tb  - ?b  ? ?b  ? ?b    ?b  ? ?b  d 'c  \ bc  ? ic    ?c  # ?c  ? ?c  [ ?c  - ?c  ? d  ? d    d  ? d    1d  # Dd   ?d   ?d  v ?d  # ?d  ? ?d    ?d  ? ?d    We  ? ^e    oe  } ?e  } ?e  ? ?e    ?e  } f  # f  ? f    +f  ? 4f    Df  ? Qf  # ef  ? lf    {f  ? ?f    ?f  ? ?f  ? ?f  ? ?f  # ?f  ? ?f    ?f  [  g  ? g  ? %g   ,g  ? Gg  X Rg  l ?g  ? ?g  ? ?g  ? ?g  ? ?g  ? ?g  ? h  ? !h  # /h  : Lh  [ Sh  ~ ^h    jh  ? qh    vh  ? ?h   ?h  e ?h  ? ?h  ? ?h  X i  l 4i   Ei  ? Li  ? Xi  ? _i  ? fi   i  ? ?i    ?i  # j  # qj  # ?j  # k  ? 4k  \ ?k  / ?k  # Al  # ?l  # ?l    ?l    !m  # Am  # vm  / ?m  # ?m  # ?m  / !n  # qn  / ?n  # ?n  ? ?n    ?n    ?n    o    $o    1o  # Io  ? ao  # o  ? ?o    ?o  l ?o  ? ?o  ? ?o  ? p    1p  # Gp  l Mp  ? Tp    ~p  / ?p  ? ?p  # 'q  ? ?q  ? ?q    ?q  \ ?q  # ?q  ? r  # $r   1r  # >r   Qr  # ?r  / ?r    ?r  ? ?r    ?r  ? ?r  # ?r  ? s    "s  ? 9s    ds  ? ms    {s  ? ?s  ? ?s    ?s    ?s  - ?s    ?s  ? ?s    t  l t  ? At  # ?t  ? ?t  # ?t  ? ?t  U u  ? Au  # ?u   ?u  # ?u  m v  # Uv  ^ qv  # ?v   ?v  # ?v    ?v  ? ?v  = w    w    "w  ? >w    Fw  ? aw  # x  # ?x  ? ?y    z  ? 4z  ? Dz  ? kz  ? ?z  # Q{  # |  # {|  ? ?|  P ?|  / ?|  # !~  # f~  ? {~  \ ?~  /   #   ? ?  ? ;?  \ ??  ? 3?  P J?  8 Q?  # ??  # ?  # A?  # K?    Q?  # c?  ? ??  ? ??  P ??  ? ??  ? ??  8 ?  # ?  # \?  ? n?  H ??  ? ??    ??  ? ??  ? ?   ?    (?   =?    S?    a?  # ??    ??  / ??  # ??    ?  ? 1?  # X?  ? y?    ??  u ??  ? ??  ? ??    ?  [ 2?  ? 4?    F?  ? M?  ? ]?    n?   ??  ? ??  ? ??  ? ??  ? ?  ? !?    4?  ? ;?    N?  ? ??  t ??    ??  
  ??  1 ??  6 ??  ? ??  ? ?  ? ?  ? D?  K ]?   ??  ? ??  ? ??  t ??  
  ??  1 ??  6 ??  ? ??  ? ?  t ?    +?  
  0?  1 I?  t U?    b?  
  g?  1 ??    ??  
  ??  1 ??    ??  ? ??  8 ??    ?  t ?  
  ?  1 5?    F?    L?  u l?  ? z?    ?  1 ??  # ?  ? C?    H?    Q?  # l?    ??  ? ??    ??  ? ??    ??    ??  ? ??    4?  ? j?    x?    ??  | ??  ? ??     ?  P M?  ? e?  ? u?  ? ??   ??   ?  j 1?  ? V?   |?   ??   ??  ? 	?  U ?  ? #?  ? ;?  ? M?  ? l?  ? ??   ??  j ??  o ??    ?  # ?  ? ??    ?    }?  ? ??  ? ??  ? ??  ? ??    <?  ? ]?  g q?   ??  g ??  8 ??  
  ??  1 ??  # ??  # !?  # .?  ? H?    Q?    ??  ? ??  8 ??  # ??  ? 8?  ? `?  ? q?  # ??  ? ?  ? B?  ? ??  ? ??  ? ?  ? W?  ? i?    ??  ? ??  ? ??  ? ?  ? !?  ? A?  ? K?    P?    U?    Z?    _?    q?  # ??  ? ??  ? ??  ? ??  ? ??  / ??   '?  ? >?  G )?  / O?  ? j?  8 q?  # ??  ? ??  P 2?  P 5?  ? A?  # d?  ? ??  P ??  o ??  # ??  ? ??    ?   3?   E?  w X?  w c?  ? z?    ??  # !?  # A?  # x?    ??    ??  
  ??  1 ??    ??  
  ??  1 ??    ??    ??  # ??  # ??  ? ??    ??  # p?    x?    ??    ??    ??  ? ??    ??  # Q?  # m?  > ??  5 ??  # ??  # ??  # A?  # ??  # ?  ? q?  # ??  ` ?  ` ?  ? 5?  ? ??  # ??  # ??  ? ??  ? ??    Q?  ? t?  ` ??  ? C?    ??    ??    ??    ??    W?    c?    u?    ??  f ?  ? w?  8 |?    ??  # ??  # ??  # ?  # q?  # ??  # ??  # ??  # 1?  # ??  # ?  ? ?    b?    ??  # O?  ? V?    {?  ` ??  ` ??  * ??  ? ??    ?  ` "?  ` ??  # ??    ??   ?    ?  ? 4?  ? >?    [?  ? e?    ?  ? ??    ??  ? ??    ??  ? ??  ? ??  ? ??  ? ?  ? %?  ? H?  ? R?  ? u?  ? ?  ? ??  ? ??  ) ??  ? ??  # ??   ??   ??   ??   ??   ??   ??   ??  ` ?   .?   ??   P?   a?   r?   ??  ` ??   ??   ??   ??   ??   ??   ?   ?   #?  ` 4?   E?   ]?   n?   ?   ??   ??   ??   ??  ` ??   ??   ??   ?   ?   0?   A?   R?   c?   t?  ` ??   ??   ??   ??   ??   ??   ??   ?  ` 1?  # ??  9 ??  9 ??  9 ?  9 0?  ? O?  ? `?  ` ??  ` ??  9 ?  9 9?  ? Q?  # ^?  ? u?  ? ??  ? ??  ? ??  ` ??   ??  ? ??  ? ?  ? %?  ? =?   Q?  # p?  ? ??  ? ??  ` ??  ? ??  ? ??  ? ??  ? ?  ? ?  ? 1?  # 7?    K?  ? k?  ? {?    ??   ??  ? ??   ??  ? *?  ? 4?  ? @?  	 c?    l?  
  q?  1 z?  ? ??    ??   ??    ??  
  ??  1 ??  	 ?  ? ?  	 8?  	 N?    ??  	 ??    ??    ??    ??  # ??  ? ?  ? %?    0?    e?  ? u?    ??    ??  # ??  * ??  ` ??  ` ??  # ?  ? 5?  * F?  ` W?  ` j?  ` (?  ` >?  ` W?  ` h?  ` s?  ?  ?  ` ?  ` Z?  8 a?  # K?    R?    ?  # ?  g (?  @ Q?  # m?  ? t?    ??  [ ??  ? ??    ??  [ ??  } ??  ? ??  ? ??  # ?  ?  ?    -?  [ 4?  ~ @?  ? H?    }?  [ ??  ? ??    ??  [ ??  . ??  ? ??  ? B?  ? I?    ??  # ??    ??  ? ??  ? ??    ??    ??    "?  o |?  o ??  ? ??  ? ??  8 ??  # ??  ? ?  ? ?    -?    A?  " w?    ??    ??  ? ??  ? ??  / ?  / (?  / q?    v?  J ??    ??  / ??    ??  J ??    ??  o ??    ??  ? ?  ? ?  ? k?  h x?  
  }?  1 ??  
  ??  1 ??    ??    ??  q ?   ?   %?  / 7?  / @?  r H?  O O?  ? Z?  ? ??    ??  J ??    ??  / ??    ??  J ??  / ?    ?  / )?    .?  J G?    N?  / e?    j?  J ??    ??  o ??    ??  ? ??    ??  J ??    ??  / ??    ?  J  ?  / ;?    @?  / J?    Q?  ? V?    [?  J l?    q?    x?  ? }?    ??  J ??    ??    ??  J ??    ??  / ??    ??  J ?  / '?    ,?  / @?    E?  ? O?    T?  ? r?  
  w?  1 ??    ??  ? ??    ??  ? ??    ??  J ??    ??    ??  ? ??    ??  ? ??    ??  J ?    ?    ?  ? 1?    6?  J O?    V?  / c?    h?  ? ??    ??  ? ??    ??  ? ??    ??  J ??    ??    ??  ? ??    ??  J ??    ??    ??  ? ??    ?
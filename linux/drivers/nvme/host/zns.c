nfo: decoder set video input %d, audio input %d
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
			see tuner.h for values parmtype=enc_pcm_bufs:int parmtype=enc_vbi_bufs:int parmtype=enc_yuv_bufs:int parmtype=enc_idx_bufs:int parmtype=enc_mpg_bufs:int parmtype=enc_ts_bufs:int parmtype=enc_pcm_bufsize:int parmtype=enc_yuv_bufsize:int parmtype=enc_idx_bufsize:int parmtype=enc_mpg_bufsize:int parmtype=enc_ts_bufsize:int parmtype=enc_pcm_buffers:int parmtype=enc_vbi_buffers:int parmtype=enc_yuv_buffers:int parmtype=enc_idx_buffers:int parmtype=enc_mpg_buffers:int parmtype=enc_ts_buffers:int parmtype=cx18_first_minor:int parmtype=cx18_pci_latency:int parmtype=retry_mmio:int parmtype=mmio_ndelay:int parmtype=debug:int parmtype=ntsc:string parmtype=secam:string parmtype=pal:string parmtype=cardtype:array of int parmtype=radio:array of int parmtype=tuner:array of int firmware=v4l-cx23418-apu.fw firmware=v4l-cx23418-cpu.fw firmware=v4l-cx23418-dig.fw firmware=dvb-cx18-mpc718-mt352.fw parm=adapter_nr:DVB adapter numbers parmtype=adapter_nr:array of short srcversion=1CE209474DA443168BA26F2 a
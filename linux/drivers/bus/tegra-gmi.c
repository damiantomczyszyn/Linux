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
   6%s:  info: Pixel ra
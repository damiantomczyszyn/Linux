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
   3%s: Unable to ope
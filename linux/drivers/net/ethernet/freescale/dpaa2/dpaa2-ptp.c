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
 ir-kbd-i2c dvb-netup-altera-04.fw dvb-netup-altera-01.fw cx25840 cs3308 configure UNKNOW
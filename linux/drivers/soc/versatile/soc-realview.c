6%s:  file: read %zd bytes from %s
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
 CX18_CPU_SET_STREAM_OUTPUT_TYPE CX18_CPU_SET_SPATIAL_FILTER_TYPE    CX18_CPU_SET_SCENE_CHANGE_DETECTION CX18_CPU_SET_USERDATA_PLACE_HOLDER  6%s:  irq: received interrupts SW1: %x	SW2: %x  HW2
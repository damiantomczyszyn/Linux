tate);
		}
	}

}
EXPORT_SYMBOL(altera_ci_release);

static void altera_pid_control(struct netup_hw_pid_filter *pid_filt,
		u16 pid, int onoff)
{
	struct fpga_internal *inter = pid_filt->internal;
	u8 store = 0;

	/* pid 0-0x1f always enabled, don't touch them */
	if ((pid == 0x2000) || (pid < 0x20))
		return;

	mutex_lock(&inter->fpga_mutex);

	netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR0, (pid >> 3) & 0xff, 0);
	netup_fpga_op_rw(inter, NETUP_CI_PID_ADDR1,
			((pid >> 11) & 0x03) | (pid_filt->nr << 2), 0);

	store = netup_fpga_op_rw(inter, NETUP_CI_PID_DATA, 0, NETUP_CI_FLG_RD);

	if (onoff)/* 0 - on, 1 - off */
		store |= (1 << 
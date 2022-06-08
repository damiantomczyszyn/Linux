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

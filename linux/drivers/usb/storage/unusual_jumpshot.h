_read(ALT_PIN_OUT_SEL));
	dprintk(1, "%s() GPIO2                    0x%08X\n", __func__,
		cx_read(GPIO2));
	dprintk(1, "%s() gpcnt(0x%08X)          0x%08X\n", __func__,
		port->reg_gpcnt, cx_read(port->reg_gpcnt));
	dprintk(1, "%s() gpcnt_ctl(0x%08X)      0x%08x\n", __func__,
		port->reg_gpcnt_ctl, cx_read(port->reg_gpcnt_ctl));
	dprintk(1, "%s() dma_ctl(0x%08X)        0x%08x\n", __func__,
		port->reg_dma_ctl, c
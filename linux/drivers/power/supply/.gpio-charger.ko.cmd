00);
		break;
	}
}

int cx23885_ir_init(struct cx23885_dev *dev)
{
	static struct v4l2_subdev_io_pin_config ir_rxtx_pin_cfg[] = {
		{
			.flags	  = BIT(V4L2_SUBDEV_IO_PIN_INPUT),
			.pin	  = CX23885_PIN_IR_RX_GPIO19,
			.fun
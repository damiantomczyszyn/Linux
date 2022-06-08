.duty_cycle = 25,      /* 25 %   - RC-5 carrier */
	.invert_level = false,
	.invert_carrier_sense = false,
};

int cx23888_ir_probe(struct cx23885_dev *dev)
{
	struct cx23888_ir_state *state;
	struct v4l2_subdev *sd;
	struct v4l2_subdev_ir_parameters d
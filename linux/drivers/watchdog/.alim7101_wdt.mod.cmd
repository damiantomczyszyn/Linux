}

static const struct vb2_ops dvb_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buf
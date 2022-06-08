>drv_priv;
	struct cx23885_buffer *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_free_buffer(dev, buf);
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);

	cx23885_buf_queue(&dev->ts1, buf);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->ts1.mpegq;
	unsigned long flags;
	int ret;

	ret = cx23885_initialize_codec(dev, 1);
	if (ret == 0) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		cx23885_start_dma(&dev->ts1, dmaq, buf);
		return 0;
	}
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_QUEUED);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
	return ret;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx2388
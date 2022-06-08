n interrupt which signals that the previous buffer has been
 * DMAed successfully and that it can be returned to userspace.
 *
 * It also sets the final jump of the previous buffer to the start of the new
 * buffer, thus chaining the new buffer into the DMA chain. This is a single
 * atomic u32 write, so there is no race condition.
 *
 * The end-result of all this that you only get an interrupt when a buffer
 * is ready, so the control flow is very easy.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct cx23885_dev *dev = vb->vb2_queue->drv_priv;
	struct cx23885_buffer   *buf = container_of(vbuf,
		struct cx23885_buffer, vb);
	struct cx23885_buffer   *prev;
	struct cx23885_dmaqueue *q    = &dev->vidq;
	unsigned long flags;

	/* add jump to start */
	buf->risc.cpu[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[0] = cpu_to_le32(RISC_JUMP | RISC_CNT_INC);
	buf->risc.jmp[1] = cpu_to_le32(buf->risc.dma + 12);
	buf->risc.jmp[2] = cpu_to_le32(0); /* bits 63-32 */

	spin_lock_irqsave(&dev->slock, flags);
	if (list_empty(&q->active)) {
		list_add_tail(&buf->queue, &q->active);
		dprintk(2, "[%p/%d] buffer_queue - first active\n",
			buf, buf->vb.vb2_buf.index);
	} else {
		buf->risc.cpu[0] |= cpu_to_le32(RISC_IRQ1);
		prev = list_entry(q->active.prev, struct cx23885_buffer,
			queue);
		list_add_tail(&buf->queue, &q->active);
		prev->risc.jmp[1] = cpu_to_le32(buf->risc.dma);
		dprintk(2, "[%p/%d] buffer_queue - append to active\n",
				buf, buf->vb.vb2_buf.index);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static int cx23885_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

	cx23885_start_video_dma(dev, dmaq, buf);
	return 0;
}

static void cx23885_stop_streaming(struct vb2_queue *q)
{
	struct cx23885_dev *dev = q->drv_priv;
	struct cx23885_dmaqueue *dmaq = &dev->vidq;
	unsigned long flags;

	cx_clear(VID_A_DMA_CTL, 0x11);
	spin_lock_irqsave(&dev->slock, flags);
	while (!list_empty(&dmaq->active)) {
		struct cx23885_buffer *buf = list_entry(dmaq->active.next,
			struct cx23885_buffer, queue);

		list_del(&buf->queue);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

static const struct vb2_ops cx23885_video_qops = {
	.queue_setup    = queue_setup,
	.buf_prepare  = buffer_prepare,
	.buf_finish = buffer_finish,
	.buf_queue    = buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = cx23885_start_streaming,
	.stop_streaming = cx23885_stop_streaming,
};

/* ------------------------------------------------------------------ */
/* VIDEO IOCTLS                                                       */

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);

	f->fmt.pix.width        = dev->width;
	f->fmt.pix.height       = dev->height;
	f->fmt.pix.field        = dev->field;
	f->fmt.pix.pixelformat  = dev->fmt->fourcc;
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * dev->fmt->depth) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
	struct v4l2_format *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	struct cx23885_fmt *fmt;
	enum v4l2_field   field;
	unsigned int      maxw, maxh;

	fmt = format_by_fourcc(f->fmt.pix.pixelformat);
	if (NULL == fmt)
		return -E
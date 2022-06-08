onst struct v4l2_frequency *f)
{
	struct cx23885_dev *dev = video_drvdata(file);
	int ret;

	switch (dev->board)
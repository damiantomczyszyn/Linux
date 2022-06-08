tialize CI: Error %d.\n", __func__, ret);
	kfree(state);
	return ret;
}

void netup_ci_exit(struct cx23885_tspor
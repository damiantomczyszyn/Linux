tialize CI: Error %d.\n", __func__, ret);
	kfree(state);
	return ret;
}

void netup_ci_exit(struct cx23885_tsport *port)
{
	struct netup_ci_state *state;

	if (NULL == port)
		return;

	state = (struct netup_ci_state *)port->port_priv;
	if (NUL
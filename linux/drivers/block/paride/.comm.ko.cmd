t_nr)
{
	if (temp_int == NULL)
		return NULL;

	if ((temp_int->pid_filt[filt_nr]) == NULL)
		return NULL;

	if (temp_int->pid_filt[filt_nr]->demux == demux_dev)
		return temp_int;

	return NUL
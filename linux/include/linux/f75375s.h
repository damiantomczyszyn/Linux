filt->nr = hw_filt_nr - 1;
	/* store old feed controls */
	pid_filt->start_feed = config->demux->start_feed;
	pid_filt->stop_feed = config->demux->stop_feed;
	/* replace with new feed controls */
	if (hw_filt_nr == 1) {
		pid_filt->demux->start_feed = altera_ci_start_feed_1;
		pid_filt->demux->stop_feed = altera_ci_stop_feed_1;
	} else if (hw_filt_nr == 2) {
		pid_filt->demux->start_feed = altera_ci_start_feed_2;
		pid_filt->demux->stop_feed = altera_ci_stop_feed_2;
	}

	altera_toggle_fullts_streaming(pid_filt, 0, 1);

	return 0;
err:

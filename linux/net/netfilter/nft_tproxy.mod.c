s[0]),
				       GFP_KERNEL);
		if (reader_tasks == NULL) {
			TOROUT_ERRSTRING("reader_tasks: Out of memory");
			kfree(writer_tasks);
			writer_tasks = NULL;
			firsterr = -ENOMEM;
			goto unwind;
		}
	}

	/*
	 * Create the kthreads and start torturing (oh, those poor little locks).
	 *
	 * TODO: Note that we interleave writers with readers, giving writers a
	 * slight advantage, by creating its kthread first. This can be modified
	 * for very specific needs, or even let the user choose the policy, if
	 * ever wanted.
	 */
	for (i = 0, j = 0; i < cxt.nrealwriters_stress ||
		    j < cxt.nrealreaders_stress; i++, j++) {
		if (i >= cxt.nrealwriters_stress)
			goto cre
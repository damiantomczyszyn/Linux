npinned lock\n"))
				return;

			hlock->pin_count -= cookie.val;

			if (WARN((int)hlock->pin_count < 
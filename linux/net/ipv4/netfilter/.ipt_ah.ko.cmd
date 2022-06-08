{
			req.ret = -ERANGE;
			return req;
		}

		req.util = req.percent << SCHED_CAPACITY_SHIFT;
		req.util = DIV_ROUND_CLOSEST_ULL(req.util, UCLAMP_PERCENT_SCALE);
	}

	return req;
}

static ssi
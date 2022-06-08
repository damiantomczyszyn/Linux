ient_demod))
			goto frontend_detach;
		if (!try_module_get(client_demod->dev.driver->owner
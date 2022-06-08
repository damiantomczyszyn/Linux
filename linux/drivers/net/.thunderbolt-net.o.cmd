!try_module_get(client_tuner->dev.driver->owner)) {
				i2c_unregister_device(client_tuner);
				client_tuner = NULL
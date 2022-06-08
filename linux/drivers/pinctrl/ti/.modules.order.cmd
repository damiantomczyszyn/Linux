ient_tuner);
				module_put(client_demod->dev.driver->owner);
				i2c_unregister_device(client_demod);

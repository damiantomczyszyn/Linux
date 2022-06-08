		sn);
}

static void hauppauge_eeprom(struct cx23885_dev *dev, u8 *eeprom_data)
{
	struct tveeprom tv;

	tveeprom_hauppauge_analog(&tv, eeprom_data);

	/* Make sure we support
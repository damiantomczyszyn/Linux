lient.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom, sizeof(eeprom));
		pr_info("TeVii S470 MAC= %pM\n", eeprom + 0xa0);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xa0, 6);
		return 0;
		}
	case CX23885_BOARD_DVBSKY_T9580:
	case CX23885_BOARD_DVBSKY_S950:
	case CX23885_BOARD_DVBSKY_S952:
	case CX23885_BOARD_DVBSKY_T982: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */

		if (port->nr > 2)
			return 0;

		/* Read entire EEPROM */
		dev->i2c_bus[0].i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_bus[0].i2c_client, eeprom,
				sizeof(eeprom));
		pr_info("%s port %d MAC address: %pM\n",
			cx23885_boards[dev->board].name, port->nr,
			eeprom + 0xc0 + (port->nr-1) * 8);
		memcpy(port->frontends.adapter.proposed_mac, eeprom + 0xc0 +
			(port->nr-1) * 8, 6);
		return 0;
		}
	case CX23885_BOARD_DVBSKY_S950C:
	case CX23885_BOARD_DVBSKY_T980C:
	case CX23885_BOARD_TT_CT2_4500_CI: {
		u8 eeprom[256]; /* 24C02 i2c eeprom */
		struct sp2_config sp2_config;
		struct i2c_board_info info;
		struct cx23885_i2c *i2c_bus = &dev->i2c_bus[0];

		/* attach CI */
		memset(
00000036);
		cx_write(MC417_OEN, 0x00001000);
		cx_set(MC417_RWD, 0x00000002);
		msleep(200);
		cx_clear(MC417_RWD, 0x00000800);
		msleep(200);
		cx_set(MC417_RWD, 0x000008
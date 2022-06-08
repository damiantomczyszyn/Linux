		mem &= ~ALT_DATA;
		mem |= (data & ALT_DATA);
	}

	if (flag)
		mem |= ALT_AD_RG;
	else
		mem &= ~ALT_AD_RG;

	mem &= ~ALT_CS;
	if (read)
		mem = (mem & ~ALT_RD) | ALT_WR;
	else
		mem
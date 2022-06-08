nt));
	WRITE_ONCE(rdp->dynticks_nesting, 1);
	WARN_ON_ONCE(rdp->dynticks_nmi_nesting);
	WRITE_ONCE(rdp->dynticks_nmi_nesting, DYNTICK_IRQ_NONIDLE);
	instrum
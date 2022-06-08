ine void cx23885_irq_disable_all(struct cx23885_dev *dev)
{
	cx23885_irq_disable(dev, 0xffffffff);
}

void cx23885_irq_remove(struct cx23885_dev *dev, u32 mask)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	dev->pci_irqmask &= ~mask;
	cx_clear(PCI_INT_MSK, mask);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
}

static u32 cx23885_irq_get_mask(struct cx23885_dev *dev)
{
	u32 v;
	unsigned long flags;
	spin_lock_irqsave(&dev->pci_irqmask_lock, flags);

	v = cx_read(PCI_INT_MSK);

	spin_unlock_irqrestore(&dev->pci_irqmask_lock, flags);
	return v;
}

static int cx23885_risc_decode(u32 risc)
{
	static char *instr[16] = {
		[RISC_SYNC    >> 28] = "sync",
		[RISC_WRITE   >> 28] = "write",
		[RISC_WRITEC  >> 28] = "writec",
		[RISC_READ    >> 28] = "read",
		[RISC_READC   >> 28] = "readc",
		[RISC_JUMP    >> 28] = "jump",
		[RISC_SKIP    >> 28] = "skip",
		[RISC_WRITERM >> 28] = "writerm",
		[RISC_WRITECM >> 28] = "writecm",
		[RISC_WRITECR >> 28] = "writecr",
	};
	static int incr[16] = {
		[RISC_WRITE   >> 28] = 3,
		[RISC_JUMP    >> 28] = 3,
		[RISC_SKIP    >> 28] = 1,
		[RISC_SYNC    >> 28] = 1,
		[RISC_WRITERM >> 28] = 3,
		[RISC_WRITECM >> 28] = 3,
		[RISC_WRITECR >> 28] = 4,
	};
	static char *bits[] = {
		"12",   "13",   "14",   "resync",
		"cnt0", "cnt1", "18",   "19",
		"20",   "21",   "22",   "23",
		"irq1", "irq2", "eol",  "sol",
	};
	int i;

	printk(KERN_DEBUG "0x%08x [ %s", risc,
	       instr[risc
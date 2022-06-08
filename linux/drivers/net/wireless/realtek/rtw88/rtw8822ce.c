cx23885_irq_add_enable(dev, PCI_MSK_GPIO0);
		break;
	}

	/*
	 * The CX2388[58] IR controller can start firing interrupts when
	 * enabled, so these have to take place after the cx23885_irq() handler
	 * is hooked up by the call to request_irq() above.
	 */
	cx23885_ir_pci_int_enable(dev);
	cx23885_input_init(dev);

	return 0;

fail_irq:
	cx23885_dev_unregister(dev);
fail_ctrl:
	v4l2_ctrl_handler_free(hdl);
	v4l2_device_unregister(&dev->v4l2_dev);
fail_free:
	kfree(dev);
	return err;
}

static void cx23885_finidev(struct pci_dev *pci_dev)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct cx23885_dev *dev = to_cx23885(v4l2_dev);

	cx23885_input_fini(dev);
	cx23885_ir_fini(dev);

	cx23885_shutdown(dev);

	/* unregister stuff */
	free_irq(pci_dev->irq, dev);

	pci_disable_device(pci_dev);

	cx23885_dev_unregister(dev);
	v4l2_ctrl_handler_free(&dev->ctrl_handler);
	v4l2_device_
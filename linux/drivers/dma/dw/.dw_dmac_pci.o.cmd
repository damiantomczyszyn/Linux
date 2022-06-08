00d00);
		/* enable irq */
		cx_write(GPIO_ISM, 0x00000000);/* INTERRUPTS active low*/
		break;
	case CX23885
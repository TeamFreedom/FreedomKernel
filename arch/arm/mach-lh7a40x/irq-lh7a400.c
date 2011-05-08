/* arch/arm/mach-lh7a40x/irq-lh7a400.c
 *
 *  Copyright (C) 2004 Coastal Environmental Systems
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach/irq.h>
#include <mach/irqs.h>

#include "common.h"

  /* CPU IRQ handling */

static void lh7a400_mask_irq(struct irq_data *d)
{
	INTC_INTENC = (1 << d->irq);
}

static void lh7a400_unmask_irq(struct irq_data *d)
{
	INTC_INTENS = (1 << d->irq);
}

static void lh7a400_ack_gpio_irq(struct irq_data *d)
{
	GPIO_GPIOFEOI = (1 << IRQ_TO_GPIO (d->irq));
	INTC_INTENC = (1 << d->irq);
}

static struct irq_chip lh7a400_internal_chip = {
	.name		= "MPU",
	.irq_ack	= lh7a400_mask_irq, /* Level triggering -> mask is ack */
	.irq_mask	= lh7a400_mask_irq,
	.irq_unmask	= lh7a400_unmask_irq,
};

static struct irq_chip lh7a400_gpio_chip = {
	.name		= "GPIO",
	.irq_ack	= lh7a400_ack_gpio_irq,
	.irq_mask	= lh7a400_mask_irq,
	.irq_unmask	= lh7a400_unmask_irq,
};


  /* IRQ initialization */

void __init lh7a400_init_irq (void)
{
	int irq;

	INTC_INTENC = 0xffffffff;	/* Disable all interrupts */
	GPIO_GPIOFINTEN = 0x00;		/* Disable all GPIOF interrupts */
	barrier ();

	for (irq = 0; irq < NR_IRQS; ++irq) {
		switch (irq) {
		case IRQ_GPIO0INTR:
		case IRQ_GPIO1INTR:
		case IRQ_GPIO2INTR:
		case IRQ_GPIO3INTR:
		case IRQ_GPIO4INTR:
		case IRQ_GPIO5INTR:
		case IRQ_GPIO6INTR:
		case IRQ_GPIO7INTR:
			set_irq_chip (irq, &lh7a400_gpio_chip);
			set_irq_handler (irq, handle_level_irq); /* OK default */
			break;
		default:
			set_irq_chip (irq, &lh7a400_internal_chip);
			set_irq_handler (irq, handle_level_irq);
		}
		set_irq_flags (irq, IRQF_VALID);
	}

	lh7a40x_init_board_irq ();

/* *** FIXME: the LH7a400 does use FIQ interrupts in some cases.  For
   the time being, these are not initialized. */

/*	init_FIQ(); */
}

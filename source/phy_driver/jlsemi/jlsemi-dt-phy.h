/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device Tree constants for JLSemi jl1xxx PHY
 *
 * Author: Gangqiao Kuang
 *
 * Copyright (c) 2021 JLSemi Corporation
 */

#ifndef _DT_BINDINGS_JLSEMI_JL1XXX_H
#define _DT_BINDINGS_JLSEMI_JL1XXX_H

/* PHY LED Modes */
#define JL1XXX_LED0_STRAP		(1 << 0)
#define JL1XXX_LED0_EEE			(1 << 1)
#define JL1XXX_LED0_100_ACTIVITY	(1 << 2)
#define JL1XXX_LED0_10_ACTIVITY		(1 << 3)
#define JL1XXX_LED0_100_LINK		(1 << 4)
#define JL1XXX_LED0_10_LINK		(1 << 5)
#define JL1XXX_LED1_STRAP		(1 << 8)
#define JL1XXX_LED1_EEE			(1 << 9)
#define JL1XXX_LED1_100_ACTIVITY	(1 << 10)
#define JL1XXX_LED1_10_ACTIVITY		(1 << 11)
#define JL1XXX_LED1_100_LINK		(1 << 12)
#define JL1XXX_LED1_10_LINK		(1 << 13)

/* PHY LED As Gpio Output */
#define JL1XXX_GPIO_LED0_OUT		(1 << 2)
#define JL1XXX_GPIO_LED1_OUT		(1 << 3)
#define JL1XXX_GPIO_LED0_EN		(1 << 14)
#define JL1XXX_GPIO_LED1_EN		(1 << 15)

#define LED_CTRL_EN		0

#define JL1XXX_CFG_LED_MODE	(JL1XXX_LED0_100_LINK | \
				 JL1XXX_LED0_10_LINK | \
				 JL1XXX_LED1_100_ACTIVITY | \
				 JL1XXX_LED1_10_ACTIVITY)

#define JL1XXXX_CFG_GPIO	(JL1XXX_GPIO_LED0_EN | \
				 JL1XXX_GPIO_LED0_OUT | \
				 JL1XXX_GPIO_LED1_EN | \
				 JL1XXX_GPIO_LED1_OUT)

/* PHY LED Global Period */
#define JL1XXX_GLOBAL_PERIOD_MS		0x10

/* PHY LED Global Hold On */
#define JL1xxx_GLOBAL_ON_MS		0x8

/* PHY peration mode select */
#define CONFIG_C_MACRO_MODE		true

#define CONFIG_DEVICE_TREE_MODE		true

#define CONFIG__ETHTOOL_MODE		true

#endif


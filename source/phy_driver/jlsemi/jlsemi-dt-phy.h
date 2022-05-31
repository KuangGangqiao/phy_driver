/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device Tree constants for JLSemi jl1xxx PHY
 *
 * Author: Gangqiao Kuang
 *
 * Copyright (c) 2021 JLSemi Corporation
 */

#ifndef _DT_BINDINGS_JLSEMI_PHY_H
#define _DT_BINDINGS_JLSEMI_PHY_H

/******************************** JL1XXX-LED ************************************/
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

/* PHY LED Control Enable Mask */
#define JL1XXX_LED_STATIC_OP_EN		(1 << 0)
#define JL1XXX_LED_DYNAMIC_OP_EN	(1 << 1)
#define JL1XXX_LED_MODE_EN		(1 << 2)
#define JL1XXX_LED_GLOABL_PERIOD_EN	(1 << 3)
#define JL1XXX_LED_GLOBAL_ON_EN		(1 << 4)
#define JL1XXX_LED_GPIO_OUT_EN		(1 << 5)

/* PHY LED operation mode select */
#define JL1XXX_LED_C_MACRO_MODE		true
#define JL1XXX_LED_DEVICE_TREE_MODE	true
#define JL1XXX_LED_ETHTOOL_MODE		false

#define JL1XXX_LED_CTRL_EN	(JL1XXX_LED_STATIC_OP_EN | \
				 JL1XXX_LED_DYNAMIC_OP_EN)

#define JL1XXX_CFG_LED_MODE	(JL1XXX_LED0_100_LINK | \
				 JL1XXX_LED0_10_LINK | \
				 JL1XXX_LED1_100_ACTIVITY | \
				 JL1XXX_LED1_10_ACTIVITY)

#define JL1XXX_CFG_GPIO		(JL1XXX_GPIO_LED0_EN | \
				 JL1XXX_GPIO_LED0_OUT | \
				 JL1XXX_GPIO_LED1_EN | \
				 JL1XXX_GPIO_LED1_OUT)

/* PHY LED Global Period */
#define JL1XXX_GLOBAL_PERIOD_MS		0x10

/* PHY LED Global Hold On */
#define JL1XXX_GLOBAL_ON_MS		0x8
/**************************************************************************/

/****************************** JL1XXX-WOL *********************************/
/* PHY WOL Control Enable Mask */
#define JL1XXX_WOL_STATIC_OP_EN		(1 << 0)
#define JL1XXX_WOL_DYNAMIC_OP_EN	(1 << 1)

/* PHY WOL peration mode select */
#define JL1XXX_WOL_C_MACRO_MODE		true
#define JL1XXX_WOL_DEVICE_TREE_MODE	true
#define JL1XXX_WOL_ETHTOOL_MODE		true

#define JL1XXX_WOL_CTRL_EN	(JL1XXX_WOL_STATIC_OP_EN | \
				 JL1XXX_WOL_DYNAMIC_OP_EN)

/*************************************************************************/

/****************************** JL1XXX-INTR ******************************/
/* PHY Interrupt Control Enable Mask */
#define JL1XXX_INTR_STATIC_OP_EN	(1 << 0)
#define JL1XXX_INTR_DYNAMIC_OP_EN	(1 << 1)
#define JL1XXX_INTR_LINK_CHANGE_EN	(1 << 2)
#define JL1XXX_INTR_AN_ERR_EN		(1 << 3)

/* PHY Interrupt Operation Mode Select */
#define JL1XXX_INTR_C_MACRO_MODE	true
#define JL1XXX_INTR_DEVICE_TREE_MODE	true
#define JL1XXX_INTR_ETHTOOL_MODE	true

#define JL1XXX_INTR_CTRL_EN	(JL1XXX_INTR_STATIC_OP_EN | \
				 JL1XXX_INTR_LINK_CHANGE_EN)
/*************************************************************************/

/******************************** JL2XXX-LED *****************************/
/* PHY LED Modes */
#define JL2XXX_LED0_LINK10		(1 << 0)
#define JL2XXX_LED0_LINK100		(1 << 1)
#define JL2XXX_LED0_LINK1000		(1 << 3)
#define JL2XXX_LED0_ACTIVITY		(1 << 4)
#define JL2XXX_LED1_LINK10		(1 << 5)
#define JL2XXX_LED1_LINK100		(1 << 6)
#define JL2XXX_LED1_LINK1000		(1 << 8)
#define JL2XXX_LED1_ACTIVITY		(1 << 9)
#define JL2XXX_LED2_LINK10		(1 << 10)
#define JL2XXX_LED2_LINK100		(1 << 11)
#define JL2XXX_LED2_LINK1000		(1 << 13)
#define JL2XXX_LED2_ACTIVITY		(1 << 14)
/* mode_A = 0 and mode_B = 1 default mode_A */
#define JL2XXX_LED_GLB_MODE_B		(1 << 15)

/* PHY LED Polarity */
#define JL2XXX_LED0_POLARITY		(1 << 12)
#define JL2XXX_LED1_POLARITY		(1 << 11)
#define JL2XXX_LED2_POLARITY		(1 << 10)

/* PHY LED Control Enable Mask */
#define JL2XXX_LED_STATIC_OP_EN		(1 << 0)
#define JL2XXX_LED_DYNAMIC_OP_EN	(1 << 1)
#define JL2XXX_LED_MODE_EN		(1 << 2)
#define JL2XXX_LED_GLOABL_PERIOD_EN	(1 << 3)
#define JL2XXX_LED_GLOBAL_ON_EN		(1 << 4)
#define JL2XXX_LED_POLARITY_EN		(1 << 5)

/* PHY LED Operation Mode Select */
#define JL2XXX_LED_C_MACRO_MODE		true
#define JL2XXX_LED_DEVICE_TREE_MODE	true
#define JL2XXX_LED_ETHTOOL_MODE		true

#define JL2XXX_LED_CTRL_EN	(JL2XXX_LED_STATIC_OP_EN | \
				 JL2XXX_LED_DYNAMIC_OP_EN)

#define JL2XXX_CFG_LED_MODE	(JL2XXX_LED0_LINK10 | \
				 JL2XXX_LED0_ACTIVITY | \
				 JL2XXX_LED1_LINK100 | \
				 JL2XXX_LED1_ACTIVITY | \
				 JL2XXX_LED2_LINK1000 | \
				 JL2XXX_LED2_ACTIVITY)

#define JL2XXX_LED_POLARITY	(JL2XXX_LED0_POLARITY | \
				 JL2XXX_LED1_POLARITY | \
				 JL2XXX_LED2_POLARITY)

/* PHY LED Global Period */
#define JL2XXX_GLOBAL_PERIOD_MS		0x3

/* PHY LED Global Hold On */
#define JL2XXX_GLOBAL_ON_MS		0x2
/*************************************************************************/

/******************************** JL2XXX-FLD *****************************/
/* PHY Fast Link Down Control Enable Mask */
#define JL2XXX_FLD_STATIC_OP_EN		(1 << 0)
#define JL2XXX_FLD_DYNAMIC_OP_EN	(1 << 1)
#define JL2XXX_FLD_DELAY_EN		(1 << 2)

/* PHY Fast Link Down Operation Mode Select */
#define JL2XXX_FLD_C_MACRO_MODE		true
#define JL2XXX_FLD_DEVICE_TREE_MODE	false
#define JL2XXX_FLD_ETHTOOL_MODE		true

#define JL2XXX_FLD_CTRL_EN		(JL2XXX_FLD_STATIC_OP_EN | \
					 JL2XXX_FLD_DYNAMIC_OP_EN)
#define JL2XXX_FLD_DELAY		0
/*************************************************************************/

/******************************** JL2XXX-WOL *****************************/
/* PHY WOL Control Enable Mask */
#define JL2XXX_WOL_STATIC_OP_EN		(1 << 0)
#define JL2XXX_WOL_DYNAMIC_OP_EN	(1 << 1)

/* PHY WOL Operation Mode Select */
#define JL2XXX_WOL_C_MACRO_MODE		true
#define JL2XXX_WOL_DEVICE_TREE_MODE	true
#define JL2XXX_WOL_ETHTOOL_MODE		true

#define JL2XXX_WOL_CTRL_EN	(JL2XXX_WOL_STATIC_OP_EN | \
				 JL2XXX_WOL_DYNAMIC_OP_EN)
/*************************************************************************/

/******************************** JL2XXX-INTR ****************************/
/* PHY Interrupt Control Enable Mask */
#define JL2XXX_INTR_STATIC_OP_EN	(1 << 0)
#define JL2XXX_INTR_DYNAMIC_OP_EN	(1 << 1)
#define JL2XXX_INTR_LINK_CHANGE_EN	(1 << 2)
#define JL2XXX_INTR_AN_ERR_EN		(1 << 3)
#define JL2XXX_INTR_AN_COMPLETE_EN	(1 << 4)
#define JL2XXX_INTR_AN_PAGE_RECE	(1 << 5)

/* PHY Interrupt Operation Mode Select */
#define JL2XXX_INTR_C_MACRO_MODE	true
#define JL2XXX_INTR_DEVICE_TREE_MODE	true
#define JL2XXX_INTR_ETHTOOL_MODE	true

#define JL2XXX_INTR_CTRL_EN	(JL2XXX_INTR_STATIC_OP_EN | \
				 JL2XXX_INTR_LINK_CHANGE_EN)
/*************************************************************************/

/******************************** JL2XXX-DSFT ****************************/
/* PHY Downshift Control Enable Mask */
#define JL2XXX_DSFT_STATIC_OP_EN	(1 << 0)
#define JL2XXX_DSFT_DYNAMIC_OP_EN	(1 << 1)

/* PHY Downshift Operation Mode Select */
#define JL2XXX_DSFT_C_MACRO_MODE	true
#define JL2XXX_DSFT_DEVICE_TREE_MODE	true
#define JL2XXX_DSFT_ETHTOOL_MODE	true

#define JL2XXX_DSFT_CTRL_EN	(JL2XXX_DSFT_STATIC_OP_EN | \
				 JL2XXX_DSFT_DYNAMIC_OP_EN)

#define JL2XXX_DSFT_AN_CNT		3
/*************************************************************************/

#endif


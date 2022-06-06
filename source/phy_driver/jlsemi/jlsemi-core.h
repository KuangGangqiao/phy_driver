/*
 * Copyright (C) 2021 JLSemi Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#ifndef _JLSEMI_CORE_H
#define _JLSEMI_CORE_H

#include <linux/phy.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/of.h>
#include "jlsemi-dt-phy.h"

#define JL1XXX_PHY_ID		0x937c4023
#define JL2XXX_PHY_ID		0x937c4032
#define JLSEMI_PHY_ID_MASK	0xfffffff0

#define JL1XXX_PAGE0		0
#define JL1XXX_PAGE7		7
#define JL1XXX_INTR_REG		19
#define JL1XXX_INTR_LINK	BIT(13)
#define JL1XXX_INTR_AN_ERR	BIT(11)
#define JL1XXX_LED_REG		19
#define JL1XXX_LED_EN		BIT(3)
#define JL1XXX_INTR_STATUS_REG	30

#define JL1XXX_PAGE7		7
#define JL1XXX_RMII_CTRL_REG	16
#define JL1XXX_RMII_OUT		BIT(12)
#define JL1XXX_RMII_MODE	BIT(3)

#define JL1XXX_PAGE129		129
#define JL1XXX_LED_MODE_REG	24
#define JL1XXX_MAC_ADDR0_REG	25
#define JL1XXX_MAC_ADDR1_REG	26
#define JL1XXX_MAC_ADDR2_REG	27
#define JL1XXX_WOL_CTRL_REG	28
#define JL1XXX_WOL_DIS		BIT(15)
#define JL1XXX_WOL_CLEAR	BIT(1)
#define JL1xxx_WOL_RECEIVE	BIT(0)
#define ADDR8_HIGH_TO_LOW(n)	((n >> 4) | (n << 4))

#define JL2XXX_RGMII_CTRL_PAGE	0x00ab
#define JL2XXX_RGMII_TX_RSTN	BIT(14)
#define JL2XXX_RGMII_ERR_STAS	BIT(3)
#define JL2XXX_RGMII_TX_EN	BIT(1)

#define JL2XXX_RGMII_STUS_PAGE	0x00a6
#define JL2XXX_RGMII_STUS_REG	0x0012

#define JL2XXX_BASIC_PAGE	0x0000
#define JL2XXX_BMCR_REG		0x0000
#define JL2XXX_SOFT_RESET	BIT(15)
#define JL2XXX_SPEED_LSB	BIT(13)
#define JL2XXX_AUTONEG_EN	BIT(12)
#define JL2XXX_SPEED_MSB	BIT(6)
#define JL2XXX_DSFT_CTRL_REG	17
#define JL2XXX_DSFT_EN		BIT(12)
#define JL2XXX_DSFT_SMART_EN	BIT(13)
#define JL2XXX_DSFT_TWO_WIRE_EN	BIT(14)
#define JL2XXX_DSFT_STL_MASK	0x03e0
#define JL2XXX_DSFT_STL_CNT(n)	((n << 5) & JL2XXX_DSFT_STL_MASK)
#define JL2XXX_DSFT_AN_MASK	0x001f
#define JL2XXX_DSFT_CNT_MAX	32
#define JL2XXX_PHY_INFO_REG	29
#define JL2XXX_PATCH_MASK	0xffff
#define JL2XXX_SW_MASK		0xffff

#define JL2XXX_PATCH		0x00ad
#define JL2XXX_PATCH_REG	0x0010

#define JLSEMI_PHY_PAGE		0x001f
#define JL2XXX_WOL_CTL_PAGE	0x0012
#define JL2XXX_WOL_CTL_REG	0x0015
#define JL2XXX_WOL_STAS_PAGE	0x1200
#define JL2XXX_WOL_STAS_REG	0x0010
#define JL2XXX_MAC_ADDR2_REG	0x0011
#define JL2XXX_MAC_ADDR1_REG	0x0012
#define JL2XXX_MAC_ADDR0_REG	0x0013
#define JL2XXX_WOL_EVENT	BIT(1)
#define JL2XXX_WOL_POLARITY	BIT(14)
#define JL2XXX_WOL_EN		BIT(6)
#define JL2XXX_WOL_CTL_EN	BIT(15)

#define JL2XXX_PAGE2626		2626
#define JL2XXX_INTR_CTL_REG	18
#define JL2XXX_INTR_LINK_CHANGE	BIT(4)
#define JL2XXX_INTR_AN_COMPLETE	BIT(3)
#define JL2XXX_INTR_AN_PAGE	BIT(2)
#define JL2XXX_INTR_AN_ERR	BIT(0)

#define JL2XXX_PAGE2627		2627
#define JL2XXX_INTR_STATUS_REG	29
#define JL2XXX_CLK_CTRL_REG	19
#define JL2XXX_CLK_OUT_PIN	BIT(0)
#define JL2XXX_CLK_SSC_EN	BIT(3)
#define JL2XXX_CLK_125M_OUT	BIT(11)
#define JL2XXXX_CLK_SRC		BIT(12)

#define JL2XXX_PAGE158		158
#define JL2XXX_INTR_PIN_REG	16
#define JL2XXX_INTR_PIN_EN	BIT(14)

#define JL2XXX_PAGE160		160
#define JL2XXX_PIN_EN_REG	21
#define JL2XXX_PIN_OUTPUT	BIT(11)

#define JL2XXX_PAGE3336		3336
#define JL2XXX_RGMII_CTRL_REG	17

#define LED_PERIOD_MASK		0xff00
#define LEDPERIOD(n)		(n << 8) & LED_PERIOD_MASK
#define LED_ON_MASK		0x00ff
#define LEDON(n)		(n << 0) & LED_ON_MASK

/*************************************************************************/
struct jl_hw_stat {
	const char *string;
	u8 reg;
	u16 page;
	u16 mask;
};

static const struct jl_hw_stat jl2xxx_hw_stats[] = {
	{
		.string	= "phy_patch_version",
		.reg	= JL2XXX_PATCH_REG,
		.page	= JL2XXX_PATCH,
		.mask	= JL2XXX_PATCH_MASK,
	}, {
		.string	= "phy_software_version",
		.reg	= JL2XXX_PHY_INFO_REG,
		.page	= JL2XXX_BASIC_PAGE,
		.mask	= JL2XXX_SW_MASK,
	},
};

enum jl_static_op_mode {
	STATIC_NONE		= 0,
	STATIC_C_MACRO		= 1,
	STATIC_DEVICE_TREE	= 2,
};

enum jl_dynamic_op_mode {
	DYNAMIC_NONE		= 0,
	DYNAMIC_ETHTOOL		= 1,
};

struct jl_config_mode {
	enum jl_static_op_mode static_op;
	enum jl_dynamic_op_mode dynamic_op;
};

struct jl_led_ctrl {
	u16 enable;			/* LED control enable */
	u16 mode;			/* LED work mode */
	u16 global_period;		/* LED global twinkle period */
	u16 global_on;			/* LED global twinkle hold on time */
	u16 gpio_output;		/* LED is used as gpio output */
	u16 polarity;			/* LED polarity */
	struct jl_config_mode op;	/* LED config operation mode */
};

struct jl_fld_ctrl {
	u16 enable;			/* Fast link down control enable */
	u8 delay;			/* Fast link down time */
	struct jl_config_mode op;	/* Fast link down config operation mode */
};

struct jl_wol_ctrl {
	u16 enable;
	struct jl_config_mode op;
};

struct jl_intr_ctrl {
	u16 enable;
	struct jl_config_mode op;
};

struct jl_downshift_ctrl {
	u16 enable;
	u8 count;
	struct jl_config_mode op;
};

struct jl_rgmii_ctrl {
	u16 enable;
	u16 rx_delay;
	u16 tx_delay;
	struct jl_config_mode op;
};

struct jl_patch_ctrl {
	u16 enable;
	struct jl_config_mode op;
};

struct jl_clk_ctrl {
	u16 enable;
	struct jl_config_mode op;
};


struct jl1xxx_priv {
	struct jl_led_ctrl led;
	struct jl_wol_ctrl wol;
	struct jl_intr_ctrl intr;
	bool static_inited;
};

struct jl2xxx_priv {
	struct jl_led_ctrl led;
	struct jl_fld_ctrl fld;
	struct jl_wol_ctrl wol;
	struct jl_intr_ctrl intr;
	struct jl_downshift_ctrl downshift;
	struct jl_rgmii_ctrl rgmii;
	struct jl_patch_ctrl patch;
	struct jl_clk_ctrl clk;
	const struct jl_hw_stat *hw_stats;
	bool static_inited;
	int nstats;
	u64 *stats;
};

/* macros to simplify debug checking */
#define JLSEMI_PHY_MSG(msg,args...) printk(msg, ## args)

/************************* Configuration section *************************/


/************************* JLSemi iteration code *************************/
int jl2xxx_downshift_dynamic_op_get(struct phy_device *phydev, u8 *data);

int jl2xxx_downshift_dynamic_op_set(struct phy_device *phydev, u8 cnt);

int jlsemi_read_paged(struct phy_device *phydev, int page, u32 regnum);

int jl2xxx_intr_ack_event(struct phy_device *phydev);

int jl2xxx_intr_static_op_set(struct phy_device *phydev);

int jl1xxx_intr_ack_event(struct phy_device *phydev);

int jl1xxx_intr_static_op_set(struct phy_device *phydev);

int jl2xxx_wol_dynamic_op_get(struct phy_device *phydev);

int jl2xxx_wol_dynamic_op_set(struct phy_device *phydev);

int jl1xxx_wol_dynamic_op_get(struct phy_device *phydev);

int jl1xxx_wol_dynamic_op_set(struct phy_device *phydev);

int jl2xxx_fld_dynamic_op_get(struct phy_device *phydev, u8 *msecs);

int jl2xxx_fld_dynamic_op_set(struct phy_device *phydev, const u8 *msecs);

int jl1xxx_operation_mode_select(struct phy_device *phydev);

int jl1xxx_operation_args_get(struct phy_device *phydev);

int jl1xxx_static_op_init(struct phy_device *phydev);

int jl2xxx_operation_mode_select(struct phy_device *phydev);

int jl2xxx_operation_args_get(struct phy_device *phydev);

int jl2xxx_static_op_init(struct phy_device *phydev);

int jlsemi_soft_reset(struct phy_device *phydev);

int jlsemi_aneg_done(struct phy_device *phydev);

int jl2xxx_pre_init(struct phy_device *phydev);

/********************** Convenience function for phy **********************/

/* Notice: You should change page 0 when you When you call it after*/
int jlsemi_write_page(struct phy_device *phydev, int page);

int jlsemi_read_page(struct phy_device *phydev);

int jlsemi_modify_paged_reg(struct phy_device *phydev,
			    int page, u32 regnum,
			    u16 mask, u16 set);

int jlsemi_set_bits(struct phy_device *phydev,
		    int page, u32 regnum, u16 val);

int jlsemi_clear_bits(struct phy_device *phydev,
		      int page, u32 regnum, u16 val);

int jlsemi_fetch_bit(struct phy_device *phydev,
		     int page, u32 regnum, u16 val);

int jlsemi_drivers_register(struct phy_driver *phydrvs, int size);

void jlsemi_drivers_unregister(struct phy_driver *phydrvs, int size);

/**
 * module_jlsemi_driver() - Helper macro for registering PHY drivers
 * @__phy_drivers: array of PHY drivers to register
 *
 * Helper macro for PHY drivers which do not do anything special in module
 * init/exit. Each module may only use this macro once, and calling it
 * replaces module_init() and module_exit().
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))

#define jlsemi_module_driver(__phy_drivers, __count)			\
static int __init phy_module_init(void)					\
{									\
	return jlsemi_drivers_register(__phy_drivers, __count);		\
}									\
module_init(phy_module_init);						\
static void __exit phy_module_exit(void)				\
{									\
	jlsemi_drivers_unregister(__phy_drivers, __count);		\
}									\
module_exit(phy_module_exit)

#define module_jlsemi_driver(__phy_drivers)				\
	jlsemi_module_driver(__phy_drivers, ARRAY_SIZE(__phy_drivers))

#else

#define module_jlsemi_driver(__phy_drivers)				\
	module_phy_driver(__phy_drivers)
#endif

#endif /* _JLSEMI_CORE_H */


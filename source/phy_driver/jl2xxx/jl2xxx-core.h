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

#define JL2XXX_PHY_ID		0x937c4032
#define JLSEMI_PHY_ID_MASK	0xfffffff0

#define JL2XXX_RGMII_CTRL_PAGE	0x00ab
#define JL2XXX_RGMII_CTRL_REG	0x0011
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

#define JL2XXX_DIG_PAGE		0x00c9
#define JL2XXX_DIG_REG		0x0011
#define JL2XXX_CLK_10M_EN	BIT(15)
#define JL2XXX_DAC_MSB		BIT(1)
#define JL2XXX_DAC_LSB		BIT(0)

#define JL2XXX_PHY_PAGE		0x001f
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


/************************* Configuration section *************************/

#define JLSEMI_WOL_EN		1


/************************* JLSemi iteration code *************************/
struct jl2xx1_priv {
	u16 sw_info;
};

int jl2xxx_pre_init(struct phy_device *phydev);

int jl2xxx_dis_rgmii_tx(struct phy_device *phydev);

int jl2xxx_check_rgmii(struct phy_device *phydev);

int jl2xxx_config_suspend(struct phy_device *phydev);

int jl2xxx_config_resume(struct phy_device *phydev);

int jl2xxx_clear_wol_event(struct phy_device *phydev);

int jl2xxx_store_mac_addr(struct phy_device *phydev);

int jl2xxx_software_version(struct phy_device *phydev);

int jl2xxx_config_phy_info(struct phy_device *phydev,
			   struct jl2xx1_priv *jl2xx1);

int jl2xxx_enable_wol(struct phy_device *phydev, bool enable);

int jl2xxx_setup_wol_high_polarity(struct phy_device *phydev, bool high);

/********************** Convenience function for phy **********************/

/* Notice: You should change page 0 when you When you call it after*/
int jl2xxx_write_page(struct phy_device *phydev, int page);

int jl2xxx_read_page(struct phy_device *phydev);

int jl2xxx_modify_paged_reg(struct phy_device *phydev,
			    int page, u32 regnum,
			    u16 mask, u16 set);

int jl2xxx_set_bits(struct phy_device *phydev,
		    int page, u32 regnum, u16 val);

int jl2xxx_clear_bits(struct phy_device *phydev,
		      int page, u32 regnum, u16 val);

int jl2xxx_get_bit(struct phy_device *phydev,
		   int page, u32 regnum, u16 val);

int jl2xxx_drivers_register(struct phy_driver *phydrvs, int size);

void jl2xxx_drivers_unregister(struct phy_driver *phydrvs, int size);

/**
 * module_jl2xxx_driver() - Helper macro for registering PHY drivers
 * @__phy_drivers: array of PHY drivers to register
 *
 * Helper macro for PHY drivers which do not do anything special in module
 * init/exit. Each module may only use this macro once, and calling it
 * replaces module_init() and module_exit().
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))

#define jl2xxx_module_driver(__phy_drivers, __count)			\
static int __init phy_module_init(void)					\
{									\
	return jl2xxx_drivers_register(__phy_drivers, __count);		\
}									\
module_init(phy_module_init);						\
static void __exit phy_module_exit(void)				\
{									\
	jl2xxx_drivers_unregister(__phy_drivers, __count);		\
}									\
module_exit(phy_module_exit)

#define module_jl2xxx_driver(__phy_drivers)				\
	jl2xxx_module_driver(__phy_drivers, ARRAY_SIZE(__phy_drivers))

#else

#define module_jl2xxx_driver(__phy_drivers)				\
	module_phy_driver(__phy_drivers)
#endif

#endif /* _JLSEMI_CORE_H */


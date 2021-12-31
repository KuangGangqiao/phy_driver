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
#ifndef _JLSEMIAUTO_CORE_H
#define _JLSEMIAUTO_CORE_H

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/mii.h>
#include <linux/delay.h>
#include <linux/phy.h>

#define JL3XXX_UNKNOWN		0x0000
#define JL3XXX_100BASE_T1	0x0001
#define JL3XXX_1000BASE_T1	0x0002

#define JL3XXX_PHY_ID		0x937c4032

#define JL3XXX_PMA_CTL_ADDR	0x0001
#define JL3XXX_PMA_CTL		0x0834
#define JL3XXX_MASTER_MODE	BIT(14)
#define JL3XXX_PMA_BASE		0x0001
#define JL3XXX_PMA_RESET	BIT(15)
#define JL3XXX_PHYSID1		0x0002
#define JL3XXX_PHYSID2		0x0003

#define JL3XXX_PCS_CTL_ADDR	0x0003
#define JL3XXX_PCS_CTL		0x0000
#define JL3XXX_PCS_SPEED_LSB	BIT(13)
#define JL3XXX_PCS_SPEED_MSB	BIT(6)

#define JL3XXX_IEEE_CTL_ADDR	0x001f
#define JL3XXX_IEEE_CTL		0x0471
#define JL3XXX_GLOBAL_LINK	BIT(0)
#define JL3XXX_RGMII_CFG	0x3400
#define JL3XXX_RGMII_TX_DLY	BIT(4)

enum jl3xxx_mode {
	PHY_MODE_UNKNOWN,
	PHY_MODE_MASTER,
	PHY_MODE_SLAVE,
	PHY_MODE_AUTONEGO,
};

enum jl3xxx_speed {
	PHY_SPEED_UNKNOWN,
	PHY_SPEED_100M,
	PHY_SPEED_1000M,
};

struct jl3xxx_priv {
	enum jl3xxx_mode device_mode;
	enum jl3xxx_speed device_speed;
};

static const char *const jl3xxx_modes[] = {
	[PHY_MODE_MASTER]		= "master",
	[PHY_MODE_SLAVE]		= "slave",
	[PHY_MODE_AUTONEGO]		= "autonego",
};

static const char *const speed_names[] = {
	[PHY_SPEED_100M]	= "100M-speed",
	[PHY_SPEED_1000M]	= "1000M-speed",
};

enum jl3xxx_mode get_device_mode(struct device *dev);

enum jl3xxx_speed get_device_speed(struct device *dev);

int jl3xxx_mmd_modify(struct phy_device *phydev,
		      int devad, u16 reg, u16 mask, u16 bits);

int jl3xxx_fetch_mmd_bit(struct phy_device *phydev,
			 int devad, u16 reg, u16 bit);

int jl3xxx_set_mmd_bits(struct phy_device *phydev,
			int devad, u16 reg, u16 bits);

int jl3xxx_clear_mmd_bits(struct phy_device *phydev,
			  int devad, u16 reg, u16 bits);

int jl3xxx_config_phy_status(struct phy_device *phydev,
			     struct jl3xxx_priv *priv);

int jl3xxx_check_link(struct phy_device *phydev);

int jl3xxx_set_mode(struct phy_device *phydev, int mode);

int jl3xxx_set_speed(struct phy_device *phydev, int speed);

int jl3xxx_get_speed(struct phy_device *phydev);

int jl3xxx_set_tx_dly(struct phy_device *phydev);

#endif

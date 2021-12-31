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
#include "jl3xxx-core.h"
#include <linux/phy.h>
#include <linux/of.h>

enum jl3xxx_mode get_device_mode(struct device *dev)
{
	const char *device_mode;
	int ret;

	ret = device_property_read_string(dev, "dr_mode", &device_mode);
	if (ret < 0)
		return PHY_MODE_UNKNOWN;

	ret = match_string(jl3xxx_modes, ARRAY_SIZE(jl3xxx_modes), device_mode);
	if (ret < 0)
		return PHY_MODE_UNKNOWN;

	return ret;
}
EXPORT_SYMBOL_GPL(get_device_mode);

enum jl3xxx_speed get_device_speed(struct device *dev)
{
	const char *device_speed;
	int ret;

	ret = device_property_read_string(dev, "maximum-speed", &device_speed);
	if (ret < 0)
		return PHY_SPEED_UNKNOWN;

	ret = match_string(speed_names, ARRAY_SIZE(speed_names), device_speed);
	if (ret < 0)
		return PHY_SPEED_UNKNOWN;

	return ret;
}
EXPORT_SYMBOL_GPL(get_device_speed);

int jl3xxx_mmd_modify(struct phy_device *phydev,
		      int devad,u16 reg, u16 mask, u16 bits)
{
	int old, val, ret;

	old = phy_read_mmd(phydev, devad, reg);
	if (old < 0)
		return old;

	val = (old & ~mask) | (bits & mask);
	if (val == old)
		return 0;

	ret = phy_write_mmd(phydev, devad, reg, val);

	return ret < 0 ? ret : 1;
}
EXPORT_SYMBOL_GPL(jl3xxx_mmd_modify);

int jl3xxx_fetch_mmd_bit(struct phy_device *phydev,
			 int devad, u16 reg, u16 bit)
{
	int val;

	val = phy_read_mmd(phydev, devad, reg);
	if (val < 0)
		return val;

	return ((val & bit) == bit) ? 1 : 0;
}
EXPORT_SYMBOL_GPL(jl3xxx_fetch_mmd_bit);

int jl3xxx_set_mmd_bits(struct phy_device *phydev,
			int devad, u16 reg, u16 bits)
{
	return jl3xxx_mmd_modify(phydev, devad, reg, 0, bits);
}
EXPORT_SYMBOL_GPL(jl3xxx_set_mmd_bits);

int jl3xxx_clear_mmd_bits(struct phy_device *phydev,
			  int devad, u16 reg, u16 bits)
{
	return jl3xxx_mmd_modify(phydev, devad, reg, bits, 0);
}
EXPORT_SYMBOL_GPL(jl3xxx_clear_mmd_bits);

int jl3xxx_check_link(struct phy_device *phydev)
{
	return jl3xxx_fetch_mmd_bit(phydev, JL3XXX_IEEE_CTL_ADDR,
				    JL3XXX_IEEE_CTL, JL3XXX_GLOBAL_LINK);
}
EXPORT_SYMBOL_GPL(jl3xxx_check_link);

int jl3xxx_set_mode(struct phy_device *phydev, int mode)
{
	if (mode == PHY_MODE_MASTER)
		jl3xxx_set_mmd_bits(phydev, JL3XXX_PMA_CTL_ADDR,
				    JL3XXX_PMA_CTL, JL3XXX_MASTER_MODE);
	else if (mode == PHY_MODE_SLAVE)
		jl3xxx_clear_mmd_bits(phydev, JL3XXX_PMA_CTL_ADDR,
				      JL3XXX_PMA_CTL, JL3XXX_MASTER_MODE);
	else
		dev_warn(&phydev->mdio.dev,
			 "Automatic negotiation is not supported at present");

	return 0;
}
EXPORT_SYMBOL_GPL(jl3xxx_set_mode);

int jl3xxx_set_speed(struct phy_device *phydev, int speed)
{
	if (speed == PHY_SPEED_1000M)
		jl3xxx_mmd_modify(phydev, JL3XXX_PCS_CTL_ADDR, JL3XXX_PCS_CTL,
				  JL3XXX_PCS_SPEED_LSB, JL3XXX_PCS_SPEED_MSB);
	else
		jl3xxx_mmd_modify(phydev, JL3XXX_PCS_CTL_ADDR, JL3XXX_PCS_CTL,
				  JL3XXX_PCS_SPEED_MSB, JL3XXX_PCS_SPEED_LSB);

	return 0;
}
EXPORT_SYMBOL_GPL(jl3xxx_set_speed);

int jl3xxx_get_speed(struct phy_device *phydev)
{
	int msb, lsb;

	msb = jl3xxx_fetch_mmd_bit(phydev, JL3XXX_PCS_CTL_ADDR,
				   JL3XXX_PCS_CTL, JL3XXX_PCS_SPEED_MSB);

	lsb = jl3xxx_fetch_mmd_bit(phydev, JL3XXX_PCS_CTL_ADDR,
				   JL3XXX_PCS_CTL, JL3XXX_PCS_SPEED_LSB);

	if (~lsb & msb)
		return JL3XXX_1000BASE_T1;
	else
		return JL3XXX_100BASE_T1;
}
EXPORT_SYMBOL_GPL(jl3xxx_get_speed);

int jl3xxx_set_tx_dly(struct phy_device *phydev)
{
	return jl3xxx_set_mmd_bits(phydev, JL3XXX_IEEE_CTL_ADDR,
				   JL3XXX_RGMII_CFG, JL3XXX_RGMII_TX_DLY);
}
EXPORT_SYMBOL_GPL(jl3xxx_set_tx_dly);

int jl3xxx_config_phy_status(struct phy_device *phydev,
			     struct jl3xxx_priv *priv)
{
	if (priv->device_mode == PHY_MODE_UNKNOWN)
		priv->device_mode = PHY_MODE_MASTER;

	if (priv->device_speed == PHY_SPEED_UNKNOWN)
		priv->device_speed = PHY_SPEED_1000M;

	return 0;
}
EXPORT_SYMBOL_GPL(jl3xxx_config_phy_status);

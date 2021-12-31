/*
 * Driver for JLSemi Automotive JL3XXX Ethernet PHYs
 *
 * drivers/net/phy/jl3xxx.c
 *
 * Driver for JLSemi PHYs
 *
 * Author: Gangqiao Kuang <gqkuang@jlsemi.com>
 *
 * Copyright (c) 2021 JingLue Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "jl3xxx-core.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/mii.h>
#include <linux/phy.h>

static int jl3xxx_soft_reset(struct phy_device *phydev)
{
	/* no soft reset */
	return 0;
}

static int jl3xxx_config_init(struct phy_device *phydev)
{
	struct jl3xxx_priv *priv = phydev->priv;
	int err;
#if 0
	phydev->supported = SUPPORTED_100baseT_Full |
			    SUPPORTED_1000baseT_Full;
	phydev->advertising = SUPPORTED_100baseT_Full |
			      SUPPORTED_1000baseT_Full;
#endif
	err = jl3xxx_set_speed(phydev, priv->device_speed);
	if (err < 0)
		return err;

	err = jl3xxx_set_mode(phydev, priv->device_mode);
	if (err < 0)
		return err;

	err = jl3xxx_set_mmd_bits(phydev, JL3XXX_PMA_CTL_ADDR,
				  JL3XXX_PMA_BASE, JL3XXX_PMA_RESET);
	if (err < 0)
		return err;

	/* Wait soft reset to complete */
	msleep(50);

	return 0;
}

static int jl3xxx_config_aneg(struct phy_device *phydev)
{
	/* Automatic negotiation is not supported at present */
	return 0;
}

static int jl3xxx_aneg_done(struct phy_device *phydev)
{
	/* Automatic negotiation is not supported at present */
	return 1;
}

static int jl3xxx_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct jl3xxx_priv *priv;
	u32 id;

	if (!phydev->is_c45)
		return -ENOTSUPP;

	priv = devm_kzalloc(&phydev->mdio.dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->device_mode = get_device_mode(dev);
	priv->device_speed = get_device_speed(dev);
	phydev->priv = priv;

	dev_dbg(&phydev->mdio.dev, "device_mode: %d device_speed: %d\n",
		priv->device_mode, priv->device_speed);

	/* If the device tree is not initialized, we will initialize phy */
	jl3xxx_config_phy_status(phydev, priv);

	id = phy_read_mmd(phydev, JL3XXX_PMA_CTL_ADDR, JL3XXX_PHYSID1) << 16 |
	     phy_read_mmd(phydev, JL3XXX_PMA_CTL_ADDR, JL3XXX_PHYSID2);

	dev_info(&phydev->mdio.dev, "phy_id: 0x%x\n", id);

	return 0;
}

static int jl3xxx_read_status(struct phy_device *phydev)
{
	int speed;

	speed = jl3xxx_get_speed(phydev);

	if (speed == JL3XXX_1000BASE_T1)
		phydev->speed = SPEED_1000;
	else if (speed == JL3XXX_100BASE_T1)
		phydev->speed = SPEED_100;
	else
		phydev->speed = SPEED_UNKNOWN;

	if (jl3xxx_check_link(phydev))
		phydev->link = 1;
	else
		phydev->link = 0;

	phydev->duplex = DUPLEX_FULL;
	phydev->pause = 0;
	phydev->asym_pause = 0;

	if (phydev->link)
		jl3xxx_set_tx_dly(phydev);

	return 0;
}

static struct phy_driver jl3xxx_driver[] = {
{
	.phy_id		= JL3XXX_PHY_ID,
	.name		= "jl3xxx",
	.phy_id_mask	= 0xfffffff0,
	.features	= PHY_GBIT_FEATURES,
	.flags		= PHY_POLL,
	.soft_reset	= jl3xxx_soft_reset,
	.probe		= jl3xxx_probe,
	.config_init	= jl3xxx_config_init,
	.config_aneg	= jl3xxx_config_aneg,
	.aneg_done	= jl3xxx_aneg_done,
	.read_status	= jl3xxx_read_status,
} };

module_phy_driver(jl3xxx_driver);

static struct mdio_device_id __maybe_unused jl3xxx_tbl[] = {
	{ JL3XXX_PHY_ID, 0xfffffff0 },
	{ },
};

MODULE_DEVICE_TABLE(mdio, jl3xxx_tbl);

MODULE_DESCRIPTION("JLSemi Automotive JL3XXX PHY driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gangqiao Kuang");

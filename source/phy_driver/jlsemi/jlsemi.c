/*
 * drivers/net/phy/jlsemi.c
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
#include "jlsemi-core.h"
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/netdevice.h>

MODULE_DESCRIPTION("JLSemi PHY driver");
MODULE_AUTHOR("Gangqiao Kuang");
MODULE_LICENSE("GPL");

static int jl1xxx_probe(struct phy_device *phydev)
{
	struct jl1xxx_priv *jl1xxx;
	int err;

	jl1xxx = kzalloc(sizeof(*jl1xxx), GFP_KERNEL);
	if (!jl1xxx)
		return -ENOMEM;

	phydev->priv = jl1xxx;

	/* Select operation mode */
	jl1xxx_operation_mode_select(phydev);

	err = jl1xxx_operation_get(phydev);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_config_init(struct phy_device *phydev)
{
	int ret;

	ret = jl1xxx_operation_init(phydev);
	if (ret < 0)
		return ret;

	return 0;
}

static int jl1xxx_ack_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int jl1xxx_config_intr(struct phy_device *phydev)
{
	return 0;
}

static int jl1xxx_read_status(struct phy_device *phydev)
{
	return genphy_read_status(phydev);
}

static void jl1xxx_remove(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	if (priv)
		kfree(priv);
}

static int jl1xxx_suspend(struct phy_device *phydev)
{
	return genphy_suspend(phydev);
}

static int jl1xxx_resume(struct phy_device *phydev)
{
	return genphy_resume(phydev);
}

static int jl2xxx_probe(struct phy_device *phydev)
{
	struct jl2xxx_priv *jl2xxx;
	int err;

	jl2xxx = kzalloc(sizeof(*jl2xxx), GFP_KERNEL);
	if (!jl2xxx)
		return -ENOMEM;

	err = jl2xxx_config_phy_info(phydev, jl2xxx);
	if (err < 0)
		return err;

	phydev->priv = jl2xxx;

	err = jl2xxx_pre_init(phydev);
	if (err < 0)
		return err;

	/* Select operation mode */
	jl2xxx_operation_mode_select(phydev);

	err = jl2xxx_operation_get(phydev);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_config_init(struct phy_device *phydev)
{
	int ret;

	ret = jl2xxx_operation_init(phydev);
	if (ret < 0)
		return ret;

	return 0;
}

static int jl2xxx_ack_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int jl2xxx_config_intr(struct phy_device *phydev)
{
	return 0;
}

static int jl2xxx_read_status(struct phy_device *phydev)
{
	return genphy_read_status(phydev);
}

static int jl2xxx_config_aneg(struct phy_device *phydev)
{
	return genphy_config_aneg(phydev);
}

static int jl2xxx_suspend(struct phy_device *phydev)
{
	return genphy_suspend(phydev);
}

static int jl2xxx_resume(struct phy_device *phydev)
{
	return genphy_resume(phydev);
}

static void jl2xxx_get_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	int wol_en;

	wol->supported = WAKE_MAGIC;
	wol->wolopts = 0;

	wol_en = jlsemi_fetch_bit(phydev, JL2XXX_WOL_CTL_PAGE,
				  JL2XXX_WOL_CTL_REG, JL2XXX_WOL_EN);

	if (wol_en)
		wol->wolopts |= WAKE_MAGIC;
}

static int jl2xxx_set_wol(struct phy_device *phydev,
			  struct ethtool_wolinfo *wol)
{
	int err;

	if (wol->wolopts & WAKE_MAGIC) {
		err = jl2xxx_enable_wol(phydev, true);
		if (err < 0)
			return err;

		err = jl2xxx_clear_wol_event(phydev);
		if (err < 0)
			return err;

		err = jl2xxx_setup_wol_high_polarity(phydev, true);
		if (err < 0)
			return err;

		err = jl2xxx_store_mac_addr(phydev);
		if (err < 0)
			return err;
	} else {
		err = jl2xxx_enable_wol(phydev, false);
		if (err < 0)
			return err;

		err = jl2xxx_setup_wol_high_polarity(phydev, true);
		if (err < 0)
			return err;

		err = jl2xxx_clear_wol_event(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

static void jl2xxx_remove(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	if (priv)
		kfree(priv);
}

static struct phy_driver jlsemi_drivers[] = {
	{
		.phy_id		= JL1XXX_PHY_ID,
		.phy_id_mask    = JLSEMI_PHY_ID_MASK,
		.name           = "JL1xxx Fast Ethernet",
		/* PHY_BASIC_FEATURES */
		.features	= PHY_BASIC_FEATURES,
		.probe		= jl1xxx_probe,
		.aneg_done	= jlsemi_aneg_done,
		.config_intr	= jl1xxx_config_intr,
		.soft_reset	= jlsemi_soft_reset,
		.read_status	= jl1xxx_read_status,
		.config_init    = jl1xxx_config_init,
		.remove		= jl1xxx_remove,
		.suspend        = jl1xxx_suspend,
		.resume         = jl1xxx_resume,
	},
	{
		.phy_id         = JL2XXX_PHY_ID,
		.phy_id_mask    = JLSEMI_PHY_ID_MASK,
		.name           = "JL2xxx Gigabit Ethernet",
		/* PHY_BASIC_FEATURES */
		.features	= PHY_GBIT_FEATURES,
		.probe		= jl2xxx_probe,
		.config_intr	= jl2xxx_config_intr,
		.soft_reset	= jlsemi_soft_reset,
		.aneg_done	= jlsemi_aneg_done,
		.read_status	= jl2xxx_read_status,
		.config_init    = jl2xxx_config_init,
		.config_aneg    = jl2xxx_config_aneg,
		.suspend        = jl2xxx_suspend,
		.resume         = jl2xxx_resume,
		.remove		= jl2xxx_remove,
		.get_wol	= jl2xxx_get_wol,
		.set_wol	= jl2xxx_set_wol,
	},
};

module_jlsemi_driver(jlsemi_drivers);

static struct mdio_device_id __maybe_unused jlsemi_tbl[] = {
        { JL1XXX_PHY_ID, JLSEMI_PHY_ID_MASK },
        { JL2XXX_PHY_ID, JLSEMI_PHY_ID_MASK },
	{ }
};

MODULE_DEVICE_TABLE(mdio, jlsemi_tbl);

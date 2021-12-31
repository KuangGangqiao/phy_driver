/*
 * drivers/net/phy/jl2xx1.c
 *
 * Driver for JLSemi PHYs
 *
 * Author: Gangqiao Kuang <gqkuang@jl2xxx.com>
 *
 * Copyright (c) 2021 JingLue Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "jl2xxx-core.h"
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/netdevice.h>

static int jl2xxx_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct jl2xx1_priv *jl2xx1;
	int err;

	jl2xx1 = devm_kzalloc(dev, sizeof(*jl2xx1), GFP_KERNEL);
	if (!jl2xx1)
		return -ENOMEM;

	err = jl2xxx_config_phy_info(phydev, jl2xx1);
	if (err < 0)
		return err;

	phydev->priv = jl2xx1;

	return 0;
}

static int jl2xxx_config_aneg(struct phy_device *phydev)
{
	return genphy_config_aneg(phydev);
}

static int jl2xxx_config_init(struct phy_device *phydev)
{
	return 0;
}

static int jl2xxx_read_status(struct phy_device *phydev)
{
	return genphy_read_status(phydev);
}

static int jl2xxx_suspend(struct phy_device *phydev)
{
	return genphy_suspend(phydev);
}

static int jl2xxx_resume(struct phy_device *phydev)
{
	return genphy_resume(phydev);
}

#if JLSEMI_WOL_EN
static void jl2xxx_get_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	int wol_en;

	wol->supported = WAKE_MAGIC;
	wol->wolopts = 0;

	wol_en = jl2xxx_get_bit(phydev, JL2XXX_WOL_CTL_PAGE,
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
#endif

static void jl2xxx_remove(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct jl2xx1_priv *priv = phydev->priv;

	if (priv)
		devm_kfree(dev, priv);
}

static struct phy_driver jl2xxx_driver[] = {
{
        .phy_id         = JL2XXX_PHY_ID,
        .name           = "JL2xxx Gigabit Ethernet",
        .phy_id_mask    = JLSEMI_PHY_ID_MASK,

        /* PHY_BASIC_FEATURES */
	.features	= PHY_GBIT_FEATURES,
	.probe		= jl2xxx_probe,
	.read_status	= jl2xxx_read_status,
	.config_init    = jl2xxx_config_init,
        .config_aneg    = jl2xxx_config_aneg,
        .suspend        = jl2xxx_suspend,
        .resume         = jl2xxx_resume,
	.remove		= jl2xxx_remove,

	#if JLSEMI_WOL_EN
	.get_wol	= jl2xxx_get_wol,
	.set_wol	= jl2xxx_set_wol,
	#endif
}};

module_jl2xxx_driver(jl2xxx_driver);

static struct mdio_device_id __maybe_unused jl2xxx_tbl[] = {
        { JL2XXX_PHY_ID, JLSEMI_PHY_ID_MASK },
        { }
};

MODULE_DEVICE_TABLE(mdio, jl2xxx_tbl);

MODULE_DESCRIPTION("JLSemi PHY driver");
MODULE_AUTHOR("Gangqiao Kuang");
MODULE_LICENSE("GPL");

// SPDX-License-Identifier: GPL-2.0+
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

#if (JLSEMI_DEBUG_INFO)
static int jlsemi_phy_reg_print(struct phy_device *phydev)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(jl_phy); i++) {
		if (jl_phy[i].enable) {
			ret = jlsemi_read_paged(phydev, jl_phy[i].page,
						jl_phy[i].reg);
			JLSEMI_PHY_MSG("%s: 0x%x\n", jl_phy[i].string, ret);
			if (ret < 0)
				return ret;
		}
	}

	return 0;
}
#endif

static int jl1xxx_probe(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl1xxx_priv *jl1xxx = NULL;
	struct device *d = NULL;
	int err;

	jl1xxx = devm_kzalloc(dev, sizeof(*jl1xxx), GFP_KERNEL);
	if (!jl1xxx)
		return -ENOMEM;

	phydev->priv = jl1xxx;

	d = jlsemi_get_device(phydev);
#if (JLSEMI_KERNEL_DEVICE_TREE_USE)
	d->of_node = of_find_compatible_node(NULL, NULL,
					    "ethernet-phy-id937c.4020");
	if (!d->of_node)
		JLSEMI_PHY_MSG("%s: Find device node failed\n", __func__);
#endif
	/* Select operation mode */
	jl1xxx_operation_mode_select(phydev);

	err = jl1xxx_operation_args_get(phydev);
	if (err < 0)
		return err;

	if (jl1xxx->intr.enable & JL1XXX_INTR_STATIC_OP_EN ||
	    jl1xxx->intr.enable & JL1XXX_INTR_DYNAMIC_OP_EN)
		phydev->irq = JL1XXX_INTR_IRQ;

	jl1xxx->static_inited = false;

	return 0;
}

static int jl1xxx_config_init(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int ret;

	if (!priv->static_inited) {
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl1xxx_config_init_before:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		ret = jl1xxx_static_op_init(phydev);
		if (ret < 0)
			return ret;
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl1xxx_config_init_after:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		priv->static_inited = JLSEMI_PHY_NOT_REENTRANT;
	}

	return 0;
}

static int jl1xxx_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = jl1xxx_intr_ack_event(phydev);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_config_intr(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->intr.enable & JL1XXX_INTR_STATIC_OP_EN) {
		err = jl1xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;

		err = jl1xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

static int jl1xxx_read_status(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->intr.enable & JL1XXX_INTR_STATIC_OP_EN) {
		err = jl1xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;
	}

	return genphy_read_status(phydev);
}

static void jl1xxx_remove(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl1xxx_priv *priv = phydev->priv;

	if (priv)
		devm_kfree(dev, priv);
}

static void jl1xxx_get_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int wol_en;

	if (priv->wol.enable & JL1XXX_WOL_DYNAMIC_OP_EN) {
		wol->supported = WAKE_MAGIC;
		wol->wolopts = 0;

		wol_en = jl1xxx_wol_dynamic_op_get(phydev);

		if (!wol_en)
			wol->wolopts |= WAKE_MAGIC;
	}
}

static int jl1xxx_set_wol(struct phy_device *phydev,
			  struct ethtool_wolinfo *wol)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->wol.enable & JL1XXX_WOL_DYNAMIC_OP_EN) {
		if (wol->wolopts & WAKE_MAGIC) {
			err = jl1xxx_wol_dynamic_op_set(phydev);
			if (err < 0)
				return err;
		}
	}

	return 0;
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
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl2xxx_priv *jl2xxx = NULL;
	struct device *d = NULL;
	int err;

	jl2xxx = devm_kzalloc(dev, sizeof(*jl2xxx), GFP_KERNEL);
	if (!jl2xxx)
		return -ENOMEM;

	phydev->priv = jl2xxx;

	d = jlsemi_get_device(phydev);
#if (JLSEMI_KERNEL_DEVICE_TREE_USE)
	d->of_node = of_find_compatible_node(NULL, NULL,
					    "ethernet-phy-id937c.4030");
	if (!d->of_node)
		JLSEMI_PHY_MSG("%s: Find device node failed\n", __func__);
#endif
	/* Select operation mode */
	jl2xxx_operation_mode_select(phydev);

	err = jl2xxx_operation_args_get(phydev);
	if (err < 0)
		return err;

	if (jl2xxx->intr.enable & JL2XXX_INTR_STATIC_OP_EN ||
	    jl2xxx->intr.enable & JL2XXX_INTR_DYNAMIC_OP_EN)
		phydev->irq = JL2XXX_INTR_IRQ;

	jl2xxx->static_inited = false;
	jl2xxx->nstats = ARRAY_SIZE(jl2xxx_hw_stats);
	jl2xxx->hw_stats = jl2xxx_hw_stats;
	jl2xxx->stats = kcalloc(jl2xxx->nstats, sizeof(u64), GFP_KERNEL);
	if (!jl2xxx->stats)
		return -ENOMEM;

	err = jl2xxx_ptp_probe(phydev);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_config_init(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int ret;

	if (!priv->static_inited) {
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl2xxx_config_init_before:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		ret = jl2xxx_static_op_init(phydev);
		if (ret < 0)
			return ret;
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl2xxx_config_init_after:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		priv->static_inited = JLSEMI_PHY_NOT_REENTRANT;
	}

	return 0;
}

static int jl2xxx_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_intr_ack_event(phydev);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_config_intr(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->intr.enable & JL2XXX_INTR_STATIC_OP_EN) {
		err = jl2xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;

		err = jl2xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

static int jl2xxx_read_status(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	bool fiber_mode;
	int err;

	if (priv->intr.enable & JL2XXX_INTR_STATIC_OP_EN) {
		err = jl2xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;
	}

	fiber_mode = jl2xxx_read_fiber_status(phydev);
	if (fiber_mode)
		return 0;

	return genphy_read_status(phydev);
}

static int jl2xxx_config_aneg(struct phy_device *phydev)
{
	u16 phy_mode;
	int val;

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE18,
				JL2XXX_WORK_MODE_REG);
	phy_mode = val & JL2XXX_WORK_MODE_MASK;

	if (phydev->interface == PHY_INTERFACE_MODE_SGMII)
		return 0;

	if ((phydev->interface != PHY_INTERFACE_MODE_SGMII) &&
	    (phy_mode == JL2XXX_FIBER_RGMII_MODE))
		return jl2xxx_config_aneg_fiber(phydev);

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
	struct jl2xxx_priv *priv = phydev->priv;
	int wol_en;

	if (priv->wol.enable & JL2XXX_WOL_DYNAMIC_OP_EN) {
		wol->supported = WAKE_MAGIC;
		wol->wolopts = 0;

		wol_en = jl2xxx_wol_dynamic_op_get(phydev);

		if (wol_en)
			wol->wolopts |= WAKE_MAGIC;
	}
}

static int jl2xxx_set_wol(struct phy_device *phydev,
			  struct ethtool_wolinfo *wol)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->wol.enable & JL2XXX_WOL_DYNAMIC_OP_EN) {
		if (wol->wolopts & WAKE_MAGIC) {
			err = jl2xxx_wol_dynamic_op_set(phydev);
			if (err < 0)
				return err;
		}
	}

	return 0;
}

#if (JL2XXX_PHY_TUNABLE)
static int jl2xxx_get_tunable(struct phy_device *phydev,
			      struct ethtool_tunable *tuna, void *data)
{
	struct jl2xxx_priv *priv = phydev->priv;

	switch (tuna->id) {
	case ETHTOOL_PHY_FAST_LINK_DOWN:
		if (priv->fld.enable & JL2XXX_FLD_DYNAMIC_OP_EN)
			return jl2xxx_fld_dynamic_op_get(phydev, data);
		else
			return 0;
	case ETHTOOL_PHY_DOWNSHIFT:
		if (priv->downshift.enable & JL2XXX_DSFT_DYNAMIC_OP_EN)
			return jl2xxx_downshift_dynamic_op_get(phydev, data);
		else
			return 0;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int jl2xxx_set_tunable(struct phy_device *phydev,
			      struct ethtool_tunable *tuna, const void *data)
{
	struct jl2xxx_priv *priv = phydev->priv;

	switch (tuna->id) {
	case ETHTOOL_PHY_FAST_LINK_DOWN:
		if (priv->fld.enable & JL2XXX_FLD_DYNAMIC_OP_EN)
			return jl2xxx_fld_dynamic_op_set(phydev, data);
		else
			return 0;
	case ETHTOOL_PHY_DOWNSHIFT:
		if (priv->downshift.enable & JL2XXX_DSFT_DYNAMIC_OP_EN)
			return jl2xxx_downshift_dynamic_op_set(phydev,
							*(const u8 *)data);
		else
			return 0;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}
#endif

#if (JL2XXX_GET_STAT)
static u64 get_stat(struct phy_device *phydev, int i)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int val;

	val = jlsemi_read_paged(phydev, priv->hw_stats[i].page,
				priv->hw_stats[i].reg);
	if (val < 0)
		return U64_MAX;

	val = val & priv->hw_stats[i].mask;
	priv->stats[i] += val;

	return priv->stats[i];
}

static void jl2xxx_get_stats(struct phy_device *phydev,
			    struct ethtool_stats *stats, u64 *data)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int i;

	if (!priv)
		return;

	for (i = 0; i < priv->nstats; i++)
		data[i] = get_stat(phydev, i);
}
#endif

#if (JL2XXX_GET_STRING)
static void jl2xxx_get_strings(struct phy_device *phydev, u8 *data)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int i;

	if (!priv)
		return;

	for (i = 0; i < priv->nstats; i++)
		strlcpy(data + i * ETH_GSTRING_LEN,
			priv->hw_stats[i].string, ETH_GSTRING_LEN);
}
#endif

static int jl2xxx_ts_info(struct phy_device *dev, struct ethtool_ts_info *info)
{
	struct jl2xxx_priv *priv = dev->priv;

	info->so_timestamping =
		SOF_TIMESTAMPING_TX_HARDWARE |
		SOF_TIMESTAMPING_RX_HARDWARE |
		SOF_TIMESTAMPING_RAW_HARDWARE;
	info->phc_index = ptp_clock_index(priv->ptp_clock);
	info->tx_types =
		(1 << HWTSTAMP_TX_OFF) |
		(1 << HWTSTAMP_TX_ON) |
		(1 << HWTSTAMP_TX_ONESTEP_SYNC);
	info->rx_filters =
		(1 << HWTSTAMP_FILTER_NONE) |
		(1 << HWTSTAMP_FILTER_PTP_V1_L4_EVENT) |
		(1 << HWTSTAMP_FILTER_PTP_V2_L4_EVENT) |
		(1 << HWTSTAMP_FILTER_PTP_V2_L2_EVENT) |
		(1 << HWTSTAMP_FILTER_PTP_V2_EVENT);

	return 0;
}
static int jl2xxx_hwtstamp(struct phy_device *phydev, struct ifreq *ifr)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct hwtstamp_config cfg;
	int err;

	if (copy_from_user(&cfg, ifr->ifr_data, sizeof(cfg)))
		return -EFAULT;

	if (cfg.flags)
		return -EINVAL;

	if (cfg.tx_type < 0 || cfg.tx_type > HWTSTAMP_TX_ONESTEP_SYNC)
		return -ERANGE;

	priv->hwts_tx_en = cfg.tx_type;

	switch (cfg.rx_filter) {
	case HWTSTAMP_FILTER_NONE:
		priv->hwts_rx_en = 0;
		break;
	case HWTSTAMP_FILTER_PTP_V2_EVENT:
	case HWTSTAMP_FILTER_PTP_V2_SYNC:
	case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
		priv->hwts_rx_en = 1;
		break;
	default:
		return -ERANGE;
	}

	err = jl2xxx_ptp_config(phydev);
	if (err < 0)
		return -EINVAL;

	return copy_to_user(ifr->ifr_data, &cfg, sizeof(cfg)) ? -EFAULT : 0;
}

static bool jl2xxx_rxtstamp(struct phy_device *phydev,
			    struct sk_buff *skb, int type)
{
	struct jl2xxx_skb_info *skb_info =
		(struct jl2xxx_skb_info *)skb->cb;
	struct skb_shared_hwtstamps *shhwtstamps = NULL;
	struct jl2xxx_priv *priv = phydev->priv;
	struct list_head *this, *next;
	unsigned long flags;
	struct rxts *rxts;

	if (!priv->hwts_rx_en)
		return false;

	if (!(type & PTP_CLASS_V2))
		return false;

	spin_lock_irqsave(&priv->rx_lock, flags);
	prune_rx_ts(priv);
	list_for_each_safe(this, next, &priv->rxts) {
		rxts = list_entry(this, struct rxts, list);
		if (match(skb, type, rxts->seqid)) {
			shhwtstamps = skb_hwtstamps(skb);
			memset(shhwtstamps, 0, sizeof(*shhwtstamps));
			shhwtstamps->hwtstamp = ns_to_ktime(rxts->nsec);
			list_del_init(&rxts->list);
			list_add(&rxts->list, &priv->rxpool);
			break;
		}
	}
	spin_unlock_irqrestore(&priv->rx_lock, flags);

	if (!shhwtstamps) {
		skb_info->ptp_type = type;
		skb_info->tmo = jiffies + SKB_TIMESTAMP_TIMEOUT;
		skb_queue_tail(&priv->rx_queue, skb);
		schedule_delayed_work(&priv->rx_ts_work, 0);
	} else {
		netif_rx_ni(skb);
	}

	return true;
}

static void jl2xxx_txtstamp(struct phy_device *phydev,
			    struct sk_buff *skb, int type)
{
	struct jl2xxx_skb_info *skb_info =
		(struct jl2xxx_skb_info *)skb->cb;
	struct jl2xxx_priv *priv = phydev->priv;

	switch (priv->hwts_tx_en) {
	case HWTSTAMP_TX_ONESTEP_SYNC:
		if (is_sync(skb, type)) {
			kfree_skb(skb);
			return;
		}
		fallthrough;
	case HWTSTAMP_TX_ON:
		skb_info->ptp_type = type;
		skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
		skb_info->tmo = jiffies + SKB_TIMESTAMP_TIMEOUT;
		skb_queue_tail(&priv->tx_queue, skb);
		schedule_delayed_work(&priv->tx_ts_work, 0);
		break;
	case HWTSTAMP_TX_OFF:
	default:
		kfree_skb(skb);
		break;
	}

}

static void jl2xxx_remove(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl2xxx_priv *priv = phydev->priv;

	jl2xxx_ptp_remove(phydev);

	kfree(priv->stats);
	if (priv)
		devm_kfree(dev, priv);
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
		.get_wol	= jl1xxx_get_wol,
		.set_wol	= jl1xxx_set_wol,
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
		.ts_info	= jl2xxx_ts_info,
		.hwtstamp	= jl2xxx_hwtstamp,
		.rxtstamp	= jl2xxx_rxtstamp,
		.txtstamp	= jl2xxx_txtstamp,

#if (JL2XXX_PHY_TUNABLE)
		.get_tunable	= jl2xxx_get_tunable,
		.set_tunable	= jl2xxx_set_tunable,
#endif
#if (JL2XXX_GET_STAT)
		.get_stats	= jl2xxx_get_stats,
#endif
#if (JL2XXX_GET_STRING)
		.get_strings	= jl2xxx_get_strings,
#endif
	},
};

module_jlsemi_driver(jlsemi_drivers);

static struct mdio_device_id __maybe_unused jlsemi_tbl[] = {
	{JL1XXX_PHY_ID, JLSEMI_PHY_ID_MASK},
	{JL2XXX_PHY_ID, JLSEMI_PHY_ID_MASK},
	{ }
};

MODULE_DEVICE_TABLE(mdio, jlsemi_tbl);

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

#define DRIVER_VERSION		"1.2.11"
#define DRIVER_NAME_100M	"JL1xxx Fast Ethernet " DRIVER_VERSION
#define DRIVER_NAME_1000M	"JL2xxx Gigabit Ethernet " DRIVER_VERSION

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
	int err;

	jl1xxx = devm_kzalloc(dev, sizeof(*jl1xxx), GFP_KERNEL);
	if (!jl1xxx)
		return -ENOMEM;

	phydev->priv = jl1xxx;

#if (JLSEMI_KERNEL_DEVICE_TREE_USE)
	if (!dev->of_node)
		JLSEMI_PHY_MSG("%s: Find device node failed\n", __func__);
#endif
	err = jl1xxx_operation_args_get(phydev);
	if (err < 0)
		return err;

	if (jl1xxx->intr.enable & JL1XXX_INTR_STATIC_OP_EN)
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

#if (JLSEMI_PHY_WOL)
static void jl1xxx_get_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int wol_en;

	if (priv->wol.ethtool) {
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

	if (priv->wol.ethtool) {
		if (wol->wolopts & WAKE_MAGIC) {
			err = jl1xxx_wol_dynamic_op_set(phydev);
			if (err < 0)
				return err;
		}
	}

	return 0;
}
#endif

static int jl1xxx_suspend(struct phy_device *phydev)
{
	return genphy_suspend(phydev);
}

static int jl1xxx_resume(struct phy_device *phydev)
{
	return genphy_resume(phydev);
}

static int jl2xxx_ptp_enable(struct ptp_clock_info *ptp,
			     struct ptp_clock_request *rq, int on)
{
	return -EOPNOTSUPP;
}

#define ONE_SEC_TO_NSEC	1000000000
static int jl2xxx_phc_adjust(struct phy_device *phydev, s64 delta)
{
	u32 ns_gain;
	u32 s, ns;

	jl2xxx_ptp_get_global_time(phydev, &s, &ns);

	if ((delta + ns) > ONE_SEC_TO_NSEC)
		ns_gain = (delta + ns) - ONE_SEC_TO_NSEC;
	else if ((delta + ns) < 0)
		ns_gain = ONE_SEC_TO_NSEC + (delta + ns);
	else
		ns_gain = delta + ns;

	jl2xxx_ptp_set_global_time(phydev, 0, ns_gain);

	return 0;
}

static int jl2xxx_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
	struct jl2xxx_priv *priv =
	container_of(ptp, struct jl2xxx_priv, ptp_info);
	struct phy_device *phydev = priv->phydev;

	return jl2xxx_phc_adjust(phydev, delta);
}

static long jl2xxx_ppm_to_ppb(long ppm)
{
	/*
	 * The 'freq' field in the 'struct timex' is in parts per
	 * million, but with a 16 bit binary fractional field.
	 *
	 * We want to calculate
	 *
	 *    ppb = scaled_ppm * 1000 / 2^16
	 *
	 * which simplifies to
	 *
	 *    ppb = scaled_ppm * 125 / 2^13
	 */
	s64 ppb = 1 + ppm;

	ppb *= 125;
	ppb >>= 13;

	return (long) ppb;
}

static int jl2xxx_ptp_adjfine(struct ptp_clock_info *ptp, long scaled_ppm)
{
	struct jl2xxx_priv *priv =
		container_of(ptp, struct jl2xxx_priv, ptp_info);
	struct ptp_clock_info *ptp_info = &priv->ptp_info;
	bool neg_adj = false;
	long scaled_ppb;

	scaled_ppb = jl2xxx_ppm_to_ppb(scaled_ppm);
	if (scaled_ppb < 0)
		neg_adj = true;

	priv->ptp_work_neg_adj = neg_adj;
	priv->ptp_work_ppb = scaled_ppb;

	if ((priv->ptp_work_ppb * priv->last_ppb / ptp_info->max_adj) < 0)
		priv->safe_quit = true;

	priv->last_ppb = priv->ptp_work_ppb;

	schedule_delayed_work(&priv->state_queue, 175);

	return 0;
}

static int jl2xxx_ptp_gettime64(struct ptp_clock_info *ptp,
				struct timespec64 *ts)
{
	struct jl2xxx_priv *priv =
		container_of(ptp, struct jl2xxx_priv, ptp_info);
	struct phy_device *phydev = priv->phydev;
	u32 sec, ns;
	int ret;

	/* Update ptp hardware clock to system real time */
	ret = jl2xxx_ptp_get_global_time(phydev, &sec, &ns);
	if (ret < 0)
		return ret;

	ts->tv_sec = sec;
	ts->tv_nsec = ns;

	return ret;
}

static int jl2xxx_ptp_settime64(struct ptp_clock_info *ptp,
				const struct timespec64 *ts)
{
	struct jl2xxx_priv *priv =
		container_of(ptp, struct jl2xxx_priv, ptp_info);
	struct phy_device *phydev = priv->phydev;
	int ret;

	/* maybe need lock */
	ret = jl2xxx_ptp_set_global_time(phydev, ts->tv_sec, ts->tv_nsec);

	return 0;
}

static struct ptp_clock_info jl2xxx_ptp_info = {
	.owner		= THIS_MODULE,
	.name		= "jl2xxx ptp clock",
	.enable		= jl2xxx_ptp_enable,
	.adjfine	= jl2xxx_ptp_adjfine,
	.adjtime	= jl2xxx_ptp_adjtime,
	.gettime64	= jl2xxx_ptp_gettime64,
	.settime64	= jl2xxx_ptp_settime64,
	.max_adj	= 10 * MAX_AMT,
};

static int jl2xxx_get_rxts(struct jl2xxx_priv *priv)
{
	int err;

	err = jl2xxx_ptp_get_ts_arr_time(priv->phydev,
					&priv->arr_ts.sec,
					&priv->arr_ts.nsec);
	if (err < 0)
		return err;

	err = jl2xxx_ptp_get_ts_arr_seqid(priv->phydev,
					  &priv->arr_ts.seqid);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_get_txts(struct jl2xxx_priv *priv)
{
	int err;

	err = jl2xxx_ptp_get_ts_dep_time(priv->phydev, &priv->dep_ts.sec,
					 &priv->dep_ts.nsec);
	if (err < 0)
		return err;
	err = jl2xxx_ptp_get_ts_dep_seqid(priv->phydev, &priv->dep_ts.seqid);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_ptp_config(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	bool is_reset;
	int err;

	is_reset = jlsemi_fetch_bit(phydev, PAGE160, PTP_GLB_CFG0, PTP_RESET);
	if (phydev->link && !is_reset) {
		err = jl2xxx_ptp_init(phydev);
		if (err < 0)
			return err;
		err = jl2xxx_ptp_init_pps(phydev);
		if (err < 0)
			return err;
		priv->adj_state = PHC_STOP_ADJ;
		/*
		 * Due to link down need config again,
		 * we need init safe_quit to false
		 */
		priv->safe_quit = false;
		pr_info("%s: Ptp init success!", __func__);
	}

	return 0;
}

static int jl2xxx_phc_set_step(struct phy_device *phydev,
			       bool positive, long step)
{
	if (step < 0)
		step = -step;

	return jl2xxx_ptp_clock_step(phydev, positive, step);
}


static void jl2xxx_phy_to_rxts(struct jl2xxx_priv *priv, struct rxts *rxts)
{
	mutex_lock(&priv->ptp_lock);
	jl2xxx_get_rxts(priv);
	mutex_unlock(&priv->ptp_lock);

	rxts->sec = priv->arr_ts.sec;
	rxts->nsec = priv->arr_ts.nsec;
	rxts->seqid = priv->arr_ts.seqid;
	rxts->tmo = jiffies + SKB_TIMESTAMP_TIMEOUT;
}

static void prune_rx_ts(struct jl2xxx_priv *priv)
{
	struct list_head *this, *next;
	struct rxts *rxts;

	list_for_each_safe(this, next, &priv->rxts) {
		rxts = list_entry(this, struct rxts, list);
		/* rx timestamp timeout */
		if (time_after(jiffies, rxts->tmo)) {
			list_del_init(&rxts->list);
			list_add(&rxts->list, &priv->rxpool);
		}
	}
}

static int is_sync(struct sk_buff *skb, int type)
{
	u8 *data = skb->data, *msgtype;
	unsigned int offset = 0;

	if (type & PTP_CLASS_VLAN)
		offset += VLAN_HLEN;

	switch (type & PTP_CLASS_PMASK) {
	case PTP_CLASS_IPV4:
		offset += ETH_HLEN + IPV4_HLEN(data + offset) + UDP_HLEN;
		break;
	case PTP_CLASS_IPV6:
		offset += ETH_HLEN + IP6_HLEN + UDP_HLEN;
		break;
	case PTP_CLASS_L2:
		offset += ETH_HLEN;
		break;
	default:
		return 0;
	}

	if (type & PTP_CLASS_V1)
		offset += OFF_PTP_CONTROL;

	if (skb->len < offset + 1)
		return 0;

	msgtype = data + offset;

	return (*msgtype & 0xf) == 0;
}

static int match(struct sk_buff *skb, unsigned int type, u16 seqid)
{
	u8 *data = skb_mac_header(skb);
	unsigned int offset = 0;
	u16 *pseqid;

	/* Fix seqid = 0 */
	if (!seqid)
		return 1;

	if (type & PTP_CLASS_VLAN)
		offset += VLAN_HLEN;

	switch (type & PTP_CLASS_PMASK) {
	case PTP_CLASS_IPV4:
		offset += ETH_HLEN + IPV4_HLEN(data + offset) + UDP_HLEN;
		break;
	case PTP_CLASS_IPV6:
		offset += ETH_HLEN + IP6_HLEN + UDP_HLEN;
		break;
	case PTP_CLASS_L2:
		offset += ETH_HLEN;
		break;
	default:
		return 0;
	}
	/* Check sequence id */
	offset += OFF_PTP_SEQUENCE_ID;
	if (skb->len + ETH_HLEN < offset + sizeof(*pseqid))
		return 0;

	pseqid = (u16 *)(data + offset);

	if (seqid != ntohs(*pseqid))
		return 0;

	return 1;
}

static void rx_timestamp_work(struct work_struct *work)
{
	struct jl2xxx_priv *priv =
		container_of(work, struct jl2xxx_priv, rx_ts_work.work);
	struct rxts *rxts;
	struct skb_shared_hwtstamps *shhwtstamps = NULL;
	struct sk_buff *skb;
	unsigned long flags;

	spin_lock_irqsave(&priv->rx_lock, flags);

	prune_rx_ts(priv);

	if (list_empty(&priv->rxpool)) {
		pr_info("%s: Rx timestamp pool is empty\n", __func__);
		goto out;
	}
	rxts = list_first_entry(&priv->rxpool, struct rxts, list);
	list_del_init(&rxts->list);
	jl2xxx_phy_to_rxts(priv, rxts);

	spin_lock(&priv->rx_queue.lock);
	skb_queue_walk(&priv->rx_queue, skb) {
		struct jl2xxx_skb_info *skb_info;

		skb_info = (struct jl2xxx_skb_info *)skb->cb;
		if (match(skb, skb_info->ptp_type, rxts->seqid)) {
			__skb_unlink(skb, &priv->rx_queue);
			shhwtstamps = skb_hwtstamps(skb);
			memset(shhwtstamps, 0, sizeof(*shhwtstamps));
			shhwtstamps->hwtstamp = ns_to_ktime(rxts->nsec);
			list_add(&rxts->list, &priv->rxpool);
			break;
		}
	}
	spin_unlock(&priv->rx_queue.lock);

	if (!shhwtstamps)
		list_add_tail(&rxts->list, &priv->rxts);
out:
	spin_unlock_irqrestore(&priv->rx_lock, flags);

	if (shhwtstamps)
		netif_rx_ni(skb);
}

static void tx_timestamp_work(struct work_struct *work)
{
	struct jl2xxx_priv *priv =
		container_of(work, struct jl2xxx_priv, tx_ts_work.work);
	struct skb_shared_hwtstamps shhwtstamps;
	struct jl2xxx_skb_info *skb_info;
	struct sk_buff *skb;

	skb = skb_dequeue(&priv->tx_queue);
	if (skb != NULL) {
		skb_info = (struct jl2xxx_skb_info *)skb->cb;
		if (time_after(jiffies, skb_info->tmo)) {
			kfree_skb(skb);
			schedule_delayed_work(&priv->tx_ts_work, 0);
			pr_info("%s: Tx timestamp is timeout!\n", __func__);
			return;
		}
		mutex_lock(&priv->ptp_lock);
		jl2xxx_get_txts(priv);
		mutex_unlock(&priv->ptp_lock);
		if (match(skb, skb_info->ptp_type, priv->dep_ts.seqid)) {
			memset(&shhwtstamps, 0, sizeof(shhwtstamps));
			shhwtstamps.hwtstamp = ns_to_ktime(priv->dep_ts.nsec);
			skb_complete_tx_timestamp(skb, &shhwtstamps);
		} else {
			pr_info("%s: Tx timestamp doesn't match with skb\n",
								__func__);
			pr_info("%s: Seqid=%d\n", __func__,
						priv->dep_ts.seqid);
		}
	}
}

#define MAX_ADJ_TIMES		1000000000
static void phc_adj_state_machine(struct work_struct *work)
{
	struct jl2xxx_priv *priv =
	container_of(work, struct jl2xxx_priv, state_queue.work);
	struct ptp_clock_info *ptp_info = &priv->ptp_info;
	int i;

	switch (priv->adj_state) {
	case PHC_STOP_ADJ:
		if ((priv->ptp_work_ppb >= (ptp_info->max_adj * 9) / 10) ||
		   (priv->ptp_work_ppb <= -(ptp_info->max_adj * 9) / 10))
			priv->adj_state = PHC_LOOP_ADJ;
		else if (priv->ptp_work_ppb != 0 &&
			 priv->last_state == PHC_STEP_ADJ)
			priv->adj_state = PHC_STEP_ADJ;
		break;
	case PHC_LOOP_ADJ:
		for (i = 0; i < MAX_ADJ_TIMES; i++) {
			if (priv->safe_quit)
				goto quit;
			jl2xxx_phc_set_step(priv->phydev,
					    priv->ptp_work_neg_adj,
					    priv->ptp_work_ppb);
			if (i % 10)
				udelay(150);
		}
quit:
		priv->adj_state = PHC_READY_ADJ;
		break;
	case PHC_READY_ADJ:
		if ((priv->ptp_work_ppb >= (ptp_info->max_adj / 2)) ||
		    (priv->ptp_work_ppb <= -(ptp_info->max_adj / 2))) {
			for (i = 0; i < (ptp_info->max_adj / MAX_AMT); i++) {
				jl2xxx_phc_set_step(priv->phydev,
						    priv->ptp_work_neg_adj,
						    priv->ptp_work_ppb);
				udelay(100);
			}
		} else {
			priv->adj_state = PHC_STEP_ADJ;
		}
		break;
	case PHC_STEP_ADJ:
		if (!priv->ptp_work_ppb)
			priv->adj_state = PHC_STOP_ADJ;
		else if ((priv->ptp_work_ppb > (ptp_info->max_adj / 2)) ||
			(priv->ptp_work_ppb < -(ptp_info->max_adj / 2)))
			priv->adj_state = PHC_READY_ADJ;
		jl2xxx_phc_set_step(priv->phydev,
				    priv->ptp_work_neg_adj,
				    priv->ptp_work_ppb);
		priv->last_state = PHC_STEP_ADJ;
		break;
	}
}

static int jl2xxx_ptp_probe(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl2xxx_priv *priv = phydev->priv;
	int i;

	priv->ptp_info = jl2xxx_ptp_info;
	priv->ptp_clock = ptp_clock_register(&priv->ptp_info, dev);
	if (IS_ERR(priv->ptp_clock))
		return PTR_ERR(priv->ptp_clock);

	priv->phydev = phydev;

	INIT_LIST_HEAD(&priv->rxts);
	INIT_LIST_HEAD(&priv->rxpool);

	for (i = 0; i < MAX_RXTS; i++)
		list_add(&priv->rx_pool_data[i].list, &priv->rxpool);

	spin_lock_init(&priv->rx_lock);
	mutex_init(&priv->ptp_lock);

	INIT_DELAYED_WORK(&priv->rx_ts_work, rx_timestamp_work);
	INIT_DELAYED_WORK(&priv->tx_ts_work, tx_timestamp_work);
	INIT_DELAYED_WORK(&priv->state_queue, phc_adj_state_machine);

	skb_queue_head_init(&priv->rx_queue);
	skb_queue_head_init(&priv->tx_queue);

	return 0;
}

static void jl2xxx_ptp_remove(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	priv->safe_quit = true;
	cancel_delayed_work_sync(&priv->rx_ts_work);
	cancel_delayed_work_sync(&priv->tx_ts_work);
	cancel_delayed_work_sync(&priv->state_queue);

	skb_queue_purge(&priv->rx_queue);
	skb_queue_purge(&priv->tx_queue);

	if (priv->ptp_clock)
		ptp_clock_unregister(priv->ptp_clock);
}


static int jl2xxx_probe(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl2xxx_priv *jl2xxx = NULL;
	int err;

	jl2xxx = devm_kzalloc(dev, sizeof(*jl2xxx), GFP_KERNEL);
	if (!jl2xxx)
		return -ENOMEM;

	phydev->priv = jl2xxx;

#if (JLSEMI_KERNEL_DEVICE_TREE_USE)
	if (!dev->of_node)
		JLSEMI_PHY_MSG("%s: Find device node failed\n", __func__);
#endif
	err = jl2xxx_operation_args_get(phydev);
	if (err < 0)
		return err;

	err = jl2xxx_ptp_probe(phydev);
	if (err < 0)
		return err;

	if (jl2xxx->intr.enable & JL2XXX_INTR_STATIC_OP_EN)
		phydev->irq = JL2XXX_INTR_IRQ;

	jl2xxx->static_inited = false;
	jl2xxx->nstats = ARRAY_SIZE(jl2xxx_hw_stats);
	jl2xxx->hw_stats = jl2xxx_hw_stats;
	jl2xxx->stats = kcalloc(jl2xxx->nstats, sizeof(u64), GFP_KERNEL);
	if (!jl2xxx->stats)
		return -ENOMEM;

	return 0;
}

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

static int jl1xxx_config_aneg(struct phy_device *phydev)
{
	return genphy_config_aneg(phydev);
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
	    ((phy_mode == JL2XXX_FIBER_RGMII_MODE) ||
	    (phy_mode == JL2XXX_UTP_FIBER_RGMII_MODE)))
		return jl2xxx_config_aneg_fiber(phydev);

	return genphy_config_aneg(phydev);
}

static int jl2xxx_suspend(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	/* clear wol event */
	if (priv->wol.enable & JL2XXX_WOL_STATIC_OP_EN) {
		jlsemi_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);
		jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);
	}
	return genphy_suspend(phydev);
}

static int jl2xxx_resume(struct phy_device *phydev)
{
	return genphy_resume(phydev);
}

#if (JLSEMI_PHY_WOL)
static void jl2xxx_get_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int wol_en;

	if (priv->wol.ethtool) {
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

	if (priv->wol.ethtool) {
		if (wol->wolopts & WAKE_MAGIC) {
			err = jl2xxx_wol_dynamic_op_set(phydev);
			if (err < 0)
				return err;
		}
	}

	return 0;
}
#endif

#if (JL2XXX_PHY_TUNABLE)
static int jl2xxx_get_tunable(struct phy_device *phydev,
			      struct ethtool_tunable *tuna, void *data)
{
	struct jl2xxx_priv *priv = phydev->priv;

	switch (tuna->id) {
	case ETHTOOL_PHY_FAST_LINK_DOWN:
		if (priv->fld.ethtool)
			return jl2xxx_fld_dynamic_op_get(phydev, data);
		else
			return 0;
	case ETHTOOL_PHY_DOWNSHIFT:
		if (priv->downshift.ethtool)
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
		if (priv->fld.ethtool)
			return jl2xxx_fld_dynamic_op_set(phydev, data);
		else
			return 0;
	case ETHTOOL_PHY_DOWNSHIFT:
		if (priv->downshift.ethtool)
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

static void jl2xxx_remove(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl2xxx_priv *priv = phydev->priv;

	jl2xxx_ptp_remove(phydev);

	kfree(priv->stats);
	if (priv)
		devm_kfree(dev, priv);
}

static inline int jlsemi_aneg_done(struct phy_device *phydev)
{
	int retval = phy_read(phydev, MII_BMSR);

	return (retval < 0) ? retval : (retval & BMSR_ANEGCOMPLETE);
}

static int jl2xxx_aneg_done(struct phy_device *phydev)
{
	u16 phy_mode;
	int val;

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE18,
				JL2XXX_WORK_MODE_REG);
	phy_mode = val & JL2XXX_WORK_MODE_MASK;

	if (phydev->interface == PHY_INTERFACE_MODE_SGMII)
		return 0;

	// fiber not an complite
	if ((phydev->interface != PHY_INTERFACE_MODE_SGMII) &&
	    ((phy_mode == JL2XXX_FIBER_RGMII_MODE) ||
	    (phy_mode == JL2XXX_UTP_FIBER_RGMII_MODE)))
		return BMSR_ANEGCOMPLETE;

	return jlsemi_aneg_done(phydev);
}


static struct phy_driver jlsemi_drivers[] = {
	{
		.phy_id		= JL1XXX_PHY_ID,
		.phy_id_mask    = JLSEMI_PHY_ID_MASK,
		.name           = DRIVER_NAME_100M,
		/* PHY_BASIC_FEATURES */
		.features	= PHY_BASIC_FEATURES,
		.probe		= jl1xxx_probe,
		.config_intr	= jl1xxx_config_intr,
		.read_status	= jl1xxx_read_status,
		.config_init    = jl1xxx_config_init,
		.config_aneg    = jl1xxx_config_aneg,
		.aneg_done	= jlsemi_aneg_done,
		.suspend        = jl1xxx_suspend,
		.resume         = jl1xxx_resume,
		.remove		= jl1xxx_remove,
#if (JLSEMI_PHY_WOL)
		.get_wol	= jl1xxx_get_wol,
		.set_wol	= jl1xxx_set_wol,
#endif
	},
	{
		.phy_id         = JL2XXX_PHY_ID,
		.phy_id_mask    = JLSEMI_PHY_ID_MASK,
		.name           = DRIVER_NAME_1000M,
		/* PHY_BASIC_FEATURES */
		.features	= PHY_GBIT_FEATURES,
		.probe		= jl2xxx_probe,
		.config_intr	= jl2xxx_config_intr,
		.read_status	= jl2xxx_read_status,
		.config_init    = jl2xxx_config_init,
		.config_aneg    = jl2xxx_config_aneg,
		.aneg_done	= jl2xxx_aneg_done,
		.suspend        = jl2xxx_suspend,
		.resume         = jl2xxx_resume,
		.remove		= jl2xxx_remove,
		.ts_info	= jl2xxx_ts_info,
		.hwtstamp	= jl2xxx_hwtstamp,
		.rxtstamp	= jl2xxx_rxtstamp,
		.txtstamp	= jl2xxx_txtstamp,
#if (JLSEMI_PHY_WOL)
		.get_wol	= jl2xxx_get_wol,
		.set_wol	= jl2xxx_set_wol,
#endif
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

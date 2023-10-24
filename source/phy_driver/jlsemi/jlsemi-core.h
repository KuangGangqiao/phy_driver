/* SPDX-License-Identifier: GPL-2.0+
 *
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

#define JLSEMI_KERNEL_DEVICE_TREE_USE	0

#include <linux/phy.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/of.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/ethtool.h>
#include <linux/ptp_classify.h>
#include <linux/ptp_clock_kernel.h>
#include <linux/net_tstamp.h>
#include <linux/if_vlan.h>
#include <linux/timecounter.h>
#if (JLSEMI_KERNEL_DEVICE_TREE_USE)
#include <dt-bindings/phy/jlsemi-dt-phy.h>
#else
#include "jlsemi-dt-phy.h"
#endif

#define JL1XXX_PHY_ID		0x937c4023
#define JL2XXX_PHY_ID		0x937c4032
#define JL3XXX_PHY_ID		0x937c4018
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

#define JL1XXX_PAGE24		24
#define JL1XXX_REG24		24
#define JL1XXX_MDI_TX_BM_MASK	0x1c00
#define JL1XXX_MDI_TX_BM(n)	(n << 10)
#define JL1XXX_MDI_TX_SRN	BIT(0)

#define JL1XXX_PAGE7		7
#define JL1XXX_REG16		16
#define JL1XXX_RMII_MODE		BIT(3)
#define JL1XXX_RMII_CLK_50M_INPUT	BIT(12)
#define JL1XXX_RMII_TX_SKEW_MASK	(0xf << 8)
#define JL1XXX_RMII_TX_SKEW(n)		((n << 8) & JL1XXX_RMII_TX_SKEW_MASK)
#define JL1XXX_RMII_RX_SKEW_MASK	(0xf << 4)
#define JL1XXX_RMII_RX_SKEW(n)		((n << 4) & JL1XXX_RMII_RX_SKEW_MASK)
#define JL1XXX_RMII_CRS_DV		BIT(2)

#define JL2XXX_PAGE0		0
#define JL2XXX_BMCR_REG		0x0000
#define JL2XXX_SOFT_RESET	BIT(15)
#define JL2XXX_SPEED_LSB	BIT(13)
#define JL2XXX_AUTONEG_EN	BIT(12)
#define JL2XXX_SPEED_MSB	BIT(6)
#define JL2XXX_DSFT_CTRL_REG	17
#define JL2XXX_DSFT_EN		BIT(12)
#define JL2XXX_DSFT_SMART_EN	BIT(13)
#define JL2XXX_DSFT_TWO_WIRE_EN	BIT(14)
#define JL2XXX_DSFT_AN_ERR_EN	BIT(15)
#define JL2XXX_DSFT_STL_MASK	0x03e0
#define JL2XXX_DSFT_STL_CNT(n)	(((n << 5) & JL2XXX_DSFT_STL_MASK))
#define JL2XXX_DSFT_AN_MASK	0x001f
#define JL2XXX_DSFT_CNT_MAX	32
#define JL2XXX_PHY_INFO_REG	29
#define JL2XXX_PATCH_MASK	0xffff
#define JL2XXX_SW_MASK		0xffff
#define JL2XXX_AUTO_GAIN_DIS	BIT(6)

#define JLSEMI_PAGE31		0x001f
#define JL2XXX_WOL_CTRL_PAGE	0x0012
#define JL2XXX_WOL_CTRL_REG	0x0015
#define JL2XXX_WOL_STAS_PAGE	0x1200
#define JL2XXX_WOL_STAS_REG	0x0010
#define JL2XXX_MAC_ADDR2_REG	0x0011
#define JL2XXX_MAC_ADDR1_REG	0x0012
#define JL2XXX_MAC_ADDR0_REG	0x0013
#define JL2XXX_WOL_EVENT	BIT(1)
#define JL2XXX_WOL_POLARITY	BIT(14)
#define JL2XXX_WOL_EN		BIT(15)
#define JL2XXX_WOL_GLB_EN	BIT(6)

#define JL2XXX_PAGE2626		2626
#define JL2XXX_INTR_CTRL_REG	18
#define JL2XXX_INTR_LINK_CHANGE	BIT(4)
#define JL2XXX_INTR_AN_COMPLETE	BIT(3)
#define JL2XXX_INTR_AN_PAGE	BIT(2)
#define JL2XXX_INTR_AN_ERR	BIT(0)

#define JL2XXX_PAGE2627		2627
#define JL2XXX_INTR_STATUS_REG	29
#define JL2XXX_CLK_CTRL_REG	25
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

#define JL2XXX_PAGE18		18
#define JL2XXX_RXC_OUT_REG	21
#define JL2XXX_RXC_OUT		BIT(14)

#define JL2XXX_WORK_MODE_REG	21
#define JL2XXX_WORK_MODE_MASK	0x7

#define JL2XXX_PAGE160		160
#define JL2XXX_REG25		25
#define JL2XXX_CPU_RESET	BIT(3)

#define JL2XXX_PAGE173		173
#define JL2XXX_PATCH_REG	0x0010
#define JL2XXX_REG16		16
#define JL2XXX_REG17		17
#define JL2XXX_LOAD_GO		0
#define JL2XXX_LOAD_DATA0	0x3a01
#define JL2XXX_REG20		20
#define JL2XXX_REG21		21
#define JL2XXX_LPBK_MODE_MASK	0x6
#define JL2XXX_LPBK_PMD_MODE	BIT(2)
#define JL2XXX_LPBK_EXT_MODE	BIT(1)

#define JL2XXX_PAGE174		174

#define JL2XXX_REG29		29

#define JL2XXX_PAGE179		179

#define JL2XXX_PAGE191		191
#define JL2XXX_RGMII_CFG	BIT(3)

#define JL2XXX_PAGE258		258
#define JL2XXX_SLEW_RATE_CTRL_REG	23
#define JL2XXX_SLEW_RATE_EN		BIT(12)
#define JL2XXX_SLEW_RATE_REF_CLK	BIT(13)
#define JL2XXX_SLEW_RATE_SEL_CLK	BIT(14)

#define JL2XXX_REG20		20
#define JL2XXX_SPEED1000_NO_AN	(BIT(11) | BIT(10))

#define LED_PERIOD_MASK		0xff00
#define LEDPERIOD(n)		((n << 8) & LED_PERIOD_MASK)
#define LED_ON_MASK		0x00ff
#define LEDON(n)		((n << 0) & LED_ON_MASK)

#define ADVERTISE_FIBER_1000HALF	0x40
#define ADVERTISE_FIBER_1000FULL	0x20

#define JL3XXX_UNKNOWN		0x0000
#define JL3XXX_100BASE_T1	0x0001
#define JL3XXX_1000BASE_T1	0x0002

#define JL3XXX_PMA_CTL_ADDR	0x0001
#define JL3XXX_PMA_CTL		0x0834
#define JL3XXX_MASTER_MODE	BIT(14)
#define JL3XXX_PMA_BASE		0x0000
#define JL3XXX_PMA_RESET	BIT(15)
#define JL3XXX_PHYSID1		0x0002
#define JL3XXX_PHYSID2		0x0003

#define JL3XXX_PCS_CTL_ADDR	0x0003
#define JL3XXX_PCS_CTL		0x0000
#define JL3XXX_PCS_SPEED_LSB	BIT(13)
#define JL3XXX_PCS_SPEED_MSB	BIT(6)

#define JL3XXX_IEEE_CTL_ADDR	0x0003
#define JL3XXX_IEEE_CTL		0x0901
#define JL3XXX_GLOBAL_LINK	BIT(2)
#define JL3XXX_RGMII_CFG	0x8800
#define JL3XXX_RGMII_TX_DLY	BIT(15)
#define JL3XXX_RGMII_RX_DLY	BIT(14)
#define JL3XXX_DEVAD31		0x1f
#define JL3XXX_ARR_SEC_TIME0	0x3806
#define JL3XXX_ARR_SEC_TIME1	0x3807
#define JL3XXX_ARR_SEC_TIME2	0x3808
#define JL3XXX_ARR_NSEC_TIME0	0x3809
#define JL3XXX_ARR_NSEC_TIME1	0x380a
#define JL3XXX_ARR_SEQID	0x380b
#define JL3XXX_ARR_VALID	0x3805
#define JL3XXX_ARR_VALID_BIT	BIT(0)
#define JL3XXX_DEP_VALID	0x3900
#define JL3XXX_DEP_VALID_BIT	BIT(0)
#define JL3XXX_DEP_SEC_TIME0	0x3901
#define JL3XXX_DEP_SEC_TIME1	0x3902
#define JL3XXX_DEP_SEC_TIME2	0x3903
#define JL3XXX_DEP_NSEC_TIME0	0x3904
#define JL3XXX_DEP_NSEC_TIME1	0x3905
#define JL3XXX_DEP_SEQID	0x3906

#define JL3XXX_TIME_DEC_EN	0x3a00
#define JL3XXX_TIME_DEC_EN_BIT	BIT(3)
#define JL3XXX_TIME_DEC		0x3a0b
#define JL3XXX_TIME_DEC_AMT_EXP	0x3a0c
#define JL3XXX_TIME_DEC_OP_BIT	BIT(15)
#define TIMEDECAMT_MASK		0x7fe0
#define TIMEDECAMT(n)		((n << 5) & TIMEDECAMT_MASK)
#define TIMEBASE		(1 << 10)
#define JL3XXX_READ_PULS	0x3b08
#define JL3XXX_READ_PULS_DATA	0x3b09
#define JL3XXX_READ_PULS_CMD	0x8e0e

#define JL3XXX_PPS		0x3b0a
#define JL3XXX_PPS_RANGE_MASK	0x0700
#define JL3XXX_PPS_RANGE(n)	((n << 8) & JL3XXX_PPS_RANGE_MASK)
#define JL3XXX_PPS_WIDTH_MASK	0xf000
#define JL3XXX_PPS_WIDTH(n)	((n << 12) & JL3XXX_PPS_WIDTH_MASK)

#define JL3XXX_PPS_PHASE_BIT	BIT(3)
#define JL3XXX_PPS_GPIO_IO	0x3800
#define JL3XXX_PPS_GPIO_MASK	0x0038
#define JL3XXX_PPS_GPIO(n)	((n << 5) & JL3XXX_PPS_GPIO_MASK)

#define JL3XXX_GBL_SEC_WORD0	0x3b0b
#define JL3XXX_GBL_SEC_WORD1	0x3b0c
#define JL3XXX_GBL_SEC_WORD2	0x3b0d
#define JL3XXX_GBL_NSEC_WORD0	0x3b0e
#define JL3XXX_GBL_NSEC_WORD1	0x3b0f

#define JL3XXX_PTP_CFG0		0x3b30
#define JL3XXX_PTP_POWER_DOWN	BIT(9)
#define JL3XXX_PTP_TS_SEL_SEC	BIT(13)

#define JL3XXX_PPS_SEL_GPIO	0x0706
#define JL3XXX_PPS_SEL_GPIO_BIT	BIT(11)

#define JL3XXX_PPS_GPIO_OUT	0x0708
#define JL3XXX_PPS_GPIO_OUT_BIT BIT(1)

#define JL3XXX_PTS_TRANS	0x3b31
#define JL3XXX_PTS_TRANS_MASK	0xf000
#define JL3XXX_PTS_TRANS_S(n)	((n << 12) & JL3XXX_PTS_TRANS_MASK)

#define JL3XXX_PTP_DIS		0x3b31
#define JL3XXX_PTP_DIS_BIT	BIT(0)

#define JL3XXX_PTS_CLK_CYCLE	0x3b41
#define JL3XXX_TS_CLK_CYCLE_VAL	0x1f40

#define JL3XXX_PTS_MSGID	0x3b51
#define JL3XXX_PTS_MSGID_EN(n)	n

#define JL3XXX_PTS_HW_ACCEL	0x3b33
#define JL3XXX_PTS_HW_ACCEL_BIT BIT(6)
#define JL3XXX_PTS_KEEP_SA_BIT	BIT(5)

#define JL3XXX_PTS_GLB_DAT	0x3b05
#define JL3XXX_PTS_GLB_DAT_MASK	0xff
#define JL3XXX_PTS_GLB_DAT_S(n)	((n) & JL3XXX_PTS_GLB_DAT_MASK)
#define JL3XXX_PTS_UPDAT_BIT	BIT(15)

#define JL3XXX_DEVAD3		3
#define JL3XXX_PTP_CONFIG	0x8000
#define JL3XXX_PTP_RESET_BIT	BIT(12)
#define JL3XXX_PTP_EN_BIT	BIT(4)


#define JL3XXX_RGMII_TX_ON	true
#define JL3XXX_RGMII_TX_OFF	false
#define JL3XXX_RGMII_RX_ON	true
#define JL3XXX_RGMII_RX_OFF	false

#define MAX_AMT		(1 << 26)	/* max adjtime step */

#define MAX_RXTS 64
#define SKB_TIMESTAMP_TIMEOUT 3 /* jiffies */

/*************************************************************************/
struct jl_patch {
	const u32 *data;
	u16 data_len;
	const u16 version;
	struct {
		const u16 *info;
		u16 info_len;
	} phy;
	bool (*check)(struct phy_device *phydev, struct jl_patch *patch);
	int (*load)(struct phy_device *phydev, struct jl_patch *patch);
	int (*verify)(struct phy_device *phydev, struct jl_patch *patch);
};

struct jl_hw_stat {
	const char *string;
	u8 reg;
	u16 page;
	u16 mask;
	u16 enable;
};

static const struct jl_hw_stat jl_phy[] = {
	{
		.string = "page0,reg0",
		.enable = true,
		.page = 0,
		.reg = 0,
	}, {
		.string = "page0,reg1",
		.enable = false,
		.page = 0,
		.reg = 1,
	},
};

static const struct jl_hw_stat jl2xxx_hw_stats[] = {
	{
		.string	= "phy_patch_version",
		.reg	= JL2XXX_PATCH_REG,
		.page	= JL2XXX_PAGE174,
		.mask	= JL2XXX_PATCH_MASK,
	}, {
		.string	= "phy_software_version",
		.reg	= JL2XXX_PHY_INFO_REG,
		.page	= JL2XXX_PAGE0,
		.mask	= JL2XXX_SW_MASK,
	},
};

struct jl_led_ctrl {
	u32 enable;			/* LED control enable */
	u32 mode;			/* LED work mode */
	u32 global_period;		/* LED global twinkle period */
	u32 global_on;			/* LED global twinkle hold on time */
	u32 gpio_output;		/* LED is used as gpio output */
	u32 polarity;			/* LED polarity */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_fld_ctrl {
	u32 enable;			/* Fast link down control enable */
	u32 delay;			/* Fast link down time */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_wol_ctrl {
	u32 enable;			/* Wake On LAN control enable */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_intr_ctrl {
	u32 enable;			/* Interrupt control enable */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_downshift_ctrl {
	u32 enable;			/* Downshift control enable */
	u32 count;			/* Downshift control count */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_rgmii_ctrl {
	u32 enable;			/* Rgmii control enable */
	u32 rx_delay;			/* Rgmii control rx delay */
	u32 tx_delay;			/* Rgmii control tx delay */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_patch_ctrl {
	u32 enable;			/* Patch control enable */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_clk_ctrl {
	u32 enable;			/* Clock 125M control enable */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_work_mode_ctrl {
	u32 enable;			/* Work mode control enable */
	u32 mode;			/* Work mode select mode */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_loopback_ctrl {
	u32 enable;			/* Loopback control enable */
	u32 mode;			/* Loopback select mode */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_mdi_ctrl {
	u32 enable;			/* Mdi control enable */
	u32 rate;			/* Mdi select Rate */
	u32 amplitude;			/* Mdi select amplitude */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_rmii_ctrl {
	u32 enable;			/* Rmii control enable */
	u32 tx_timing;			/* Rmii modify tx timing */
	u32 rx_timing;			/* Rmii modify rx timing */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_slew_rate_ctrl {
	u32 enable;			/* Slew rate control enable */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl_rxc_out_ctrl {
	u32 enable;			/* Rx clock out control enable */
	bool ethtool;			/* Whether the ethtool is supported */
};

struct jl1xxx_priv {
	struct jl_led_ctrl led;
	struct jl_wol_ctrl wol;
	struct jl_intr_ctrl intr;
	bool static_inited;		/* Initialization flag */
	struct jl_mdi_ctrl mdi;
	struct jl_rmii_ctrl rmii;
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
	bool static_inited;		/* Initialization flag */
	int nstats;			/* Record for dynamic operation */
	u64 *stats;			/* Pointer for dynamic operation */
	struct jl_work_mode_ctrl work_mode;
	struct jl_loopback_ctrl lpbk;
	struct jl_slew_rate_ctrl slew_rate;
	struct jl_rxc_out_ctrl rxc_out;
};

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

enum phc_adj_state {
	PHC_STOP_ADJ = 0,
	PHC_LOOP_ADJ,
	PHC_READY_ADJ,
	PHC_STEP_ADJ,
};

enum ptp_msg_type {
	SYNC = 0,
	DELAY_REQ,
	PDELAY_REQ,
	PDELAY_RESP,
	UNKNOW_TYPE = 0xf,
};

struct jl3xxx_hwts {
	u32 nsec;
	u64 sec;
	u16 seqid;
	u8 msgtype;
};

struct jl3xxx_skb_info {
	int ptp_type;
	unsigned long tmo;
};

struct rxts {
	struct list_head list;
	unsigned long tmo;
	u32 nsec;
	u64 sec;
	u16 seqid;
};

struct jl3xxx_priv {
	int phc_offset;
	int phc_offset_old;
	int curr_offset_num;
	bool init_master;
	bool is_master;
	bool init_adj;
	bool init_flag;
	bool init_ptp;
	bool safe_quit;
	long last_ppb;
	long ptp_work_ppb;
	bool ptp_work_neg_adj;
	enum phc_adj_state adj_state;
	enum phc_adj_state last_state;
	u64 glb_seconds;
	u32 glb_nseconds;
	struct phy_device *phydev;
	struct mutex ptp_lock;
	struct jl3xxx_hwts arr_ts;
	struct jl3xxx_hwts dep_ts;
	struct ptp_clock *ptp_clock;
	struct ptp_clock_info ptp_info;
	struct delayed_work rx_ts_work;
	struct delayed_work tx_ts_work;
	struct delayed_work state_queue;
	int hwts_tx_en;
	int hwts_rx_en;
	/* list of rx timestamps */
	struct list_head rxts;
	struct list_head rxpool;
	struct rxts rx_pool_data[MAX_RXTS];
	/* protects above three fields from concurrent access */
	spinlock_t rx_lock;
	struct sk_buff_head rx_queue;
	struct sk_buff_head tx_queue;
};

int jl3xxx_ptp_p2p_onestep_master_config(struct phy_device *phydev);
int jl3xxx_gptp_master_config(struct phy_device *phydev);

void jl3xxx_ptp_sync_to_system_clock(struct phy_device *phydev);

int jl3xxx_mmd_modify(struct phy_device *phydev,
		      int devad, u16 reg, u16 mask, u16 bits);

int jl3xxx_fetch_mmd_bit(struct phy_device *phydev,
			 int devad, u16 reg, u16 bit);

int jl3xxx_set_mmd_bits(struct phy_device *phydev,
			int devad, u16 reg, u16 bits);

int jl3xxx_clear_mmd_bits(struct phy_device *phydev,
			  int devad, u16 reg, u16 bits);

int jl3xxx_check_link(struct phy_device *phydev);

int jl3xxx_set_mode(struct phy_device *phydev, int mode);

int jl3xxx_set_speed(struct phy_device *phydev, int speed);

int jl3xxx_get_speed(struct phy_device *phydev);

int jl3xxx_set_rgmii_dly(struct phy_device *phydev, bool tx_on, bool rx_on);

int jl3xxx_ptp_probe(struct phy_device *phydev);

void jl3xxx_ptp_remove(struct phy_device *phydev);

int jl3xxx_ptp_config(struct phy_device *phydev);

enum ptp_msg_type jl3xxx_ptp_get_msgtype(struct sk_buff *skb, int type);

void prune_rx_ts(struct jl3xxx_priv *priv);

int match(struct sk_buff *skb, unsigned int type, u16 seqid);

/* macros to simplify debug checking */
#define JLSEMI_PHY_MSG(msg, args...) printk(KERN_INFO msg, ## args)

/************************* JLSemi iteration code *************************/
struct device *jlsemi_get_mdio(struct phy_device *phydev);

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

int jl1xxx_operation_args_get(struct phy_device *phydev);

int jl1xxx_static_op_init(struct phy_device *phydev);

int jl2xxx_operation_args_get(struct phy_device *phydev);

int jl2xxx_static_op_init(struct phy_device *phydev);

int jlsemi_soft_reset(struct phy_device *phydev);

int jl2xxx_pre_init(struct phy_device *phydev, struct jl_patch *patch);

bool jl2xxx_read_fiber_status(struct phy_device *phydev);

int jl2xxx_config_aneg_fiber(struct phy_device *phydev);

/********************** Convenience function for phy **********************/

/* Notice: You should change page 0 when you When you call it after */
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
#if (KERNEL_VERSION(4, 0, 0) > LINUX_VERSION_CODE)

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


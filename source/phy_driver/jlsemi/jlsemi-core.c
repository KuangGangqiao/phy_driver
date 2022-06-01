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
#include "jlsemi-core.h"
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>

#define JL1XXX_PAGE24		24
#define JL1XXX_LED_BLINK_REG	25

#define JL1XXX_PAGE128		128
#define JL1XXX_LED_GPIO_REG	29
#define JL1XXX_WOL_CTRL_REG	28

#define JL2XXX_PAGE3332		3332
#define JL2XXX_LED_CTRL_REG	16
#define JL2XXX_PAGE4096		4096
#define JL2XXX_LED_BLINK_REG	20
#define JL2XXX_LED_POLARITY_REG	19

#define JL2XXX_PAGE0		0
#define JL2XXX_PAGE128		128
#define JL2XXX_FLD_CTRL_REG	28
#define JL2XXX_FLD_EN		BIT(13)
#define JL2XXX_FLD_MASK		GENMASK(12, 11)
#define JL2XXX_FLD_MASK_HEAD	11
#define JL2XXX_FLD_DELAY_00MS	0
#define JL2XXX_FLD_DELAY_10MS	1
#define JL2XXX_FLD_DELAY_20MS	2
#define JL2XXX_FLD_DELAY_40MS	3


#define JL2XXX_SUPP_LED_MODE	(JL2XXX_LED0_LINK10 | \
				 JL2XXX_LED0_LINK100 | \
				 JL2XXX_LED0_LINK1000 | \
				 JL2XXX_LED0_ACTIVITY | \
				 JL2XXX_LED1_LINK10 | \
				 JL2XXX_LED1_LINK100 | \
				 JL2XXX_LED1_LINK1000 | \
				 JL2XXX_LED1_ACTIVITY | \
				 JL2XXX_LED2_LINK10 | \
				 JL2XXX_LED2_LINK100 | \
				 JL2XXX_LED2_LINK1000 | \
				 JL2XXX_LED2_ACTIVITY)

#define JL2XXX_SUPP_LED_POLARITY (JL2XXX_LED0_POLARITY | \
				  JL2XXX_LED1_POLARITY | \
				  JL2XXX_LED2_POLARITY)

#define JL1XXX_SUPP_GPIO	(JL1XXX_GPIO_LED0_EN | \
				 JL1XXX_GPIO_LED0_OUT | \
				 JL1XXX_GPIO_LED1_EN | \
				 JL1XXX_GPIO_LED1_OUT)

#define JL1XXX_SUPP_LED_MODE	(JL1XXX_LED0_EEE | \
				 JL1XXX_LED0_100_ACTIVITY | \
				 JL1XXX_LED0_10_ACTIVITY | \
				 JL1XXX_LED0_100_LINK | \
				 JL1XXX_LED0_10_LINK | \
				 JL1XXX_LED1_EEE | \
				 JL1XXX_LED1_100_ACTIVITY | \
				 JL1XXX_LED1_10_ACTIVITY | \
				 JL1XXX_LED1_100_LINK | \
				 JL1XXX_LED1_10_LINK)

/************************* Configuration section *************************/


/************************* JLSemi iteration code *************************/
static int jl1xxx_led_static_op_set(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	/* Enable LED operation */
	jlsemi_set_bits(phydev, JL1XXX_PAGE7,
			JL1XXX_LED_REG, JL1XXX_LED_EN);

	/* Set led mode */
	if (priv->led.enable & JL1XXX_LED_MODE_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE129,
					      JL1XXX_LED_MODE_REG,
					      JL1XXX_SUPP_LED_MODE,
					      priv->led.mode);
		if (err < 0)
			return err;
	}
	/* Set led period */
	if (priv->led.enable & JL1XXX_LED_GLOABL_PERIOD_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE24,
					      JL1XXX_LED_BLINK_REG,
					      LED_PERIOD_MASK,
					      LEDPERIOD(
					      priv->led.global_period));
		if (err < 0)
			return err;
	}
	/* Set led on time */
	if (priv->led.enable & JL1XXX_LED_GLOBAL_ON_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE24,
					      JL1XXX_LED_BLINK_REG,
					      LED_ON_MASK,
					      LEDON(priv->led.global_on));
		if (err < 0)
			return err;
	}
	/*Set led gpio output */
	if (priv->led.enable & JL1XXX_LED_GPIO_OUT_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE128,
					      JL1XXX_LED_GPIO_REG,
					      JL1XXX_SUPP_GPIO,
					      priv->led.gpio_output);
		if (err < 0)
			return err;
	}

	return 0;
}

static int jl2xxx_led_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	/* Set led mode */
	if (priv->led.enable & JL2XXX_LED_MODE_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE3332,
					      JL2XXX_LED_CTRL_REG,
					      JL2XXX_SUPP_LED_MODE,
					      priv->led.mode);
		if (err < 0)
			return err;
	}
	/* Set led period */
	if (priv->led.enable & JL2XXX_LED_GLOABL_PERIOD_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE4096,
					      JL2XXX_LED_BLINK_REG,
					      LED_PERIOD_MASK,
					      LEDPERIOD(
					      priv->led.global_period));
		if (err < 0)
			return err;
	}
	/* Set led on time */
	if (priv->led.enable & JL2XXX_LED_GLOBAL_ON_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE4096,
					      JL2XXX_LED_BLINK_REG,
					      LED_ON_MASK,
					      LEDON(priv->led.global_on));
		if (err < 0)
			return err;
	}
	/* Set led polarity */
	if (priv->led.enable & JL2XXX_LED_POLARITY_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE4096,
					      JL2XXX_LED_POLARITY_REG,
					      JL2XXX_SUPP_LED_POLARITY,
					      priv->led.polarity);
		if (err < 0)
			return err;
	}

	return 0;
}

static int jl1xxx_dts_led_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl1xxx,led-enable",
				   &priv->led.enable);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl1xxx,led-mode",
				   &priv->led.mode);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl1xxx,led-period",
				   &priv->led.global_period);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl1xxx,led-on",
				   &priv->led.global_on);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl1xxx,led-gpio",
				   &priv->led.gpio_output);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_dts_wol_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl1xxx,wol-enable",
				   &priv->wol.enable);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_dts_intr_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl1xxx,interrupt-enable",
				   &priv->intr.enable);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_dts_led_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl2xxx,led-enable",
				  &priv->led.enable);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl2xxx,led-mode",
				   &priv->led.mode);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl2xxx,led-period",
				   &priv->led.global_period);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl2xxx,led-on",
				   &priv->led.global_on);
	if (err < 0)
		return err;

	err = of_property_read_u16(of_node, "jl2xxx,led-polarity",
				   &priv->led.polarity);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_c_marcro_led_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	/* Config LED */
	struct jl_led_ctrl led_cfg = {
		.enable		= JL1XXX_LED_CTRL_EN,
		.mode		= JL1XXX_CFG_LED_MODE,
		.global_period	= JL1XXX_GLOBAL_PERIOD_MS,
		.global_on	= JL1XXX_GLOBAL_ON_MS,
		.gpio_output	= JL1XXX_CFG_GPIO,
	};

	priv->led = led_cfg;

	return 0;
}

static int jl1xxx_c_marcro_wol_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	/* Config WOL */
	struct jl_wol_ctrl wol_cfg = {
		.enable		= JL1XXX_WOL_CTRL_EN,
	};

	priv->wol = wol_cfg;

	return 0;
}

static int jl1xxx_c_marcro_intr_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	/* Config WOL */
	struct jl_intr_ctrl intr_cfg = {
		.enable		= JL1XXX_INTR_CTRL_EN,
	};

	priv->intr = intr_cfg;

	return 0;
}

static int jl2xxx_c_marcro_led_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	/* Config LED */
	struct jl_led_ctrl led_cfg = {
		.enable		= JL2XXX_LED_CTRL_EN,
		.mode		= JL2XXX_CFG_LED_MODE,
		.global_period	= JL2XXX_GLOBAL_PERIOD_MS,
		.global_on	= JL2XXX_GLOBAL_ON_MS,
		.polarity	= JL2XXX_LED_POLARITY,
	};

	priv->led = led_cfg;

	return 0;
}

static int jl1xxx_led_operation_mode(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->led.op;

	if (JL1XXX_LED_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL1XXX_LED_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL1XXX_LED_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl1xxx_wol_operation_mode(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->wol.op;

	if (JL1XXX_WOL_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL1XXX_WOL_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL1XXX_WOL_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl1xxx_intr_operation_mode(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->intr.op;

	if (JL1XXX_INTR_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL1XXX_INTR_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL1XXX_INTR_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl2xxx_led_operation_mode(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->led.op;

	if (JL2XXX_LED_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL2XXX_LED_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL2XXX_LED_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl1xxx_wol_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->wol.op;

	if (mode->static_op == STATIC_C_MACRO)
		jl1xxx_c_marcro_wol_cfg_get(phydev);
	else if (mode->static_op == STATIC_DEVICE_TREE)
		jl1xxx_dts_wol_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->wol.enable &= ~JL1XXX_WOL_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->wol.enable |= JL1XXX_WOL_DYNAMIC_OP_EN;
	else
		priv->wol.enable &= ~JL1XXX_WOL_DYNAMIC_OP_EN;

	return 0;
}

static int jl1xxx_intr_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->intr.op;

	if (mode->static_op == STATIC_C_MACRO)
		jl1xxx_c_marcro_intr_cfg_get(phydev);
	else if (mode->static_op == STATIC_DEVICE_TREE)
		jl1xxx_dts_intr_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->intr.enable &= ~JL1XXX_INTR_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->intr.enable |= JL1XXX_INTR_DYNAMIC_OP_EN;
	else
		priv->intr.enable &= ~JL1XXX_INTR_DYNAMIC_OP_EN;

	return 0;
}

static int jl1xxx_led_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->led.op;

	if (mode->static_op == STATIC_C_MACRO)
		jl1xxx_c_marcro_led_cfg_get(phydev);
	else if (mode->static_op == STATIC_DEVICE_TREE)
		jl1xxx_dts_led_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->led.enable &= ~JL1XXX_LED_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->led.enable |= JL1XXX_LED_DYNAMIC_OP_EN;
	else
		priv->led.enable &= ~JL1XXX_LED_DYNAMIC_OP_EN;

	return 0;
}

static int jl2xxx_led_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->led.op;

	if (mode->static_op == STATIC_C_MACRO)
		jl2xxx_c_marcro_led_cfg_get(phydev);
	else if (mode->static_op == STATIC_DEVICE_TREE)
		jl2xxx_dts_led_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->led.enable &= ~JL2XXX_LED_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->led.enable |= JL2XXX_LED_DYNAMIC_OP_EN;
	else
		priv->led.enable &= ~JL2XXX_LED_DYNAMIC_OP_EN;

	return 0;
}


static int jl2xxx_dts_fld_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl2xxx,fld-enable",
				   &priv->fld.enable);
	if (err < 0)
		return err;

	err = of_property_read_u8(of_node, "jl2xxx,fld-delay",
				  &priv->fld.delay);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_dts_wol_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl2xxx,wol-enable",
				   &priv->wol.enable);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_dts_intr_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl2xxx,interrupt-enable",
				   &priv->intr.enable);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_dts_downshift_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	int err;

	err = of_property_read_u16(of_node, "jl2xxx,downshift-enable",
				   &priv->downshift.enable);
	if (err < 0)
		return err;

	err = of_property_read_u8(of_node, "jl2xxx,downshift-count",
				   &priv->downshift.count);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_c_marcro_fld_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	/* Config fast link down */
	struct jl_fld_ctrl fld_cfg = {
		.enable		= JL2XXX_FLD_CTRL_EN,
		.delay		= JL2XXX_FLD_DELAY,
	};

	priv->fld = fld_cfg;

	return 0;
}

static int jl2xxx_c_marcro_wol_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_wol_ctrl wol_cfg = {
		.enable		= JL2XXX_WOL_CTRL_EN,
	};

	priv->wol = wol_cfg;

	return 0;
}

static int jl2xxx_c_marcro_intr_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_intr_ctrl intr_cfg = {
		.enable		= JL2XXX_INTR_CTRL_EN,
	};

	priv->intr = intr_cfg;

	return 0;
}

static int jl2xxx_c_marcro_downshift_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_downshift_ctrl downshift_cfg = {
		.enable		= JL2XXX_DSFT_CTRL_EN,
		.count		= JL2XXX_DSFT_AN_CNT,
	};

	priv->downshift = downshift_cfg;

	return 0;
}

static int jl2xxx_fld_operation_mode(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->fld.op;

	if (JL2XXX_FLD_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL2XXX_FLD_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL2XXX_FLD_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl2xxx_intr_operation_mode(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->intr.op;

	if (JL2XXX_INTR_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL2XXX_INTR_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL2XXX_INTR_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl2xxx_wol_operation_mode(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->wol.op;

	if (JL2XXX_WOL_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL2XXX_WOL_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL2XXX_WOL_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl2xxx_downshift_operation_mode(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->downshift.op;

	if (JL2XXX_DSFT_C_MACRO_MODE)
		mode->static_op = STATIC_C_MACRO;
	else if (JL2XXX_DSFT_DEVICE_TREE_MODE)
		mode->static_op = STATIC_DEVICE_TREE;
	else
		mode->static_op = STATIC_NONE;

	if (JL2XXX_DSFT_ETHTOOL_MODE)
		mode->dynamic_op = DYNAMIC_ETHTOOL;
	else
		mode->dynamic_op = DYNAMIC_NONE;

	return 0;
}

static int jl2xxx_fld_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->fld.op;

	if (mode->static_op == STATIC_C_MACRO)
		jl2xxx_c_marcro_fld_cfg_get(phydev);
	else if (mode->static_op == STATIC_DEVICE_TREE)
		jl2xxx_dts_fld_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->fld.enable &= ~JL2XXX_FLD_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->fld.enable |= JL2XXX_FLD_DYNAMIC_OP_EN;
	else
		priv->fld.enable &= ~JL2XXX_FLD_DYNAMIC_OP_EN;

	return 0;
}

static int jl2xxx_wol_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->wol.op;

	if (mode->static_op == STATIC_C_MACRO)
		jl2xxx_c_marcro_wol_cfg_get(phydev);
	else if (mode->static_op == STATIC_DEVICE_TREE)
		jl2xxx_dts_wol_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->wol.enable &= ~JL2XXX_WOL_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->wol.enable |= JL2XXX_WOL_DYNAMIC_OP_EN;
	else
		priv->wol.enable &= ~JL2XXX_WOL_DYNAMIC_OP_EN;

	return 0;
}

static int jl2xxx_intr_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->intr.op;

	if (mode->static_op == STATIC_DEVICE_TREE)
		jl2xxx_dts_intr_cfg_get(phydev);
	else if (mode->static_op == STATIC_C_MACRO)
		jl2xxx_c_marcro_intr_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->intr.enable &= ~JL2XXX_INTR_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->intr.enable |= JL2XXX_INTR_DYNAMIC_OP_EN;
	else
		priv->intr.enable &= ~JL2XXX_INTR_DYNAMIC_OP_EN;

	return 0;
}

static int jl2xxx_downshift_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_config_mode *mode = &priv->downshift.op;

	if (mode->static_op == STATIC_C_MACRO)
		jl2xxx_c_marcro_downshift_cfg_get(phydev);
	else if (mode->static_op == STATIC_DEVICE_TREE)
		jl2xxx_dts_downshift_cfg_get(phydev);
	else if (mode->static_op == STATIC_NONE)
		priv->downshift.enable &= ~JL2XXX_DSFT_STATIC_OP_EN;

	if (mode->dynamic_op == DYNAMIC_ETHTOOL)
		priv->downshift.enable |= JL2XXX_DSFT_DYNAMIC_OP_EN;
	else
		priv->downshift.enable &= ~JL2XXX_DSFT_DYNAMIC_OP_EN;

	return 0;
}

static int jl2xxx_fld_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	err = jl2xxx_fld_dynamic_op_set(phydev, &priv->fld.delay);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_wol_cfg_rmii(struct phy_device *phydev)
{
	int err;
	/* WOL Function should be in RMII Mode, the rmii
	 * clock direction should be output */
	err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE7,
				      JL1XXX_RMII_CTRL_REG,
				      JL1XXX_RMII_OUT,
				      JL1XXX_RMII_MODE);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_wol_clear(struct phy_device *phydev)
{
	jlsemi_set_bits(phydev, JL1XXX_PAGE129,
			JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_CLEAR);

	jlsemi_clear_bits(phydev, JL1XXX_PAGE129,
			  JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_CLEAR);

	return 0;
}

static bool jl1xxx_wol_receive_check(struct phy_device *phydev)
{
	if (jlsemi_fetch_bit(phydev, JL1XXX_PAGE129,
			     JL1XXX_WOL_CTRL_REG, JL1xxx_WOL_RECEIVE))
		return true;
	else
		return false;

}

static int jl1xxx_wol_enable(struct phy_device *phydev, bool enable)
{
	if (enable)
		jlsemi_clear_bits(phydev, JL1XXX_PAGE129,
				  JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_DIS);
	else
		jlsemi_set_bits(phydev, JL1XXX_PAGE129,
				JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_DIS);

	return 0;
}
static int jl1xxx_wol_store_mac_addr(struct phy_device *phydev)
{
	int err;

	jlsemi_write_page(phydev, JL1XXX_PAGE129);

	/* Store the device address for the magic packet */
	err = phy_write(phydev, JL1XXX_MAC_ADDR2_REG,
			((ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[0]) << 8) |
			  ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[1])));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL1XXX_MAC_ADDR1_REG,
			((ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[2]) << 8) |
			  ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[3])));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL1XXX_MAC_ADDR0_REG,
			((ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[4]) << 8) |
			  ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[5])));
	if (err < 0)
		return err;

	/* change page to 0 */
	jlsemi_write_page(phydev, JL1XXX_PAGE0);

	return 0;
}

static int jl2xxx_wol_enable(struct phy_device *phydev, bool enable)
{
	if (enable) {
		jlsemi_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EN);
		jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_CTL_EN);
	} else {
		jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EN);
	}
	jlsemi_soft_reset(phydev);

	return 0;
}

static int jl2xxx_wol_active_low_polarity(struct phy_device *phydev, bool low)
{
	if (low)
		jlsemi_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				JL2XXX_WOL_STAS_REG, JL2XXX_WOL_POLARITY);
	else
		jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_POLARITY);

	return 0;
}

static int jl2xxx_wol_clear(struct phy_device *phydev)
{
	jlsemi_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
			JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);
	jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
			  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);

	return 0;
}

static int jl2xxx_store_mac_addr(struct phy_device *phydev)
{
	int err;

	jlsemi_write_page(phydev, JL2XXX_WOL_STAS_PAGE);

	/* Store the device address for the magic packet */
	err = phy_write(phydev, JL2XXX_MAC_ADDR2_REG,
			((phydev->attached_dev->dev_addr[0] << 8) |
			  phydev->attached_dev->dev_addr[1]));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL2XXX_MAC_ADDR1_REG,
			((phydev->attached_dev->dev_addr[2] << 8) |
			  phydev->attached_dev->dev_addr[3]));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL2XXX_MAC_ADDR0_REG,
			((phydev->attached_dev->dev_addr[4] << 8) |
			  phydev->attached_dev->dev_addr[5]));
	if (err < 0)
		return err;

	/* change page to 0 */
	jlsemi_write_page(phydev, JL2XXX_BASIC_PAGE);

	return 0;
}

/* Get fast link down for jl2xxx */
int jl2xxx_fld_dynamic_op_get(struct phy_device *phydev, u8 *msecs)
{
	int ret;
	u16 val;

	jlsemi_write_page(phydev, JL2XXX_PAGE128);
	ret = phy_read(phydev, JL2XXX_FLD_CTRL_REG);
	jlsemi_write_page(phydev, JL2XXX_PAGE0);
	if (ret < 0)
		return ret;

	if (ret & JL2XXX_FLD_EN) {
		*msecs = ETHTOOL_PHY_FAST_LINK_DOWN_OFF;
		return 0;
	}

	val = (val & JL2XXX_FLD_MASK) >> JL2XXX_FLD_MASK_HEAD;

	switch (val) {
	case JL2XXX_FLD_DELAY_00MS:
		*msecs = 0;
		break;
	case JL2XXX_FLD_DELAY_10MS:
		*msecs = 10;
		break;
	case JL2XXX_FLD_DELAY_20MS:
		*msecs = 20;
		break;
	case JL2XXX_FLD_DELAY_40MS:
		*msecs = 40;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* Set fast link down for jl2xxx */
int jl2xxx_fld_dynamic_op_set(struct phy_device *phydev, const u8 *msecs)
{
	u16 val;
	int ret;

	if (*msecs == ETHTOOL_PHY_FAST_LINK_DOWN_OFF)
		return jlsemi_clear_bits(phydev, JL2XXX_PAGE128,
					 JL2XXX_FLD_CTRL_REG,
					 JL2XXX_FLD_EN);

	if (*msecs <= 5)
		val = JL2XXX_FLD_DELAY_00MS;
	else if (*msecs <= 15)
		val = JL2XXX_FLD_DELAY_10MS;
	else if (*msecs <= 30)
		val = JL2XXX_FLD_DELAY_20MS;
	else
		val = JL2XXX_FLD_DELAY_40MS;

	val = val << JL2XXX_FLD_MASK_HEAD;

	ret = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE128,
				      JL2XXX_FLD_CTRL_REG,
				      JL2XXX_FLD_MASK, val);
	if (ret < 0)
		return ret;

	ret = jlsemi_set_bits(phydev, JL2XXX_PAGE128,
			      JL2XXX_FLD_CTRL_REG,
			      JL2XXX_FLD_EN);
	if (ret < 0)
		return ret;

	return 0;
}

int jl2xxx_downshift_dynamic_op_get(struct phy_device *phydev, u8 *data)
{
	int val, cnt, enable;

	jlsemi_write_page(phydev, JL2XXX_BASIC_PAGE);
	val = phy_read(phydev, JL2XXX_DSFT_CTRL_REG);
	if (val < 0)
		return val;

	enable = val & JL2XXX_DSFT_EN;
	cnt = (val & JL2XXX_DSFT_AN_MASK) + 1;

	*data = enable ? cnt : DOWNSHIFT_DEV_DISABLE;

	return 0;
}

int jl2xxx_downshift_dynamic_op_set(struct phy_device *phydev, u8 cnt)
{
	int val, err;

	if (cnt > JL2XXX_DSFT_CNT_MAX)
		return -E2BIG;

	if (!cnt) {
		err = jlsemi_clear_bits(phydev, JL2XXX_BASIC_PAGE,
					JL2XXX_DSFT_CTRL_REG,
					JL2XXX_DSFT_EN);
	} else {
		val = ((cnt - 1) & JL2XXX_DSFT_AN_MASK) | JL2XXX_DSFT_EN |
			JL2XXX_DSFT_SMART_EN | JL2XXX_DSFT_TWO_WIRE_EN |
			JL2XXX_DSFT_STL_CNT(12);
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_BASIC_PAGE,
					      JL2XXX_DSFT_CTRL_REG,
					      JL2XXX_DSFT_AN_MASK, val);
	}

	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_downshift_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	err = jl2xxx_downshift_dynamic_op_set(phydev,
					      priv->downshift.count);
	if (err < 0)
		return err;

	return 0;
}

int jl1xxx_wol_dynamic_op_get(struct phy_device *phydev)
{
	return jlsemi_fetch_bit(phydev, JL1XXX_PAGE129,
				JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_DIS);
}

int jl2xxx_wol_dynamic_op_get(struct phy_device *phydev)
{
	return jlsemi_fetch_bit(phydev, JL2XXX_WOL_CTL_PAGE,
				JL2XXX_WOL_CTL_REG, JL2XXX_WOL_EN);
}

static int jl1xxx_wol_static_op_set(struct phy_device *phydev)
{
	int err;

	err = jl1xxx_wol_dynamic_op_set(phydev);
	if (err < 0)
		return err;

	return 0;
}

int jl1xxx_intr_ack_event(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
		if (priv->intr.enable & JL1XXX_INTR_STATIC_OP_EN) {
			err = phy_read(phydev, JL1XXX_INTR_STATUS_REG);
			if (err < 0)
				return err;
		}

	return 0;
}

int jl1xxx_intr_static_op_set(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;
	int ret = 0;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		if (priv->intr.enable & JL1XXX_INTR_LINK_CHANGE_EN)
			ret |= JL1XXX_INTR_LINK;
		if (priv->intr.enable & JL1XXX_INTR_AN_ERR_EN)
			ret |= JL1XXX_INTR_AN_ERR;

		err = jlsemi_set_bits(phydev, JL1XXX_PAGE7,
				      JL1XXX_INTR_REG, ret);
		if (err < 0)
			return err;
	} else {
		if (priv->intr.enable & JL1XXX_INTR_LINK_CHANGE_EN)
			ret |= JL1XXX_INTR_LINK;

		if (priv->intr.enable & JL1XXX_INTR_AN_ERR_EN)
			ret |= JL1XXX_INTR_AN_ERR;

		err = jlsemi_clear_bits(phydev, JL1XXX_PAGE7,
					JL1XXX_INTR_REG, ret);
		if (err < 0)
			return err;

	}

	return 0;
}

static int jl2xxx_wol_static_op_set(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_wol_dynamic_op_set(phydev);
	if (err < 0)
		return err;

	return 0;
}

int jl1xxx_wol_dynamic_op_set(struct phy_device *phydev)
{
	int err;

	err = jl1xxx_wol_cfg_rmii(phydev);
	if (err < 0)
		return err;

	err = jl1xxx_wol_enable(phydev, true);
	if (err < 0)
		return err;

	err = jl1xxx_wol_store_mac_addr(phydev);
	if (err < 0)
		return err;

	if (jl1xxx_wol_receive_check(phydev)) {
		err = jl1xxx_wol_clear(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl2xxx_wol_dynamic_op_set(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_wol_enable(phydev, true);
	if (err < 0)
		return err;

	err = jl2xxx_wol_clear(phydev);
	if (err < 0)
		return err;

	err = jl2xxx_wol_active_low_polarity(phydev, true);
	if (err < 0)
		return err;

	err = jl2xxx_store_mac_addr(phydev);
	if (err < 0)
		return err;

	return 0;
}
int jl2xxx_intr_ack_event(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		jlsemi_write_page(phydev, JL2XXX_PAGE2627);
		err = phy_read(phydev, JL2XXX_INTR_STATUS_REG);
		jlsemi_write_page(phydev, JL2XXX_PAGE0);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl2xxx_intr_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;
	int ret = 0;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		if (priv->intr.enable & JL2XXX_INTR_LINK_CHANGE_EN)
			ret |= JL2XXX_INTR_LINK_CHANGE;
		if (priv->intr.enable & JL2XXX_INTR_AN_ERR_EN)
			ret |= JL2XXX_INTR_AN_ERR;
		if (priv->intr.enable & JL2XXX_INTR_AN_COMPLETE_EN)
			ret |= JL2XXX_INTR_AN_COMPLETE;
		if (priv->intr.enable & JL2XXX_INTR_AN_PAGE_RECE)
			ret |= JL2XXX_INTR_AN_PAGE;

		err = jlsemi_set_bits(phydev, JL2XXX_PAGE2626,
				      JL2XXX_INTR_CTL_REG, ret);
		if (err < 0)
			return err;

		err = jlsemi_set_bits(phydev, JL2XXX_PAGE158,
				      JL2XXX_INTR_PIN_REG,
				      JL2XXX_INTR_PIN_EN);
		if (err < 0)
			return err;

		err = jlsemi_set_bits(phydev, JL2XXX_PAGE160,
				      JL2XXX_PIN_EN_REG,
				      JL2XXX_PIN_OUTPUT);
		if (err < 0)
			return err;
	} else {
		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE2626,
					JL2XXX_INTR_CTL_REG,
					JL2XXX_INTR_LINK_CHANGE |
					JL2XXX_INTR_AN_ERR |
					JL2XXX_INTR_AN_COMPLETE |
					JL2XXX_INTR_AN_PAGE);
		if (err < 0)
			return err;

		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE158,
					JL2XXX_INTR_PIN_REG,
					JL2XXX_INTR_PIN_EN);
		if (err < 0)
			return err;

		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE160,
					JL2XXX_PIN_EN_REG,
					JL2XXX_PIN_OUTPUT);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl1xxx_operation_mode_select(struct phy_device *phydev)
{
	jl1xxx_led_operation_mode(phydev);
	jl1xxx_wol_operation_mode(phydev);
	jl1xxx_intr_operation_mode(phydev);

	return 0;
}

int jl2xxx_operation_mode_select(struct phy_device *phydev)
{
	jl2xxx_led_operation_mode(phydev);
	jl2xxx_fld_operation_mode(phydev);
	jl2xxx_wol_operation_mode(phydev);
	jl2xxx_intr_operation_mode(phydev);
	jl2xxx_downshift_operation_mode(phydev);

	return 0;
}

int jl1xxx_operation_args_get(struct phy_device *phydev)
{
	jl1xxx_led_operation_args(phydev);
	jl1xxx_wol_operation_args(phydev);
	jl1xxx_intr_operation_args(phydev);

	return 0;
}

int jl2xxx_operation_args_get(struct phy_device *phydev)
{
	jl2xxx_led_operation_args(phydev);
	jl2xxx_fld_operation_args(phydev);
	jl2xxx_wol_operation_args(phydev);
	jl2xxx_intr_operation_args(phydev);
	jl2xxx_downshift_operation_args(phydev);

	return 0;
}

int jl1xxx_static_op_init(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->led.enable & JL1XXX_LED_STATIC_OP_EN) {
		err = jl1xxx_led_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->wol.enable & JL1XXX_WOL_STATIC_OP_EN) {
		err = jl1xxx_wol_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->wol.enable & JL1XXX_INTR_STATIC_OP_EN) {
		err = jl1xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl2xxx_static_op_init(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->led.enable & JL2XXX_LED_STATIC_OP_EN) {
		err = jl2xxx_led_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->fld.enable & JL2XXX_FLD_STATIC_OP_EN) {
		err = jl2xxx_fld_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->wol.enable & JL2XXX_WOL_STATIC_OP_EN) {
		err = jl2xxx_wol_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->intr.enable & JL2XXX_INTR_STATIC_OP_EN) {
		err = jl2xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->downshift.enable & JL2XXX_DSFT_STATIC_OP_EN) {
		err = jl2xxx_downshift_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

static const uint16_t patch_version = 0xdef2;

static const uint32_t init_data[] = {
	0x1f00a0, 0x1903f3, 0x1f0012, 0x150100, 0x1f00ad, 0x100000,
	0x11e0c6, 0x1f00a0, 0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb,
	0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb, 0x1f00ad, 0x110000,
	0x120400, 0x130093, 0x140000, 0x150193, 0x160000, 0x170213,
	0x180000, 0x12040c, 0x130293, 0x140000, 0x150313, 0x160000,
	0x170393, 0x180000, 0x120418, 0x130413, 0x140000, 0x150493,
	0x160000, 0x170513, 0x180000, 0x120424, 0x130593, 0x140000,
	0x150613, 0x160000, 0x170693, 0x180000, 0x120430, 0x130713,
	0x140000, 0x150793, 0x160000, 0x171137, 0x180000, 0x12043c,
	0x13006f, 0x140060, 0x15a001, 0x160113, 0x17fd41, 0x18d026,
	0x120448, 0x13d406, 0x14d222, 0x1517b7, 0x160800, 0x17aa23,
	0x189407, 0x120454, 0x130713, 0x1430f0, 0x1567b7, 0x160800,
	0x17a423, 0x1846e7, 0x120460, 0x13a703, 0x14a587, 0x156685,
	0x168f55, 0x17ac23, 0x18a4e7, 0x12046c, 0x1367b9, 0x145737,
	0x150800, 0x168793, 0x17ef27, 0x182023, 0x120478, 0x1374f7,
	0x1407b7, 0x150800, 0x165bfc, 0x17d493, 0x180037, 0x120484,
	0x13f493, 0x141f04, 0x15f793, 0x1607f7, 0x178fc5, 0x18c03e,
	0x120490, 0x134702, 0x140793, 0x150210, 0x160763, 0x1700f7,
	0x180793, 0x12049c, 0x130270, 0x140c63, 0x1530f7, 0x16a001,
	0x1707b7, 0x180002, 0x1204a8, 0x138793, 0x146967, 0x15c83e,
	0x1617b7, 0x170002, 0x188793, 0x1204b4, 0x13e567, 0x14c43e,
	0x1537b7, 0x160002, 0x178793, 0x186867, 0x1204c0, 0x13c23e,
	0x1447b7, 0x150002, 0x168793, 0x17e9a7, 0x1866b7, 0x1204cc,
	0x130800, 0x14ca3e, 0x15a783, 0x166d86, 0x1775c1, 0x188713,
	0x1204d8, 0x130ff5, 0x148ff9, 0x156735, 0x160713, 0x178007,
	0x188fd9, 0x1204e4, 0x13ac23, 0x146cf6, 0x15a783, 0x1665c6,
	0x175737, 0x180800, 0x1204f0, 0x136611, 0x14f793, 0x15f0f7,
	0x16e793, 0x170807, 0x18ae23, 0x1204fc, 0x1364f6, 0x142783,
	0x155c47, 0x169bf5, 0x172223, 0x185cf7, 0x120508, 0x13a703,
	0x14f5c6, 0x158f51, 0x16ae23, 0x17f4e6, 0x180737, 0x120514,
	0x130809, 0x14433c, 0x158fd1, 0x16c33c, 0x170637, 0x180800,
	0x120520, 0x134a74, 0x14679d, 0x158793, 0x160e07, 0x179ae1,
	0x18e693, 0x12052c, 0x130036, 0x14ca74, 0x154678, 0x1676e1,
	0x178693, 0x185006, 0x120538, 0x138ff9, 0x148fd5, 0x1507c2,
	0x168f6d, 0x1783c1, 0x188fd9, 0x120544, 0x13c67c, 0x140713,
	0x151000, 0x160793, 0x170000, 0x189c23, 0x120550, 0x1324e7,
	0x140713, 0x151010, 0x169123, 0x1726e7, 0x18470d, 0x12055c,
	0x13c63a, 0x144702, 0x158d23, 0x162407, 0x17a223, 0x182607,
	0x120568, 0x130793, 0x140270, 0x150413, 0x160000, 0x171463,
	0x1800f7, 0x120574, 0x134789, 0x14c63e, 0x154709, 0x16cc3a,
	0x174702, 0x180793, 0x120580, 0x130270, 0x141463, 0x1500f7,
	0x16478d, 0x17cc3e, 0x180513, 0x12058c, 0x130000, 0x144792,
	0x154581, 0x164485, 0x179782, 0x184018, 0x120598, 0x131775,
	0x14e563, 0x1502e4, 0x162703, 0x170a04, 0x181163, 0x1205a4,
	0x130297, 0x144818, 0x150563, 0x160097, 0x1747a2, 0x18c804,
	0x1205b0, 0x139782, 0x1466b7, 0x150800, 0x16a703, 0x174c46,
	0x189b71, 0x1205bc, 0x136713, 0x140027, 0x15a223, 0x164ce6,
	0x174783, 0x180fd4, 0x1205c8, 0x13c7b9, 0x142683, 0x151004,
	0x164745, 0x179763, 0x1820e6, 0x1205d4, 0x133737, 0x140822,
	0x152683, 0x163007, 0x177645, 0x18167d, 0x1205e0, 0x138ef1,
	0x142023, 0x1530d7, 0x162683, 0x172807, 0x18e693, 0x1205ec,
	0x131006, 0x142023, 0x1528d7, 0x162683, 0x173807, 0x18e693,
	0x1205f8, 0x131006, 0x142023, 0x1538d7, 0x162683, 0x174007,
	0x18e693, 0x120604, 0x131006, 0x142023, 0x1540d7, 0x162683,
	0x174807, 0x18e693, 0x120610, 0x131006, 0x142023, 0x1548d7,
	0x1656b7, 0x170800, 0x18a703, 0x12061c, 0x133486, 0x14830d,
	0x158b05, 0x16cf01, 0x17a703, 0x185c46, 0x120628, 0x137671,
	0x14167d, 0x158f71, 0x166611, 0x17a223, 0x185ce6, 0x120634,
	0x138f51, 0x14a223, 0x155ce6, 0x162703, 0x171084, 0x1846b2,
	0x120640, 0x131c63, 0x1402d7, 0x153737, 0x160822, 0x172683,
	0x182807, 0x12064c, 0x13e693, 0x140016, 0x152023, 0x1628d7,
	0x172683, 0x183807, 0x120658, 0x13e693, 0x140016, 0x152023,
	0x1638d7, 0x172683, 0x184007, 0x120664, 0x13e693, 0x140016,
	0x152023, 0x1640d7, 0x172683, 0x184807, 0x120670, 0x13e693,
	0x140016, 0x152023, 0x1648d7, 0x172703, 0x181004, 0x12067c,
	0x1346b2, 0x149c63, 0x151ae6, 0x160737, 0x170800, 0x184b78,
	0x120688, 0x130693, 0x140ff0, 0x15463d, 0x168b1d, 0x17ce3a,
	0x1852b7, 0x120694, 0x130800, 0x144701, 0x154389, 0x16408d,
	0x174311, 0x180537, 0x1206a0, 0x130820, 0x141593, 0x150077,
	0x1695aa, 0x17418c, 0x184572, 0x1206ac, 0x1305c2, 0x1481c1,
	0x1581a9, 0x167763, 0x1700b5, 0x189533, 0x1206b8, 0x1300e4,
	0x144513, 0x15fff5, 0x168e69, 0x170537, 0x180800, 0x1206c4,
	0x134568, 0x148121, 0x15893d, 0x167463, 0x1702b5, 0x18a583,
	0x1206d0, 0x1306c2, 0x140763, 0x151277, 0x160a63, 0x171217,
	0x1805c2, 0x1206dc, 0x1381c1, 0x14818d, 0x150d63, 0x161097,
	0x178985, 0x180586, 0x1206e8, 0x1395b3, 0x1400b4, 0x15c593,
	0x16fff5, 0x178eed, 0x180705, 0x1206f4, 0x1315e3, 0x14fa67,
	0x1535b7, 0x160822, 0x17a703, 0x183005, 0x120700, 0x13757d,
	0x148a3d, 0x150513, 0x160ff5, 0x178f69, 0x180622, 0x12070c,
	0x138e59, 0x14a023, 0x1530c5, 0x168637, 0x170800, 0x185a38,
	0x120718, 0x1375c1, 0x14f693, 0x150ff6, 0x168593, 0x170ff5,
	0x188f6d, 0x120724, 0x1306a2, 0x148ed9, 0x15da34, 0x164682,
	0x170713, 0x180210, 0x120730, 0x139163, 0x140ee6, 0x154711,
	0x16e391, 0x17471d, 0x182023, 0x12073c, 0x1310e4, 0x142683,
	0x150a04, 0x16471d, 0x179e63, 0x1800e6, 0x120748, 0x136737,
	0x140800, 0x152703, 0x164cc7, 0x170693, 0x184000, 0x120754,
	0x137713, 0x144807, 0x151463, 0x1600d7, 0x172223, 0x180e04,
	0x120760, 0x134018, 0x141163, 0x150497, 0x165703, 0x1700c4,
	0x181793, 0x12076c, 0x130117, 0x14db63, 0x150207, 0x168737,
	0x170800, 0x184778, 0x120778, 0x137713, 0x140807, 0x15e705,
	0x160513, 0x170000, 0x184792, 0x120784, 0x134581, 0x149782,
	0x1547a2, 0x164711, 0x17c818, 0x18c004, 0x120790, 0x130d23,
	0x140094, 0x150ca3, 0x160004, 0x179782, 0x1856b7, 0x12079c,
	0x130800, 0x1442b8, 0x159b71, 0x16c2b8, 0x170513, 0x180000,
	0x1207a8, 0x1347d2, 0x149782, 0x154703, 0x162684, 0x1703e3,
	0x18de07, 0x1207b4, 0x13bbd9, 0x1407b7, 0x150002, 0x168793,
	0x1765c7, 0x18c83e, 0x1207c0, 0x1327b7, 0x140002, 0x158793,
	0x16dae7, 0x17c43e, 0x1847b7, 0x1207cc, 0x130002, 0x148793,
	0x151427, 0x16c23e, 0x1757b7, 0x180002, 0x1207d8, 0x138793,
	0x149867, 0x15b1fd, 0x162683, 0x171504, 0x184709, 0x1207e4,
	0x1399e3, 0x14e2e6, 0x1536b7, 0x160822, 0x17a703, 0x183006,
	0x1207f0, 0x13663d, 0x148f51, 0x15a023, 0x1630e6, 0x17bd39,
	0x18c593, 0x1207fc, 0x130015, 0x14b5dd, 0x158991, 0x1635b3,
	0x1700b0, 0x180589, 0x120808, 0x13bdf9, 0x148991, 0x15b593,
	0x160015, 0x17bfdd, 0x180737, 0x120814, 0x130800, 0x144f28,
	0x15cf89, 0x1647c2, 0x17893d, 0x189782, 0x120820, 0x1347e2,
	0x140713, 0x151000, 0x162223, 0x1710e4, 0x182423, 0x12082c,
	0x1310f4, 0x14474d, 0x15b729, 0x168111, 0x17b7dd, 0x1814e3,
	0x120838, 0x13f097, 0x140737, 0x150800, 0x164770, 0x171713,
	0x180106, 0x120844, 0x135d63, 0x140607, 0x1585b7, 0x160800,
	0x17a683, 0x180d05, 0x120850, 0x1372c5, 0x147313, 0x1500f6,
	0x1612fd, 0x17a703, 0x180d45, 0x12085c, 0x131513, 0x1400c3,
	0x15f6b3, 0x160056, 0x178ec9, 0x18757d, 0x120868, 0x130393,
	0x140ff5, 0x151293, 0x160083, 0x17f6b3, 0x180076, 0x120874,
	0x139b41, 0x148211, 0x15e2b3, 0x160056, 0x171093, 0x180043,
	0x120880, 0x137693, 0x140016, 0x156333, 0x160067, 0x170613,
	0x187ff5, 0x12088c, 0x139713, 0x1400b6, 0x157633, 0x1600c3,
	0x178e59, 0x189513, 0x120898, 0x1300a6, 0x147613, 0x159ff6,
	0x169713, 0x170096, 0x188e49, 0x1208a4, 0x13f293, 0x14f0f2,
	0x158e59, 0x16e2b3, 0x170012, 0x1806a2, 0x1208b0, 0x137613,
	0x14eff6, 0x158e55, 0x16a823, 0x170c55, 0x18aa23, 0x1208bc,
	0x130cc5, 0x1480e3, 0x15e807, 0x1646b7, 0x170822, 0x18a703,
	0x1208c8, 0x13f006, 0x149b61, 0x156713, 0x160027, 0x17a023,
	0x18f0e6, 0x1208d4, 0x13b5ad, 0x140000, 0x150000, 0x160000,
	0x170000, 0x180000, 0x110000, 0x120400, 0x104000, 0x1f0000,
};

int jl2xxx_pre_init(struct phy_device *phydev)
{
	int i, j;
	int regaddr, val;
	int length = sizeof(init_data)/sizeof(init_data[0]);

	for (i = 0; i < length; i++) {
		regaddr = ((init_data[i] >> 16) & 0xff);
		val = (init_data[i] & 0xffff);
		phy_write(phydev, regaddr, val);
		if (regaddr == 0x18) {
			phy_write(phydev, 0x10, 0x8006);
			for (j = 0; j < 8; j++) {
				if (phy_read(phydev, 0x10) == 0) {
					break;
				}
			}
		}
	}
	/* Wait load patch complete */
	msleep(10);

	jlsemi_write_page(phydev, JL2XXX_PATCH);
	val = phy_read(phydev, JL2XXX_PATCH_REG);
	jlsemi_write_page(phydev, JL2XXX_BASIC_PAGE);

	if (val != patch_version)
		JLSEMI_PHY_MSG(KERN_ERR
			       "%s: patch version is not match", __func__);

	return 0;
}

int jlsemi_soft_reset(struct phy_device *phydev)
{
	int err;

	err = jlsemi_set_bits(phydev, JL2XXX_BASIC_PAGE,
			      JL2XXX_BMCR_REG, JL2XXX_SOFT_RESET);
	if (err < 0)
		return err;
	/* Wait soft reset complete*/
	msleep(600);

	return 0;
}

int jlsemi_aneg_done(struct phy_device *phydev)
{
	return 0;
}

int jl2xxx_config_phy_info(struct phy_device *phydev,
			   struct jl2xxx_priv *jl2xxx)
{
	int val, major, minor;

	val = phy_read(phydev, 29);
	if (val < 0)
		return val;

	major = (val >> 7) & 0x1f;
	minor = (val >> 0) & 0x7f;
	/* major enlarge 10 */
	jl2xxx->sw_info = major * 10 + minor;

	return 0;
}

/********************** Convenience function for phy **********************/

/**
 * jlsemi_write_page() - write the page register
 * @phydev: a pointer to a &struct phy_device
 * @page: page values
 */
int jlsemi_write_page(struct phy_device *phydev, int page)
{
	return phy_write(phydev, JLSEMI_PHY_PAGE, page);
}

/**
 * jlsemi_read_page() - write the page register
 * @phydev: a pointer to a &struct phy_device
 *
 * Return: get page values at present
 */
int jlsemi_read_page(struct phy_device *phydev)
{
	return phy_read(phydev, JLSEMI_PHY_PAGE);
}

/**
 * __jlsemi_save_page() - save the page value
 *@phydev: a pointer to a &struct phy_device
 *
 * Return: save page value
 */
static inline int __jlsemi_save_page(struct phy_device *phydev)
{
	return jlsemi_read_page(phydev);
}

/**
 * __jlsemi_select_page() - restore the page register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page
 *
 * Return:
 * @oldpgae: this is last page value
 * @ret: if page is change it will return new page value
 */
static inline int __jlsemi_select_page(struct phy_device *phydev, int page)
{
	int ret, oldpage;

	oldpage = ret = __jlsemi_save_page(phydev);
	if (ret < 0)
		return ret;

	if (oldpage != page) {
		ret = jlsemi_write_page(phydev, page);
		if (ret < 0)
			return ret;
	}

	return oldpage;
}

/**
 * __jlsemi_restore_page() - restore the page register
 * @phydev: a pointer to a &struct phy_device
 * @oldpage: the old page, return value from __jlsemi_save_page() or
 * __jlsemi_select_page()
 * @ret: operation's return code
 *
 * Returns:
 *   @oldpage if it was a negative value, otherwise
 *   @ret if it was a negative errno value, otherwise
 *   phy_write_page()'s negative value if it were in error, otherwise
 *   @ret
 */
static inline int __jlsemi_restore_page(struct phy_device *phydev,
					int oldpage, int ret)
{
	int r;

	if (oldpage >= 0) {
		r = jlsemi_write_page(phydev, oldpage);

		/* Propagate the operation return code if the page write
		 * was successful.
		 */
		if (ret >= 0 && r < 0)
			ret = r;
	} else {
		/* Propagate the phy page selection error code */
		ret = oldpage;
	}

	return ret;
}

/**
 * __jlsemi_modify_reg() - Convenience function for modifying a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @regnum: register number
 * @mask: bit mask of bits to clear
 * @set: bit mask of bits to set
 *
 * Returns negative errno, 0 if there was no change, and 1 in case of change
 */
static inline int __jlsemi_modify_reg(struct phy_device *phydev,
				      u32 regnum, u16 mask, u16 set)
{
	int newval, ret;

	ret = phy_read(phydev, regnum);
	if (ret < 0)
		return ret;

	newval = (ret & ~mask) | set;
	if (newval == ret)
		return 0;

	ret = phy_write(phydev, regnum, newval);

	return ret < 0 ? ret : 1;
}

/**
 * jlsemi_modify_paged_reg() - Function for modifying a paged register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number
 * @mask: bit mask of bits to clear
 * @set: bit mask of bits to set
 *
 * Returns negative errno, 0 if there was no change, and 1 in case of change
 */
int jlsemi_modify_paged_reg(struct phy_device *phydev,
			    int page, u32 regnum,
			    u16 mask, u16 set)
{
	int ret = 0, oldpage;

	oldpage = __jlsemi_select_page(phydev, page);
	if (oldpage >= 0)
		ret = __jlsemi_modify_reg(phydev, regnum, mask, set);

	return __jlsemi_restore_page(phydev, oldpage, ret);
}

/**
 * jlsemi_set_bits() - Convenience function for setting bits in a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bits to set
 */
int jlsemi_set_bits(struct phy_device *phydev,
		    int page, u32 regnum, u16 val)
{
	return jlsemi_modify_paged_reg(phydev, page, regnum, 0, val);
}

/**
 * jlsemi_clear_bits - Convenience function for clearing bits in a PHY register
 * @phydev: the phy_device struct
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bits to clear
 */
int jlsemi_clear_bits(struct phy_device *phydev,
		      int page, u32 regnum, u16 val)
{
	return jlsemi_modify_paged_reg(phydev, page, regnum, val, 0);
}

/**
 * jlsemi_fetch_bit() - Convenience function for setting bits in a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bit to get
 *
 * Note:
 * you only get one bit at meanwhile
 *
 */
int jlsemi_fetch_bit(struct phy_device *phydev,
		   int page, u32 regnum, u16 val)
{
	int ret = 0, oldpage;

	oldpage = __jlsemi_select_page(phydev, page);
	if (oldpage >= 0)
	{
		ret = phy_read(phydev, regnum);
		if (ret < 0)
			return ret;
		ret = ((ret & val) == val) ? 1 : 0;
	}

	return __jlsemi_restore_page(phydev, oldpage, ret);
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))
int jlsemi_drivers_register(struct phy_driver *phydrvs, int size)
{
	int i, j;
	int ret;

	for (i = 0; i < size; i++) {
		ret = phy_driver_register(&phydrvs[i]);
		if (ret)
			goto err;
	}

	return 0;

err:
	for (j = 0; j < i; j++)
		phy_driver_unregister(&phydrvs[j]);

	return ret;
}

void jlsemi_drivers_unregister(struct phy_driver *phydrvs, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		phy_driver_unregister(&phydrvs[i]);
	}
}
#else
#endif

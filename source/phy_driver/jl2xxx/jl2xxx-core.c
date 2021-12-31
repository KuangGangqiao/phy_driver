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
#include "jl2xxx-core.h"
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>

/************************* Configuration section *************************/


/************************* JLSemi iteration code *************************/

static inline int __reset_rgmii_tx_sw(struct phy_device *phydev)
{
	jl2xxx_clear_bits(phydev, JL2XXX_RGMII_CTRL_PAGE,
			  JL2XXX_RGMII_CTRL_REG, JL2XXX_RGMII_TX_RSTN);
	jl2xxx_set_bits(phydev, JL2XXX_RGMII_CTRL_PAGE,
			JL2XXX_RGMII_CTRL_REG, JL2XXX_RGMII_TX_RSTN);

	return 0;
}

static inline  int __get_rgmii_tx_err_status(struct phy_device *phydev)
{
	return jl2xxx_get_bit(phydev, JL2XXX_RGMII_STUS_PAGE,
			      JL2XXX_RGMII_STUS_REG, JL2XXX_RGMII_ERR_STAS);
}

static inline int __setup_bmcr(struct phy_device *phydev)
{
	return jl2xxx_clear_bits(phydev, JL2XXX_BASIC_PAGE, JL2XXX_BMCR_REG,
				 JL2XXX_SPEED_LSB | JL2XXX_SPEED_MSB |
				 JL2XXX_AUTONEG_EN);
}

static inline int __setup_clk(struct phy_device *phydev)
{
	return jl2xxx_set_bits(phydev, JL2XXX_DIG_PAGE,
			       JL2XXX_DIG_REG, JL2XXX_CLK_10M_EN);
}

static inline int __setup_dac_out(struct phy_device *phydev)
{
	return jl2xxx_modify_paged_reg(phydev, JL2XXX_DIG_PAGE, JL2XXX_DIG_REG,
				       JL2XXX_DAC_LSB, JL2XXX_DAC_MSB);
}

static inline int __close_clk(struct phy_device *phydev)
{
	return jl2xxx_clear_bits(phydev, JL2XXX_DIG_PAGE,
				 JL2XXX_DIG_REG, JL2XXX_CLK_10M_EN);
}

static inline int __soft_reset(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_set_bits(phydev, JL2XXX_BASIC_PAGE,
			      JL2XXX_BMCR_REG, JL2XXX_SOFT_RESET);
	if (err < 0)
		return err;
	msleep(600);

	return 0;
}

int jl2xxx_check_rgmii(struct phy_device *phydev)
{
	int err;

	err = __get_rgmii_tx_err_status(phydev);
	if (err)
		__reset_rgmii_tx_sw(phydev);

	return 0;
}

int jl2xxx_dis_rgmii_tx(struct phy_device *phydev)
{
	return jl2xxx_clear_bits(phydev, JL2XXX_RGMII_CTRL_PAGE,
				 JL2XXX_RGMII_CTRL_REG, JL2XXX_RGMII_TX_EN);
}


int jl2xxx_config_suspend(struct phy_device *phydev)
{
	__setup_bmcr(phydev);
	msleep(1);
	__setup_clk(phydev);
	msleep(1);
	__setup_dac_out(phydev);

	return 0;
}

int jl2xxx_config_resume(struct phy_device *phydev)
{
	__close_clk(phydev);
	__soft_reset(phydev);

	return 0;

}

int jl2xxx_enable_wol(struct phy_device *phydev, bool enable)
{
	if (enable) {
		jl2xxx_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EN);
		jl2xxx_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_CTL_EN);
	}
	else {
		jl2xxx_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EN);
	}
	__soft_reset(phydev);

	return 0;
}

int jl2xxx_setup_wol_high_polarity(struct phy_device *phydev, bool high)
{
	if (high)
		jl2xxx_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				JL2XXX_WOL_STAS_REG, JL2XXX_WOL_POLARITY);
	else
		jl2xxx_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_POLARITY);

	return 0;
}

int jl2xxx_clear_wol_event(struct phy_device *phydev)
{
	jl2xxx_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
			JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);
	jl2xxx_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
			JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);

	return 0;
}

int jl2xxx_store_mac_addr(struct phy_device *phydev)
{
	int err;

	jl2xxx_write_page(phydev, JL2XXX_WOL_STAS_PAGE);

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
	jl2xxx_write_page(phydev, JL2XXX_BASIC_PAGE);

	return 0;
}

int jl2xxx_config_phy_info(struct phy_device *phydev,
		    struct jl2xx1_priv *jl2xx1)
{
	int val, major, minor;

	val = phy_read(phydev, 29);
	if (val < 0)
		return val;

	major = (val >> 7) & 0x1f;
	minor = (val >> 0) & 0x7f;
	/* major enlarge 10 */
	jl2xx1->sw_info = major * 10 + minor;

	return 0;
}

/********************** Convenience function for phy **********************/

/**
 * jl2xxx_write_page() - write the page register
 * @phydev: a pointer to a &struct phy_device
 * @page: page values
 */
int jl2xxx_write_page(struct phy_device *phydev, int page)
{
	return phy_write(phydev, JL2XXX_PHY_PAGE, page);
}

/**
 * jl2xxx_read_page() - write the page register
 * @phydev: a pointer to a &struct phy_device
 *
 * Return: get page values at present
 */
int jl2xxx_read_page(struct phy_device *phydev)
{
	return phy_read(phydev, JL2XXX_PHY_PAGE);
}

/**
 * __jl2xxx_save_page() - save the page value
 *@phydev: a pointer to a &struct phy_device
 *
 * Return: save page value
 */
static inline int __jl2xxx_save_page(struct phy_device *phydev)
{
	return jl2xxx_read_page(phydev);
}

/**
 * __jl2xxx_select_page() - restore the page register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page
 *
 * Return:
 * @oldpgae: this is last page value
 * @ret: if page is change it will return new page value
 */
static inline int __jl2xxx_select_page(struct phy_device *phydev, int page)
{
	int ret, oldpage;

	oldpage = ret = __jl2xxx_save_page(phydev);
	if (ret < 0)
		return ret;

	if (oldpage != page) {
		ret = jl2xxx_write_page(phydev, page);
		if (ret < 0)
			return ret;
	}

	return oldpage;
}

/**
 * __jl2xxx_restore_page() - restore the page register
 * @phydev: a pointer to a &struct phy_device
 * @oldpage: the old page, return value from __jl2xxx_save_page() or
 * __jl2xxx_select_page()
 * @ret: operation's return code
 *
 * Returns:
 *   @oldpage if it was a negative value, otherwise
 *   @ret if it was a negative errno value, otherwise
 *   phy_write_page()'s negative value if it were in error, otherwise
 *   @ret
 */
static inline int __jl2xxx_restore_page(struct phy_device *phydev,
					int oldpage, int ret)
{
	int r;

	if (oldpage >= 0) {
		r = jl2xxx_write_page(phydev, oldpage);

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
 * __jl2xxx_modify_reg() - Convenience function for modifying a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @regnum: register number
 * @mask: bit mask of bits to clear
 * @set: bit mask of bits to set
 *
 * Returns negative errno, 0 if there was no change, and 1 in case of change
 */
static inline int __jl2xxx_modify_reg(struct phy_device *phydev,
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
 * jl2xxx_modify_paged_reg() - Function for modifying a paged register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number
 * @mask: bit mask of bits to clear
 * @set: bit mask of bits to set
 *
 * Returns negative errno, 0 if there was no change, and 1 in case of change
 */
int jl2xxx_modify_paged_reg(struct phy_device *phydev,
			    int page, u32 regnum,
			    u16 mask, u16 set)
{
	int ret = 0, oldpage;

	oldpage = __jl2xxx_select_page(phydev, page);
	if (oldpage >= 0)
		ret = __jl2xxx_modify_reg(phydev, regnum, mask, set);

	return __jl2xxx_restore_page(phydev, oldpage, ret);
}

/**
 * jl2xxx_set_bits() - Convenience function for setting bits in a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bits to set
 */
int jl2xxx_set_bits(struct phy_device *phydev,
		    int page, u32 regnum, u16 val)
{
	return jl2xxx_modify_paged_reg(phydev, page, regnum, 0, val);
}

/**
 * jl2xxx_clear_bits - Convenience function for clearing bits in a PHY register
 * @phydev: the phy_device struct
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bits to clear
 */
int jl2xxx_clear_bits(struct phy_device *phydev,
		      int page, u32 regnum, u16 val)
{
	return jl2xxx_modify_paged_reg(phydev, page, regnum, val, 0);
}

/**
 * jl2xxx_get_bit() - Convenience function for setting bits in a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bit to get
 *
 * Note:
 * you only get one bit at meanwhile
 *
 */
int jl2xxx_get_bit(struct phy_device *phydev,
		   int page, u32 regnum, u16 val)
{
	int ret = 0, oldpage;

	oldpage = __jl2xxx_select_page(phydev, page);
	if (oldpage >= 0)
	{
		ret = phy_read(phydev, regnum);
		if (ret < 0)
			return ret;
		ret = ((ret & val) == val) ? 1 : 0;
	}

	return __jl2xxx_restore_page(phydev, oldpage, ret);
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))
int jl2xxx_drivers_register(struct phy_driver *phydrvs, int size)
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

void jl2xxx_drivers_unregister(struct phy_driver *phydrvs, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		phy_driver_unregister(&phydrvs[i]);
	}
}
#else
#endif

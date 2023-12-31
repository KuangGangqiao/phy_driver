# About Version

For version `x.y.z`, it means

- `x`: SDK version number

       Note: Only when there is a major change in the code.

- `y`: SDK major version number

       Note: According to the traditional odd even system version number,
             the odd number is the development version, and the even number
             is the stable version.

- `z`: SDK minor version number

       Note: It will change when implementing new features.


# Time: 2023 10 27
# Version: 1.2.14
**New**
- Optimize the load patch check function
**Change**
- Remove PHY_INTERFACE_MODE_SGMII redundancy options
- Update development documents

# Time: 2023 10 13
# Version: 1.2.11
**New**
- Add new patch for r3p5 (sgmii->utp) mode
- Add wol event self clear in phy suspend function
      I found that not all network cards will call jl2xxx_config_init during
      sleep and wake-up,  will cause the WOL event to be unable to be cleaned
      again, but the cleaning function will be placed in jl2xxx_suspend
      can avoid this issue

# Time: 2023 7 17
# Version: 1.2.9
**Fix**
- Fix wol enable reg addr error.
- Fix don't clear led polarity default val.
    conflicts between LED polarity driver configuration and strap pin
**New**
- Add new function for close 125MHz and 25MHz clock out

# Time: 2023 5 29
# Version: 1.2.6
**New**
- add new patch version 0x2e9c for r4p1

# Time: 2023 5 19
# Version: 1.2.5
**Fix**
- Fix downshift config val error

# Time: 2023 5 19
# Version: 1.2.4
**Fix**
- Fix jl1xxx intrrupt function typo
- Fix judgment condition omission JL2XXX_UTP_FIBER_RGMII_MODE
- Fix  add phy interface aneg_done for phy fiber mode
    Avoiding automatic negotiation failure in phy fiber mode

**New**
- Add patch of 0x9f73 for separate load big die and small die phy


# Time: 2023 3 20
# Version: 1.2.0
**New**
- Reconstructed the select mode function code
- Support Device Tree to configure different phy functions based on
  different phy addrs
- Support 100M phy driver

# Time: 2023 3 15
# Version: 1.1.15
**Fix**
- Fix rx clock out operation mode
**New**
- Add hardcode for phy version


# Time: 2022 12 21
# Version: 1.1.13
**New**
- Add rgmii cfg by pass for config init r4p1


# Time: 2022 11 30
# Version: 1.1.12
**New**
- Add fake power down function for jl2xxx
  The rxc clock disappears when the amlogic board sleeps

**Fix**
- Fix typo for JL2XXX -> JL1XXX
- Fix auto-negotiation for jl1xxx

**Change**
- Improve the compatibility between phy api wiht kernel
  we promise that the phy driver support kernel version is greater than 3.0,
  so we need to improve the flexible performance of the phy api


# Time: 2022 11 3
# Version: 1.1.8
**Fix**
- Fix code compatible with kernel version(version>3.0)
  we need to compatibel with kernel, so Decouple some kernel apis
- Fix typo for marcro->macro

**Change**
- Change the ethtool workflow of fiber mode
  phy fiber only support (1000M/full AN),(1000M/full FORCE),(100M/full FORCE),
  (100M/half FORCE), the configuration mechanism of ethtool for phy has been
  changed. Automatic negotiation only takes effect at 1000M, while other
  automatic negotiation modes do not take effect and will remain at 1000M
  such as:
         ---------
        | ethtool |
         ---------
            |
            | 100M/full|half AN or 10M/full|half AN
            | (no effect)
            V
         -------
        |  phy  | (keep 1000M/full AN)
         -------

# Time: 2022 10 25
# Version: 1.1.5
**New**
- Support fiber function for jl2xxx.
  We needs refactor work flow of phy driver when config phy to
  rgmii<-->fiber mode.
- Support Config init r4p1
  It is need to config init some register witch phy of version r4p1.
- Add Wake On Lan tool for phy test
  It is test Phy function of Wake On Lan very easy to be used and convenient.
- support check code style function of sdk
  I am add a check code script for when we to check code style
  commit code at before


**Change**
- Update mdio-tool read and write api
  It is needn't to change phy page by manual, we can to use easily

# Time: 2022 09 15
# Version: 1.0.7
**Fix**
- Fix phydev death lock issue. Solve the problem by removing mutex lock,
  for example mutex_lock(&phydev->lock) On read_staus function call
  jlsemi_set_bits() api will cause lock conflict.
- Fix typo for fast link down function

**New**
- Replace device tree api. of_find_node_by_path()-->of_find_compatible_node()
  the new device tree api can add phy configuration files anywhere in the device
  tree file.
- Formate code style by linux script checkpatch.pl

# Time: 2022 07 30
# Version: 1.0.3
**Fix**
- Fix Mac don't attach phy(err = -19) This bug was never encountered on the
  rk3328 board, so it was not tested when it was released. The reason for this
  bug is that we failed to attach our PHY using stmmac driver. This bug is
  caused by my overwriting the data of the system (device of node)
  in the PHY driver.

**New**
- Add mutex lock to driver read and write function
- Format some code style

# Time: 2022 06 29
# Version: 1.0.0

**New**
- Init phy driver SDK repo
- Add jl2xxx and jl1xxx phy driver code
  for jl1xxx:
      -led function
      -wake on lan function
      -interrupt function
      -mdi function
      -rmii function
  for jl2xxx:
      -led function
      -fast link down function
      -wake on lan function
      -interrupt function
      -rgmii function
      -clock function
      -patch function
      -work mode function
      -loopback function
      -slew rate(deglitch) function
      -downshift function
- Support three mode to operation of phy driver
  for operation:
      -c macro static mode
      -device tree static mode
      -ethtool dynamic mode

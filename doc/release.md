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

**Fix**
- Fix typo for JL2XXX -> JL1XXX
- Fix auto-negotiation for jl1xxx

**Change**
- Improve the compatibility between phy api wiht kernel


# Time: 2022 11 3
# Version: 1.1.8
**Fix**
- Fix code compatible with kernel version(version>3.0)
- Fix typo for marcro->macro

**Change**
- Change the ethtool workflow of fiber mode


# Time: 2022 10 25
# Version: 1.1.5
**New**
- Support fiber function for jl2xxx
- Support Config init r4p1
- Add Wake On Lan tool for phy test
- support check code style function of sdk

**Change**
- Update mdio-tool read and write api


# Time: 2022 09 15
# Version: 1.0.7
**Fix**
- Fix phydev death lock issue.
- Fix typo for fast link down function.

**New**
- Replace device tree api.
- Formate code style by linux script checkpatch.pl


# Time: 2022 07 30
# Version: 1.0.3

**Fix**
- Fix Mac don't attach phy(err = -19)

**New**
- Add mutex lock to driver read and write function
- Format some code style

# Time: 2022 06 29
# Version: 1.0.0

**New**
- Init phy driver SDK repo
- Add jl2xxx and jl1xxx phy driver code
- Support three mode to operation of phy driver

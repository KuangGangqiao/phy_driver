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

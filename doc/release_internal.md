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

# Time: 2022 07 30
# Version: 1.0.3
**Fix**
- Fix Mac don't attach phy(err = -19) This bug was never encountered on the
  rk3328 board, so it was not tested when it was released. The reason for this
  bug is that we failed to attach our PHY using stmmac driver. This bug is
  caused by my overwriting the data of the system (device of node)
  in the PHY driver.

**New**
- Add macro for device tree to be fond
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

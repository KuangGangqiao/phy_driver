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
      -deglitch function
      -downshift function
- Support three mode to operation of phy driver
  for operation:
      -c macro static mode
      -device tree static mode
      -ethtool dynamic mode

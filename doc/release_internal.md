# About Version

For version `x.y`, it means

- `x`: SDK major version
- `y`: SDK minor version

#Time: 2022 06 29

#Version: 1.0

**New**
- Init phy driver SDK repo
- Add jl2xxx and jl1xxx phy driver code
  for jl1xxx:
      -led function
      -wake on lan function
      -interrupt function
      -mdi funcrion
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

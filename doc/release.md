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

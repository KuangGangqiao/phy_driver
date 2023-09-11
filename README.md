* Introduction

  This software is a phy drive SDK based on the kernel standard phy device
  implementation, supports dynamic operation of ethtool and ifconfig tools,
  as well as static configuration of device tree and C MACRO.
  The goal is to provide a complete phy customer support solution.


* Features

  - Support for ethtool dynamic configuration and (device tree, c macro)
    static configuration

  - Supports automatic cross compilation and manual compilation of phy drivers

  - Support automatic packaging and publishing, check code style, and provide
    multiple phy testing tools

* Getting the Code
    git clone ssh://git@dev.jlsemi.cn:2200/gqkuang/jlsemi_ephy_kernel_phys.git


## How to Use

### Manual Compilation

For details, please refer to Jlsemi_Phy_Driver_Quick_Start_x.x.x_CN.pdf

```shell
    # 1.Packaging code for release
        make pack
```

### Automatic Compilation

User should assign linux root path and driver aim to balal...

You can update those variables in `${repo}/config.mk`

1.ARCH: the arch options for gcc compilation include arm, arm64, x86, and more
    such as: ARCH=arm64

2.CROSS_COMPILE: cross compiled by gcc_ Compile option, cross compile toolchain
    such as: CROSS_COMPILE=aarch64-linux-gnu-

3.KERNEL_DIR: cross compiled kernel path
    such as: KERNEL_DIR=~/kernel/linux-5.4.10

4.KERNEL_DECONFIG: the default configuration file selected for kernel compilation
    such as: KERNEL_DECONFIG=xxx_deconfig

5.BUILD_MODE: the select to build to .ko or in kernel .img
    such as: BUILD_MODE=m or y

6.DRIVER_LIST: compiled List Object
    such as: DRIVER_LIST=jlsemi_phy

7.OBJ_MODULE: Compiled intermediate objects
    such as: OBJ_MODULE=jlsemi_phy

8.OBJ_CODE: Compiled Code Objects
    such as: OBJ_CODE=jlsemi.o jlsemi-core.o


```makefile
# Packaging code for release
make pack

# Check the code according to the kernel code style
make check_code_style

# Initialize code before cross compiling the kernel
make config_kernel

# Cross Compilation Module .ko file
make build_out_module

# Cross compile into kernel
make build_in_kernel

# clean all object
make clean
```

```shell
cmd step:
    # 1.Cross Compilation Module .ko file
        make config_kernel
        make build_out_module
    # 2.Cross compile into kernel
        make config_kernel
        make build_in_kernel
```

## Release flow
Recommend using manual compilation to package code
1. Tag: distinguish version numbers
2. doc: update document
3. email: email release notification project team
4. pack: package the code and upload it to cloud disk

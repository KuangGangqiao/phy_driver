ARCH = arm64

CROSS_COMPILE = aarch64-linux-gnu-

KERNEL_DIR = ~/linux-source-5.4.0

KERNEL_DECONFIG = xxx_deconfig

# m, y, n
BUILD_MODE = m

DRIVER_LIST = jl2xxx

OBJ_MODULE = jlsemi

OBJ_CODE = jl2xxx.o jl2xxx-core.o

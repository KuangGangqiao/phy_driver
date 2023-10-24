ARCH = arm64

CROSS_COMPILE = aarch64-linux-gnu-

KERNEL_DIR = ~/kernel/linux-5.4.10

KERNEL_DECONFIG = xxx_deconfig

# m, y, n
BUILD_MODE = m

DRIVER_LIST = jlsemi

OBJ_MODULE = jlsemiphy

OBJ_CODE = jlsemi.o jlsemi-core.o

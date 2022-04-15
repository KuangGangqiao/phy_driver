include $(PWD)/config.mk

PWD := $(shell pwd)

SUB_DIR := $(PWD)/source/phy_driver/$(DRIVER_LIST)
export SUB_DIR

BUILD := ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
export BUILD

export KERNEL_DIR

KCFG = config JLSEMI_PHY\n\
	\t tristate \"JLSEMI PHYs\"\n\
	\t ---help---\n\
	\t Currently has a driver for the $(OBJ_MODULE)
DOLLAR = \$
MFILE = $(OBJ_MODULE)-objs \t:= $(OBJ_CODE)\
	\nobj-$(DOLLAR)(CONFIG_JLSEMI_PHY)\
	\t+= $(OBJ_MODULE).o
CFG = CONFIG_JLSEMI_PHY=$(BUILD_MODE)

config_kernel:	
	$(call config_kernel_gnu)
	$(call cross_compile_kernel)

build_out_module:
	$(call autocof)
	$(call build_driver)

build_in_kernel:
	$(call copy_code_to_kernel)
	$(call setup_Kconfg)
	$(call setup_Makefile)
	$(call setup_config)
	$(call cross_compile_kernel)

clean:
	$(call clean_config)
	$(call clean_Kconfig)
	$(call clean_Makefile)
	$(call clean_module)
	$(call clean_code)

FIND_MAKEFILE = obj-$(DOLLAR)(CONFIG_JLSEMI_PHY)
OBJ_MAKEFILE = $(KERNEL_DIR)/drivers/net/phy/Makefile

FIND_KCONFIG = if PHYLIB
KCONFIG_ID = config JLSEMI_PHY
OBJ_KCONFIG = $(KERNEL_DIR)/drivers/net/phy/Kconfig

FIND_CONFIG = CONFIG_PHYLIB
CONFIG_ID = CONFIG_JLSEMI_PHY
OBJ_CONFIG = $(KERNEL_DIR)/.config


define autocof
	echo	"obj-m = $(OBJ_MODULE).o\
		\n$(OBJ_MODULE)-objs = $(OBJ_CODE)" > $(SUB_DIR)/Makefile
endef

define cross_compile_kernel
	$(MAKE) $(BUILD) -j4 -C $(KERNEL_DIR)
endef

define build_driver
	$(MAKE) -C $(KERNEL_DIR) M=$(SUB_DIR) modules $(BUILD)
	cp $(SUB_DIR)/*.ko $(PWD)/build
endef

define config_kernel_gnu
	$(MAKE) menuconfig -C $(KERNEL_DIR)
endef

define setup_config
	$(shell if [ $(shell grep -c "^$(FIND_CONFIG)" $(OBJ_CONFIG)) -eq "1" ] \
		&& [ $(shell grep -c "^$(CONFIG_ID)" $(OBJ_CONFIG)) -eq "0" ]; then\
			sed -i "/^CONFIG_PHYLIB/a$(CFG)" $(OBJ_CONFIG) ; fi)
endef

define setup_Kconfg
	$(shell if [ $(shell grep -c "^$(FIND_KCONFIG)" $(OBJ_KCONFIG)) -eq "1" ] \
		&& [ $(shell grep -c "^$(KCONFIG_ID)" $(OBJ_KCONFIG)) -eq "0" ]; then\
			sed -i "/^if PHYLIB/a$(KCFG)" $(OBJ_KCONFIG) ; fi)
endef

define setup_Makefile
	$(shell if [ $(shell grep -c "^$(FIND_MAKEFILE)" $(OBJ_MAKEFILE)) -eq "0" ]; then\
			 sed -i "$(DOLLAR)a$(MFILE)" $(OBJ_MAKEFILE) ; fi)
endef

define copy_code_to_kernel
	$(call clean_module)
	$(shell cp $(SUB_DIR)/*.c $(SUB_DIR)/*.h $(KERNEL_DIR)/drivers/net/phy/)
endef


define clean_config
	$(shell if [ ! -f "$(OBJ_CONFIG)" ]; then\
			sed -i "/CONFIG_JLSEMI_PHY/d" $(OBJ_CONFIG); fi)
endef

define clean_Kconfig
	$(shell if [ ! -f "$(OBJ_KCONFIG)" ]; then\
			sed -i "/config JLSEMI_PHY/,+3d" $(OBJ_KCONFIG); fi)
endef

define clean_Makefile
	$(shell if [ ! -f "$(OBJ_MAKEFILE)" ]; then\
			sed -i "/obj-$(DOLLAR)(CONFIG_JLSEMI_PHY)/d" $(OBJ_MAKEFILE);\
			sed -i "/$(OBJ_MODULE)-objs/d" $(OBJ_MAKEFILE); fi)
endef

define clean_module
	$(shell rm -f $(SUB_DIR)/*.o $(SUB_DIR)/*.symvers $(SUB_DIR)/*.order \
		$(SUB_DIR)/*.ko $(SUB_DIR)/*.mod $(SUB_DIR)/*.mod.c \
		$(SUB_DIR)/Makefile $(PWD)/build/*.ko)
endef

define clean_code
	$(shell rm -rf $(KERNEL_DIR)/drivers/net/phy/$(DRIVER_LIST).c \
		$(KERNEL_DIR)/drivers/net/phy/$(DRIVER_LIST)-core.c \
		$(KERNEL_DIR)/drivers/net/phy/$(DRIVER_LIST)-core.h)
endef

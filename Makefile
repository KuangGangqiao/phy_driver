include $(PWD)/config.mk

PWD := $(shell pwd)

SUB_DIR := $(PWD)/source/phy_driver/$(DRIVER_TARGET)
export SUB_DIR

BUILD := ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
export BUILD

export KERNEL_DIR

KCFG = config JLSEMI_PHY\n\
	\t tristate \"JLSEMI PHYs\"\n\
	\t ---help---\n\
	\t Currently has a driver for the $(OBJ_MODULE)
DOLLAR = \$
MFILE = obj-$(DOLLAR)(CONFIG_JLSEMI_PHY)\
	\t += $(OBJ_MODULE).o
CFG = CONFIG_JLSEMI_PHY=m

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
	echo	" obj-m = $(OBJ_MODULE).o \n \
		$(OBJ_MODULE)-objs = $(DRIVER_TARGET).o \
		$(DRIVER_TARGET)-core.o " > $(SUB_DIR)/Makefile
endef

define cross_compile_kernel
	$(MAKE) $(BUILD) -j4 -C $(KERNEL_DIR)
endef

define build_driver
	$(MAKE) -C $(KERNEL_DIR) M=$(SUB_DIR) modules $(BUILD)
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
	$(shell cp $(SUB_DIR)/$(DRIVER_TARGET).c $(SUB_DIR)/$(DRIVER_TARGET)-core.c \
		$(SUB_DIR)/$(DRIVER_TARGET)-core.h $(KERNEL_DIR) $(KERNEL_DIR)/drivers/net/phy/)
endef


define clean_config
	$(shell if [ ! -f "$(OBJ_CONFIG)" ]; then\
			sed -i "/^CONFIG_JLSEMI_PHY/d" $(OBJ_CONFIG); fi)
endef

define clean_Kconfig
	$(shell if [ ! -f "$(OBJ_KCONFIG)" ]; then\
			sed -i "/^config JLSEMI_PHY/,+3d" $(OBJ_KCONFIG); fi)
endef

define clean_Makefile
	$(shell if [ ! -f "$(OBJ_MAKEFILE)" ]; then\
			sed -i "/^obj-$(DOLLAR)(CONFIG_JLSEMI_PHY)/d" $(OBJ_MAKEFILE); fi)
endef

define clean_module
	$(MAKE) -C $(KERNEL_DIR) M=$(SUB_DIR) clean $(BUILD)
	rm -rf $(SUB_DIR)/Makefile
endef

define clean_code
	rm -rf $(KERNEL_DIR)/drivers/net/phy/$(DRIVER_TARGET).c \
		$(DRIVER_TARGET)-core.c $(DRIVER_TARGET)-core.h
endef

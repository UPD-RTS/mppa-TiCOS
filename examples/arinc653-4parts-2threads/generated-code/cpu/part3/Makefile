export DEPLOYMENT_HEADER=$(shell pwd)/deployment.h
include $(POK_PATH)/misc/mk/config.mk
TARGET = part3.elf
OBJS = main.o activity.o deployment.o
LINKERSCRIPT = partition_3.lds
all: libpok $(TARGET)
clean: common-clean
include $(POK_PATH)/misc/mk/common-$(ARCH).mk
include $(POK_PATH)/misc/mk/rules-partition.mk
include $(POK_PATH)/misc/mk/rules-common.mk
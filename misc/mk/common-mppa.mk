CFLAGS	= $(CONFIG_MPPA_CC_CFLAGS) -DPOK_ARCH_MPPA -iwithprefix $(KIND_CFLAGS) $(GENERIC_FLAGS) -Wall -g -O0 -Wuninitialized
#LDFLAGS	=	-m $(ELF_MODE)

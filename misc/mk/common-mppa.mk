CFLAGS = $(CONFIG_MPPA_CC_CFLAGS) -iwithprefix include -fno-builtin -DPOK_ARCH_MPPA $(KIND_CFLAGS) $(GENERIC_FLAGS) -Wall -g -O0 -Wuninitialized
LDFLAGS += $(CONFIG_MPPA_CC_LFLAGS)
LDFLAGS += --verbose --print-map

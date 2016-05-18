CONFIG_CFLAGS=$(CONFIG_MPPA_CC_CFLAGS) -iwithprefix include -fno-builtin -DPOK_ARCH_MPPA $(KIND_CFLAGS) $(GENERIC_FLAGS) -Wall -Wextra -Werror -g -O0 -Wuninitialized
CFLAGS=$(CONFIG_MPPA_CC_CFLAGS) -iwithprefix include -fno-builtin -DPOK_ARCH_MPPA $(KIND_CFLAGS) $(GENERIC_FLAGS) -Wall -Wextra -Werror -g -O0 -Wuninitialized -nodefaultlibs -nostdlib
LDFLAGS+=$(CONFIG_MPPA_CC_LFLAGS)

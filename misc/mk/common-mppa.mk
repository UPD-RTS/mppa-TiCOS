CONFIG_CFLAGS=$(CONFIG_MPPA_CC_CFLAGS) -iwithprefix include -fno-builtin -DPOK_ARCH_MPPA -D__mos__ -DNO_STD_LIB $(KIND_CFLAGS) $(GENERIC_FLAGS) -Wall -Wextra -Werror -g -O0 -Wuninitialized -nodefaultlibs -nostdlib
CFLAGS=$(CONFIG_MPPA_CC_CFLAGS) -iwithprefix include -fno-builtin -DPOK_ARCH_MPPA -D__mos__ -DNO_STD_LIB $(KIND_CFLAGS) $(GENERIC_FLAGS) -Wall -Wextra -Werror -g -O0 -Wuninitialized -nodefaultlibs -nostdlib
LDFLAGS+=$(CONFIG_MPPA_CC_LFLAGS)

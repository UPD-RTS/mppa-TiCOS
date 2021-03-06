.SILENT:

AR_ppc=powerpc-elf-ar rcs
AR_x86=/usr/bin/ar rcs
AR_mppa=k1-ar rcs
AWK=/usr/bin/gawk
CAT=/bin/cat
CC_ppc=powerpc-elf-gcc
CC_x86=/usr/bin/gcc
CC_mppa=k1-gcc
CD=cd
CONFIG_CFLAGS=-W -Wall -g -m32
CONFIG_LDFLAGS=--warn-common
CONFIG_QEMU_x86= -fda $(POK_PATH)/misc/grub-boot-only.img 
CONFIG_MPPA_CC_CFLAGS=-std=gnu99 -mcluster=node -mboard=developer -march=k1b -mos=bare -L$(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)
CONFIG_MPPA_IO_CFLAGS=-std=gnu99 -DMPPA_TRACE_ENABLE
CONFIG_MPPA_CC_LFLAGS=-L$(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP) 
CONFIG_MPPA_IO_CFLAGS=-L$(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)
COPY=/bin/cp
CP=/bin/cp
CXX_x86=/usr/bin/g++
CXX_mppa=k1-elf-g++
ECHO=/bin/echo
GREP=/bin/grep
GZIP=/bin/gzip
INSTRUMENTATION=0
KILL=/bin/kill
LD_ppc=powerpc-elf-ld
LD_x86=/usr/bin/ld
LD_mppa=k1-ld
MKDIR=/bin/mkdir
MV=/bin/mv
OBJCOPY_ppc=powerpc-elf-objcopy
OBJCOPY_x86=/usr/bin/objcopy
OBJCOPY_mppa=k1-objcopy
OBJDUMP_ppc=powerpc-elf-objdump
OBJDUMP_x86=/usr/bin/objdump
OBJDUMP_mppa=k1-objdump
PERL=/usr/bin/perl
QEMU_ppc=$(QEMU_HOME)/beBox/afaerber/install/bin/qemu-system-ppc
RANLIB_ppc=powerpc-elf-ranlib
RANLIB_x86=/usr/bin/ranlib
RANLIB_mppa=k1-ranlib
RM=/bin/rm -r -f
SED=/bin/sed
SLEEP=/bin/sleep
TOUCH=/usr/bin/touch

ECHO_FLAGS=-e
ECHO_RED="\e[1;31m"
ECHO_GREEN="\e[1;32m"
ECHO_RESET="\e[0m"

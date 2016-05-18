/**
 * Copyright (C) 2013-2016 Kalray SA.
 *
 * All rights reserved.
 */

/* Include board-specific definitions */
INCLUDE vboard_u.ld

__MPPA_BURN_TX = DEFINED(__MPPA_BURN_TX) ? __MPPA_BURN_TX : 7;
__MPPA_BURN_FDIR = DEFINED(__MPPA_BURN_FDIR) ? __MPPA_BURN_FDIR : 4;

DDR_START = 0x80000000;
VIRT_U_MEM_START = ALIGN((INTERNAL_RAM_BASE + MOS_RESERVED), 0x1000);
DDR_SIZE = 0x80000000;
VIRT_U_MEM_END   = INTERNAL_RAM_BASE + ABSOLUTE(INTERNAL_RAM_SIZE);
K1_EXCEPTION_ADDRESS = 0x0;
USER_STACK_SIZE = DEFINED(USER_STACK_SIZE) ? USER_STACK_SIZE : 0x800;
BOOT_STACK_SIZE  = DEFINED(BOOT_STACK_SIZE) ? BOOT_STACK_SIZE   : 0x100;
_VBSP_OVERRIDE_PE_MASK = DEFINED(_VBSP_OVERRIDE_PE_MASK) ? _VBSP_OVERRIDE_PE_MASK : 0xffff;
KERNEL_STACK_SIZE    = DEFINED(KERNEL_STACK_SIZE) ? KERNEL_STACK_SIZE : 0x400;
_MOS_SECURITY_LEVEL = DEFINED(_MOS_SECURITY_LEVEL) ? _MOS_SECURITY_LEVEL : (DEFINED(_LIBNOC_DISABLE_FIFO_FULL_CHECK) ? 0 : 1);
TOTAL_USER_STACK_SPACE = USER_STACK_SIZE * 16;

EXTERN( _vstart,__bsp_global_desc, bin_descriptor, locked_assert_func, locked_memset, __bsp_get_router_id, __stack_overflow_detected, micro_interface_ctor, libnoc_ctor, __mppa_dsm_client_ctor, main, mppa_power_base_exit, _scoreboard, __mppa_remote_client_init_ctor)
/* Definition of sections */
SECTIONS
{
INCLUDE prologue_link.ld

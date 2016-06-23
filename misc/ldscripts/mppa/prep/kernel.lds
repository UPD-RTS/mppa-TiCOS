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
INCLUDE "prologue_link.ld"

   .tls_pe0 ALIGN(8) :  AT ( ALIGN( LOADADDR(.tbss) + SIZEOF(.tbss), 8))
   {
    _tls_pe0_start = ABSOLUTE(.);
    _master_tls_start = ABSOLUTE(_tls_pe0_start);
   /* Reserve room for .tdata and .tbss for PE0 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe0_end = ABSOLUTE(.);
    _master_tls_end = ABSOLUTE(_tls_pe0_end);
   } =0x12345678

   .tls_pe1 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe0) + SIZEOF(.tls_pe0), 8))
   {
    _tls_pe1_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE1 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe1_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe2 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe1) + SIZEOF(.tls_pe1), 8))
   {
    _tls_pe2_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE2 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe2_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe3 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe2) + SIZEOF(.tls_pe2), 8))
   {
    _tls_pe3_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE3 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe3_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe4 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe3) + SIZEOF(.tls_pe3), 8))
   {
    _tls_pe4_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE4 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe4_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe5 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe4) + SIZEOF(.tls_pe4), 8))
   {
    _tls_pe5_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE5 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe5_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe6 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe5) + SIZEOF(.tls_pe5), 8))
   {
    _tls_pe6_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE6 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe6_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe7 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe6) + SIZEOF(.tls_pe6), 8))
   {
    _tls_pe7_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE7 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe7_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe8 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe7) + SIZEOF(.tls_pe7), 8))
   {
    _tls_pe8_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE8 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe8_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe9 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe8) + SIZEOF(.tls_pe8), 8))
   {
    _tls_pe9_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE9 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe9_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe10 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe9) + SIZEOF(.tls_pe9), 8))
   {
    _tls_pe10_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE10 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe10_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe11 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe10) + SIZEOF(.tls_pe10), 8))
   {
    _tls_pe11_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE11 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe11_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe12 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe11) + SIZEOF(.tls_pe11), 8))
   {
    _tls_pe12_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE12 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe12_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe13 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe12) + SIZEOF(.tls_pe12), 8))
   {
    _tls_pe13_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE13 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe13_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe14 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe13) + SIZEOF(.tls_pe13), 8))
   {
    _tls_pe14_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE14 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe14_end = ABSOLUTE(.);
   } =0x12345678

   .tls_pe15 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe14) + SIZEOF(.tls_pe14), 8))
   {
    _tls_pe15_start = ABSOLUTE(.);
   /* Reserve room for .tdata and .tbss for PE15 */
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe15_end = ABSOLUTE(.);
   } =0x12345678

   /*
   .tls_rm ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe15) + SIZEOF(.tls_pe15), 8)
   {
    _tls_rm_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm_end = ABSOLUTE(.);
    _end = ABSOLUTE(.);
  } =0x12345678
  */

   _pag_zone_size = 0;
   _sbss_start = 0;
   _sbss_end = 0;
   _tls_rm_start = 0;

  _smem_heap_start = ABSOLUTE(.);
  _smem_heap_end = ABSOLUTE(.);

  .heap   ALIGN(0x1000) :  AT ( ALIGN( LOADADDR(.tls_pe15) + SIZEOF(.tls_pe15), 0x1000))
  {
      _heap_start    = ABSOLUTE(.);
      . = ABSOLUTE(ROM_MEM_START);
  }

  . =  0xF5000;
  _pok_heap_start = ABSOLUTE(.);

  _start_async_copy = VIRT_U_MEM_START;
  _end_async_copy = VIRT_U_MEM_END;
  _heap_end      =  VIRT_U_MEM_END;
  _bin_start_frame = LOADADDR(.boot) >> 12;
  _bin_end_frame   = INTERNAL_RAM_SIZE >> 12;

INCLUDE "epilogue_link.ld"

}

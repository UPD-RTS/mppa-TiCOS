
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
  _start_async_copy = VIRT_U_MEM_START;
  _end_async_copy = VIRT_U_MEM_END;
  _heap_end      =  VIRT_U_MEM_END;
  _bin_start_frame = LOADADDR(.boot) >> 12;
  _bin_end_frame   = INTERNAL_RAM_SIZE >> 12;


INCLUDE "epilogue_link.ld"

}

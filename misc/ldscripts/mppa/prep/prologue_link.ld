/**
 * Copyright (C) 2013-2014 Kalray SA.
 *
 * All rights reserved.
 */

  .mOS K1_BOOT_ADDRESS : AT ( K1_BOOT_ADDRESS )
  {
      KEEP(*(.mOS.*))
  }

  .boot VIRT_U_MEM_START :  AT (ALIGN ( LOADADDR(.mOS) + MOS_RESERVED, 0x1000))
  {
	KEEP(*(.binaries))
    _bin_start = ABSOLUTE(.);
    *(.my_boot)    /* boot section */
    . = ALIGN(0x8);
  }  =0x12345678


  .text ALIGN(0x1000) : AT ( ALIGN( LOADADDR(.boot) + SIZEOF(.boot), 0x1000))
  {
	__text_begin = ABSOLUTE(.);
    _handler_start = ABSOLUTE(.);
    *(.handler) /* trap handlers */
    *(.init_code) /* trap handlers */
    *(.locked_text)
    *(.locked_text.*)
    KEEP(*(.init))
    KEEP(*(.fini))
    *(.text)
    *(.text.*)
	*(.bsp_config_text)
     *(.gnu.linkonce.t.*)
    _debug_start = ABSOLUTE(.);
    . += DEBUG_SIZE;
    . = ALIGN(0x1000);
	__text_end = ABSOLUTE(.);
  }  =0x12345678
  _debug_start_lma  = LOADADDR( .text ) + _debug_start - __text_begin;


  .stack   ALIGN(0x1000) :  AT ( ALIGN( LOADADDR(.text) + SIZEOF(.text) , 0x1000))
  {
      __user_stack_begin = ABSOLUTE(.);
      _user_stack_end     = ABSOLUTE(.);
      _master_stack_end     = ABSOLUTE(.);
      . += TOTAL_USER_STACK_SPACE;
      _master_stack_start   = ABSOLUTE(.);
      _user_stack_start   = ABSOLUTE(.);
      . = ALIGN(0x1000);
      __user_stack_end = ABSOLUTE(.);
      /* Allocate the main stack area. */
      _end = __user_stack_end;

  }

  /* This is the value of the GP base */
  .rodata   ALIGN(0x1000) :  AT ( ALIGN( LOADADDR(.stack) + SIZEOF(.stack), 0x1000))  {
	  __rodata_begin = ABSOLUTE(.);
    KEEP(*crtbegin.o(.ctors))
    KEEP(*crtbegin.?(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o $OTHER_EXCLUDE_FILES) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    *(.rodata.*)
    *(.rodata)
    *(.rodata.*)
    *(.rodata)

    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o $OTHER_EXCLUDE_FILES) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    KEEP (*(.eh_frame))
    *(.jcr)
    *(.gnu.linkonce.r.*)
    *(.gcc_except_table)
    *(.gcc_except_table.*)
    . = ALIGN(0x1000);
	  __rodata_end = ABSOLUTE(.);
  } =0x12345678

  .data ALIGN(0x1000) : AT ( ALIGN( LOADADDR(.rodata) + SIZEOF(.rodata), 0x1000))
  {
  __data_begin = ABSOLUTE(.);
    _locked_data_start = ABSOLUTE(.);
    _data_start = ABSOLUTE(.);
    *(.data) /* trap handlers */
    *(.lowmem_data) /* trap handlers */
    *(.tls_start)
    *(.tls_start*)
    *(.data.*) /* trap handlers */
    *(.locked_data) /* trap handlers */
    *(.locked_data.*) /* trap handlers */
    *(.ctors)
    *(.dtors)
    *(.lowmem_data)
    *(.gnu.linkonce.d.*)
    *(COMMON)
    . = ALIGN(0x20);
    __libs_ctors_start = ABSOLUTE(.);
    KEEP(*(SORT(.libs_ctors.*)))
    __libs_ctors_end   = ABSOLUTE(.);
    . = ALIGN(0x20);
  }  =0x12345678


  .bsp_config ALIGN(0x8) : AT ( ALIGN( LOADADDR(.data) + SIZEOF(.data), 0x8))
  {
	  *(.bsp_config_desc)
	  *(.bsp_config)
  }

  MPPA_ARGAREA_SIZE  = 0x1000;
  .scoreboard ALIGN(0x100) :  AT ( ALIGN( LOADADDR(.bsp_config) + SIZEOF(.bsp_config), 0x100))
  {
      _scoreboard_start  = ABSOLUTE(.);
      KEEP(*(.scoreboard))
      _scb_mem_frames_array = ABSOLUTE(.);
      . += (INTERNAL_RAM_SIZE >> 15);
      _scoreboard_end    = ABSOLUTE(.);
      _scoreboard_boot_stack_end = ABSOLUTE(.);
      . += (BOOTSTACK_SIZE*16);
      _scoreboard_boot_stack_start = ABSOLUTE(.);
      _scoreboard_kstack_end = ABSOLUTE(.);
      . += (KSTACK_SIZE*16);
      _scoreboard_kstack_start = ABSOLUTE(.);
      MPPA_ARGAREA_START = ABSOLUTE(.);
      . += MPPA_ARGAREA_SIZE;
  }

  MPPA_ARGAREA_FILE_OFFSET  =  LOADADDR( .scoreboard ) + MPPA_ARGAREA_START - _scoreboard_start ;

  /* Section used with dynamic executable */
  .interp ALIGN(0x8):   AT ( ALIGN( LOADADDR(.scoreboard) + SIZEOF(.scoreboard), 0x8))
  {
    *(.interp)
  }
  .dynstr ALIGN(0x8):   AT ( ALIGN( LOADADDR(.interp) + SIZEOF(.interp), 0x8))
  {
    *(.dynstr)
  }

  .hash ALIGN(0x8):   AT ( ALIGN( LOADADDR(.dynstr) + SIZEOF(.dynstr), 0x8))
  {
    *(.hash)
  }

  .plt ALIGN(0x8):   AT ( ALIGN (LOADADDR(.hash) + SIZEOF(.hash), 0x8))
  {
    *(.plt)
    *(.plt.*)
  }

  .got ALIGN(0x8):   AT ( ALIGN( LOADADDR(.plt) + SIZEOF(.plt), 0x8))
  {
    *(.got)
    *(.got.*)
  }

  .dynamic ALIGN(0x8):   AT ( ALIGN( LOADADDR(.got) + SIZEOF(.got), 0x8))
  {
    *(.dynamic)
  }

  .dynsym ALIGN(0x8):   AT ( ALIGN( LOADADDR(.dynamic) + SIZEOF(.dynamic), 0x8))
  {
    *(.dynsym)
  }

  .rela.dyn ALIGN(0x8):   AT ( ALIGN( LOADADDR(.dynsym) + SIZEOF(.dynsym), 0x8))
  {
    *(.rela.dyn)
  }

  .rela.plt ALIGN(0x8):   AT ( ALIGN( LOADADDR(.rela.dyn) + SIZEOF(.rela.dyn), 0x8))
  {
    *(.rela.plt)
  }


  .bss   ALIGN(0x20) :  AT ( ALIGN( LOADADDR(.rela.plt) + SIZEOF(.rela.plt), 0x20))
  {
    _bss_start = ABSOLUTE(.);
    *(.bss)
    *(.bss.*)
    *(.dynbss)
    *(.dynbss.*)
    *(.gnu.linkonce.b.*)
    . = ALIGN(0x20);
    _bss_end = ABSOLUTE(.);
  } =0x12345678

  /* TLS support */
  .tdata   ALIGN(8) : AT ( ALIGN( LOADADDR(.bss) + SIZEOF(.bss), 8 ))
  {
    _tdata_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    . = ALIGN(8);
    _tdata_end = ABSOLUTE(.);
  } =0x12345678
  . = _tdata_end;
  _tdata_size = _tdata_end - _tdata_start;

  .tbss   ALIGN(8) : AT ( ALIGN( LOADADDR(.tdata) + SIZEOF(.tdata), 8))
  {
    _tbss_start = ABSOLUTE(.);
    *(.tbss)
    *(.tbss.*)
    *(.tbss)
    *(.tcommon)
    *(.gnu.linkonce.tb.*)
    . = ALIGN(8);
    _tbss_end = ABSOLUTE(.);
  } =0x12345678
  . = _tbss_end;
  _tbss_size = _tbss_end - _tbss_start;

  _tls_size = _tbss_size + _tdata_size;

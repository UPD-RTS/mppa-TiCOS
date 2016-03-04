
  _scoreboard_offset = ABSOLUTE(_scoreboard_start) - ABSOLUTE(VIRT_U_MEM_START);
  _scb_mem_frames_array_offset = ABSOLUTE(_scb_mem_frames_array) - ABSOLUTE(_scoreboard_start);


   /* DWARF2 debugging sections */
  .debug_info       0 : { *(.debug_info) }
  .debug_line       0 : { *(.debug_line) }
  .debug_abbrev     0 : { *(.debug_abbrev) }
  .debug_frame      0 : { *(.debug_frame) }
  .debug_aranges    0 : { *(.debug_aranges) }
  .debug_pubnames   0 : { *(.debug_pubnames) }
  .debug_str        0 : { *(.debug_str) }
  .debug_funcnames  0 : { *(.debug_funcnames) }
  .debug_typenames  0 : { *(.debug_typenames) }
  .debug_varnames   0 : { *(.debug_varnames) }
  .debug_weaknames  0 : { *(.debug_weaknames) }
  .debug_macinfo    0 : { *(.debug_macinfo) }
  .debug_loc        0 : { *(.debug_loc) }

  /DISCARD/ :
  {
      *(.boot)
      *(.boot.startup)
      *(.boot.syscall)
      *(.boot.exception)
      *(.boot.interrupt)
      *(.locked.binaries_dsm)
      *(.locked.binaries_dsm_text)
  }

  /* tracepoints */
  __k1_tracepoint_data 0 :
  {
    KEEP (*(__k1_tracepoint_data*))
  }

ENTRY(main)
SECTIONS
{

  . = SEGMENT_START(".data", DATA_ADDRESS);
  .data : {
    *(.rodata .rodata.*)
    *(.data)
    *(SORT(.init_array.*) .init_array)
    *(SORT(.fini_array.*) .fini_array)
    *(.bss)
  }

  . = SEGMENT_START(".text", TEXT_ADDRESS);
  .text : {
  *(.text .text.*)
  }

  . =  0x1F0000;
  _stack = .;
  _end = _stack; PROVIDE (end = .);

}

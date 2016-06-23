ENTRY(main)
SECTIONS
{

	. = SEGMENT_START(".data", DATA_ADDRESS);
	.data :
	{
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		*(.rodata1)

		*(.data .data.* .gnu.linkonce.d.*)
	        KEEP (*(.gnu.linkonce.d.*personality*))

		*(SORT(.init_array.*) .init_array)
		*(SORT(.fini_array.*) .fini_array)

		__bss_start = .;
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		__bss_end = .;

		_stack = .;
		_end = _stack; PROVIDE (end = .);
	}

	. = SEGMENT_START(".text", TEXT_ADDRESS);
	.text : AT ( ALIGN (0x100))
	{
		*(.start)
		*(.text .text.* .gnu.linkonce.t.*)
		/* .gnu.warning sections are handled specially by elf32.em.  */
		*(.gnu.warning)
		__text_end = .;
	}

}

install: rom.bin

rom.bin: $(TARGET)
	$(OBJCOPY) -O binary $< $@

install-clean:
	$(RM) rom.bin

run: rom.bin
	$(ECHO) "$(QEMU) -s -S  -L . -bios rom.bin rom.bin -M prep -serial /dev/stdout -cpu ppc32 -d in_asm,int"
	$(QEMU)   -L . -bios rom.bin rom.bin -M prep -serial /dev/stdout -cpu ppc32 -d in_asm,int


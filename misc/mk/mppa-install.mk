run: $(TARGET)
	k1-cluster -s $(K1_TOOLCHAIN_DIR)/lib64/libsyscall.so --march=k1b --cycle-based -- $^

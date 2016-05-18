run: $(TARGET)
	k1-cluster -s $(K1_TOOLCHAIN_DIR)/lib64/libsyscall_bare.so --march=k1b --profile -- $^

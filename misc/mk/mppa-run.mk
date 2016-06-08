SIMU_TOOL=k1-cluster
SIMU_FLAGS=-s $(K1_TOOLCHAIN_DIR)/lib64/libsyscall.so --march=k1b --profile
RUN_TOOL=k1-jtag-runner
RUN_FLAGS=--board=developer
TARGET=pok.elf

simu:
	$(SIMU_TOOL) $(SIMU_FLAGS) -- $(TARGET)

hard:
	$(RUN_TOOL) $(RUN_FLAGS) --exec-file=Cluster0:$(TARGET)


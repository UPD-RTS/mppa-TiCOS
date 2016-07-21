export ARCH=mppa
export BSP=prep
export POK_CONFIG_OPTIMIZE_FOR_GENERATED_CODE=1
include $(POK_PATH)/misc/mk/config.mk
include $(POK_PATH)/misc/mk/common-$(ARCH).mk
TARGET=$(shell pwd)/pok.elf
PARTITIONS=part1/part1.elf
#Hexadecimal representation
EXAMPLE_START_ADDR=5D000

HEADERFILE=kernel/deployment.h
$(if $(wildcard $(HEADERFILE)),,$(error "kernel/deployment.h is missing, bye bye."))
#export DEF_VADDR=`grep -i "POK_CONFIG_DEFINE_BASE_VADDR" $(HEADERFILE)`
export DEF_VADDR=$(shell grep -i POK_CONFIG_DEFINE_BASE_VADDR $(HEADERFILE)|grep -v "/")
export PARTITIONS_VADDR=`awk '{if ($$2 == "POK_CONFIG_PARTITIONS_BASE_VADDR") print $$3}' $(HEADERFILE)|sed 's/{//'|sed 's/}//'|sed 's/,/ /g'`

export SKIP_LOADER=$(shell grep -i POK_SKIP_LOADER $(HEADERFILE)|grep -v "/")
export PARTITIONS_ADDR=`awk '{if ($$2 == "POK_CONFIG_PARTITIONS_LOADADDR") print $$3}' $(HEADERFILE)|sed 's/{//'|sed 's/}//'|sed 's/,/ /g'`

KERNEL=kernel/kernel.lo
ifeq ($(ARCH), mppa)
all: build-lds partitions sections build-kernel $(TARGET)
else
all: build-lds build-kernel partitions $(TARGET)
endif

build-lds: kernel-lds partitions-lds

ifeq ($(ARCH), mppa)
sections:
	$(ECHO) "Extracting loadable sections from partitions"
	cp ./kernel/deployment.h ./kernel/deployment_t.h
	mv ./kernel/deployment.h ./kernel/deployment_original.h
	mv ./kernel/deployment_t.h ./kernel/deployment.h
	sed -i "s/.*#endif.*/\n/" ./kernel/deployment.h
	$(ECHO) "#ifdef POK_ARCH_MPPA" >> ./kernel/deployment.h
	$(ECHO) -n "	#define POK_PARTITION_ENTRIES {" >> ./kernel/deployment.h
	c=1;for v in $(PARTITIONS); do \
		$(ECHO) -n "PART_ENTRY_$${c}" >> ./kernel/deployment.h; \
		parts=`$(ECHO) "$(PARTITIONS)" | wc -w`; \
		$(ECHO) -n `if (( c < parts )) ; then echo , ; fi` >> ./kernel/deployment.h ;\
		c=`$(ECHO) $${c}+1 | bc` ; \
	done
	$(ECHO) "}" >> ./kernel/deployment.h;
	$(ECHO) "#endif" >> ./kernel/deployment.h
	$(ECHO) "#endif" >> ./kernel/deployment.h
	c=1;for v in $(PARTITIONS); do \
		entry=`k1-readelf -l "part$${c}/part$${c}.elf" | grep -w '^Entry' | grep -o '0x.*'`; \
		sed -i "s/PART_ENTRY_$${c}/$${entry}/g" kernel/deployment.h ; \
	c=`$(ECHO) $${c}+1 | bc` ; \
	done
endif

kernel-lds:
# Handle mppa linker script
ifeq ($(ARCH), mppa)
	$(ECHO) "MPPA compilation: creating kernel linker script"
	$(ECHO) "Using DEFAULT Base physical addresses"
	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_template.lds ./kernel/kernel.lds ;
	sed s/EXAMPLE_START_ADDR/0x$(EXAMPLE_START_ADDR)/ ./kernel/kernel.lds > ./kernel/kernel_tmp.lds ;
	mv ./kernel/kernel_tmp.lds ./kernel/kernel.lds;
else
# Handle kernel linker scripptkernel-lds:
ifeq ($(shell grep -i POK_NEEDS_QEMU_SETUP $(HEADERFILE)|grep -v "/"),)
	$(ECHO) "PRAORTIS SIM setup $(ARCH)"
	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/PROARTIS_sim/kernel_template.lds $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_template.lds
	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/PROARTIS_sim/kernel_postscript.lds $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_postscript.lds
	sed s/placeholder/0xFC000000/ $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_template.lds > $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.lds
else
	$(ECHO) "QEMU setup"
	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/QEMU/kernel_template.lds $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_template.lds
	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/QEMU/kernel_postscript.lds $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_postscript.lds
	sed s/placeholder/0xFFF00000/ $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_template.lds > $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.lds
endif
ifeq ($(shell grep -i POK_SKIP_LOADER $(HEADERFILE)|grep -v "/"),)
	$(ECHO) "Using DEFAULT Base physical addresses"
#	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_template.lds $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds
#	sed s/placeholder/0xFC000000/ $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds > $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.lds
	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.lds ./kernel/kernel.lds ; \
#	rm -f $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds
else
	$(ECHO) "Base physical addresses"
	cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_preamble.lds $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds
	c=1;for v in $(PARTITIONS_ADDR); do \
$(ECHO) "Base address" $$v "-" $$c ": in kernel.lds" ; \
		$(ECHO) "   . = $$v; " >> $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds; \
		$(ECHO) "   .part$$c :  " >> $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds; \
		$(ECHO) "   { " >> $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds; \
		$(ECHO) "       part$$c.elf(.text) " >> $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds; \
		$(ECHO) "   } " >> $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds; \
		$(ECHO) " " >> $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds; \
		c=`$(ECHO) $${c}+1 | bc` ; \
	done
	cat $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_postscript.lds > $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.lds ; \
  cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.lds ./kernel/kernel.lds ; \
	rm -f $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel_t.lds
endif
endif

partitions-lds:
ifeq ($(ARCH), mppa)
	$(ECHO) "Creating partition linker scripts"
	c=1;last_text=$(EXAMPLE_START_ADDR);for v in $(PARTITIONS); do \
		text_address=`$(ECHO) "ibase=obase=16; $${last_text}" | bc`; \
		data_address=`$(ECHO) "ibase=obase=16; $${text_address}+20000" | bc`; \
		last_text=`$(ECHO) "ibase=obase=16; $${data_address}+20000" | bc`; \
		$(ECHO) "Base text address 0x"$$text_address ": in ./part"$$c"/partition_"$$c".lds" ; \
		$(ECHO) "Base data address 0x"$$data_address ": in ./part"$$c"/partition_"$$c".lds" ; \
		cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/partition_template.lds ./part$$c/partition_$${c}_t.lds ; \
		sed s/TEXT_ADDRESS/0x$$text_address/ ./part$$c/partition_$${c}_t.lds > ./part$$c/partition_$${c}_t2.lds; \
		sed s/DATA_ADDRESS/0x$$data_address/ ./part$$c/partition_$${c}_t2.lds > ./part$$c/partition_$${c}.lds; \
		rm -f ./part$$c/partition_$${c}_t.lds; \
		rm -f ./part$$c/partition_$${c}_t2.lds; \
		c=`$(ECHO) $${c}+1 | bc` ; \
	done
else
# Handle partitions linker scripts
ifeq ($(shell grep -i POK_SKIP_LOADER $(HEADERFILE)|grep -v "/"),)
	$(eval PARTITION_LDS=partition_template.lds)
else
	$(eval PARTITION_LDS=partition_no_loader.lds)
	$(ECHO) "Skipping POK loader"
endif
#ifeq ($(shell grep -r "POK_CONFIG_DEFINE_BASE_VADDR" $(HEADERFILE)),)
ifeq ($(shell grep -i POK_CONFIG_DEFINE_BASE_VADDR $(HEADERFILE)|grep -v "/"),)
	$(ECHO) "Using DEFAULT Base virtual addresses"
	c=1;for v in $(PARTITIONS); do \
		$(ECHO) "Base vaddress 0x1000 -" $$c ": in ./part"$$c"/partition_"$$c".lds" ; \
		cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/$(PARTITION_LDS) $(eval) ./part$$c/partition_$$c_t.lds ; \
		sed s/placeholder/0x1000/ ./part$$c/partition_$$c_t.lds > ./part$$c/partition_$$c.lds;\
		rm -f ./part$$c/partition_t.lds ; \
    c=`$(ECHO) $${c}+1 | bc` ; \
	done
else
	c=1;for v in $(PARTITIONS_VADDR); do \
		$(ECHO) "Base vaddress" $$v "-" $$c ": in ./part"$$c"/partition_"$$c".lds" ; \
		cp $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/$(PARTITION_LDS) $(eval) ./part$$c/partition_$$c_t.lds ; \
		sed s/placeholder/$$v/ ./part$$c/partition_$$c_t.lds > ./part$$c/partition_$$c.lds;\
		rm -f ./part$$c/partition_.lds ; \
    c=`$(ECHO) $${c}+1 | bc` ; \
	done
endif
endif

build-kernel:
	$(CD) kernel && $(MAKE)
ifeq ($(ARCH), mppa)
	rm ./kernel/deployment.h
	mv ./kernel/deployment_original.h ./kernel/deployment.h
endif

partitions:
	$(CD) part1 && $(MAKE)


clean: common-clean
	$(CD) kernel && $(MAKE) clean
	$(CD) part1 && $(MAKE) clean

distclean: clean
	$(CD) kernel && $(MAKE) distclean
	$(CD) part1 && $(MAKE) distclean

include $(POK_PATH)/misc/mk/rules-common.mk
include $(POK_PATH)/misc/mk/rules-main.mk
include $(POK_PATH)/misc/mk/install-rules.mk

INSTRUMENTATION_PATH=$(POK_PATH)/kernel/core
#kernel/core
#libpok/arinc653
INSTRUMENTATION_FILE=sched
#sampling
CINS=/usr/local/bin/cins
XSTUTILS=/usr/local/bin/xstutils
ROOT_FUNCTION=pok_elect_partition
#pok_elect_partition
#READ_SAMPLING_MESSAGE
#WRITE_SAMPLING_MESSAGE

instrument:
	$(ECHO) Preprocessing...
	$(RM) -rf $(POK_PATH)/instrumentation/instr
	$(RM) -rf $(POK_PATH)/instrumentation/xsc
	$(RM) -rf $(POK_PATH)/instrumentation/*.exf
	$(RM) -rf $(POK_PATH)/instrumentation/*.p
	$(MKDIR) $(POK_PATH)/instrumentation/instr
	$(MKDIR) $(POK_PATH)/instrumentation/xsc
	#if [ -f $(INSTRUMENTATION_PATH)/$(INSTRUMENTATION_FILE)_ORIGINAL.c ]; then $(MV) -f $(INSTRUMENTATION_PATH)/$(INSTRUMENTATION_FILE)_ORIGINAL.c $(INSTRUMENTATION_PATH)/$(INSTRUMENTATION_FILE).c; fi
	$(CC_ppc) -E -ansi $(INSTRUMENTATION_PATH)/$(INSTRUMENTATION_FILE).c -o $(POK_PATH)/instrumentation/$(INSTRUMENTATION_FILE).p -I$(POK_PATH)/kernel/include -I$(POK_PATH)/instrumentation -I$(POK_PATH)/libpok/include -include ./kernel/deployment.h
	$(ECHO) Done
	$(CINS) --cext GNU -v -c $(POK_PATH)/instrumentation/rvs_instrumentation.h $(POK_PATH)/instrumentation/$(INSTRUMENTATION_FILE).p -d $(POK_PATH)/instrumentation/instr/ --xsc-dir $(POK_PATH)/instrumentation/xsc/ --exf $(POK_PATH)/instrumentation/app.exf --nbits 12 -u -D --discover-fptr
	$(XSTUTILS) $(POK_PATH)/instrumentation/xsc/*.xsc -r $(ROOT_FUNCTION) -o $(POK_PATH)/instrumentation/xsc/$(ROOT_FUNCTION).rvd -v --debug-report
	$(RM) -rf ./rvs_dbgreport_files
	$(RM) -rf ./rvs_dbgreport.txt
	$(CD) $(POK_PATH)/instrumentation/instr && for f in *.p; do mv $$f `echo $$f | sed 's/\(.*\.\)p/\1c/'`; done
	#$(MV) -f $(INSTRUMENTATION_PATH)/$(INSTRUMENTATION_FILE).c $(INSTRUMENTATION_PATH)/$(INSTRUMENTATION_FILE)_ORIGINAL.c
	#$(CP) $(POK_PATH)/instrumentation/instr/$(INSTRUMENTATION_FILE).c $(INSTRUMENTATION_PATH)/$(INSTRUMENTATION_FILE).c
	#$(CC_ppc) -c $(POK_PATH)/instrumentation/rvs.c -o $(POK_PATH)/instrumentation/rvs.o
	#$(CC_ppc) -c -I/home/proartis/proartis/POK/instrumentation /home/proartis/proartis/POK/instrumentation/rapita.cpp -o /home/proartis/proartis/POK/instrumentation/rapita.o
	#$(MAKE)

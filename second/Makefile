obj-m := second.o
second-objs := main.o second_timer.o second_fops.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko .*.cmd *.mod.c modules.order Module.symvers .tmp_versions

.PHONY: module clean

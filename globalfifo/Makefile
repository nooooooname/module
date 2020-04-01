obj-m := globalfifo.o
globalfifo-objs := main.o fops.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko .*.cmd *.mod.c modules.order Module.symvers .tmp_versions

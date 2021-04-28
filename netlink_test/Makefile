KDIR := /lib/modules/$(shell uname -r)/build

obj-m += cnpv_kern.o

cnpv_kern-objs += nlkernel.o map.o

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* *.out
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

user :
	gcc nluser.c json.c -lm -g
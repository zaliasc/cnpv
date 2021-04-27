obj-m += cnpv_kern.o

# cnpv_kern-objs += cnpv.o json.o map.o config.o

cnpv_kern-objs += utils.o

ccflags-y := -std=gnu99 -Wno-declaration-after-statement -Wno-unused-function

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
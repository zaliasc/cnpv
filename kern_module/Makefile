obj-m += cnpv.o

cnpv-objs += fsmon.o helper.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


# TARGET = myopen 
# obj-m := ${TARGET}.o
# ${TARGET}-objs := ${TARGET}.o

# all:
# 	${MAKE} modules \
# 		--directory "/lib/modules/$(shell uname --release)/build" \
# 		M="$(shell pwd)"


# clean:
# 	${MAKE} clean \
# 		--directory "/lib/modules/$(shell uname --release)/build" \
# 		M="$(shell pwd)"

TARGET = socketopt
OBJS = module.o
MDIR = drivers/misc
 
EXTRA_CFLAGS = -DEXPORT_SYMTAB
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
# PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)
 
obj-m := $(TARGET).o
 
$(TARGET)-objs :=$(OBJS)
 
default:
	make -C $(KDIR) M=$(shell pwd) modules
# make -C  $(KDIR) SUBDIRS=$(shell pwd) modules 
	gcc -o user user.c
$(TARGET).o: $(OBJS)
	$(LD) $(LD_RFLAG) -r -o $@ $(OBJS)
 
insmod:
	insmod $(TARGET).ko
rmmod:
	rmmod $(TARGET).ko
 
clean:
	-rm -rf *.o *.ko .$(TARGET).ko.cmd .*.flags *.mod.c modules.order  Module.symvers .tmp_versions
	-rm -rf protocol/*.o protocol/.*.o.cmd *.markers 
	-rm -rf user
-include $(KDIR)/Rules.make
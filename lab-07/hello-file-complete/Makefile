obj-m += hello_file.o

LINUX_SRC = /tmp/deeksha/linux-4.4.3/

all:
	make -C $(LINUX_SRC) M=$(PWD) modules

clean:
	make -C $(LINUX_SRC) M=$(PWD) clean


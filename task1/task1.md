# Команда для пакетов
```bash
# apt-get install build-essential linux-headers-$(uname -r)
```

# hello.c

![hello.c](../screens/hello.png)

# Makefile
```bash
obj-m += hello.o

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

# Команда для проверки работы
```bash
# insmod hello.ko

# dmesg | tail -1
[ ] Hello world!

# rmmod hello.ko

# dmesg | tail -1
[] Cleaning up module.
```

# Компиляция
![hello.c](../screens/comp.png)


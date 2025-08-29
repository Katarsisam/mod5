#include <linux/module.h> 
#include <linux/printk.h> 
#include <linux/kobject.h> 
#include <linux/sysfs.h> 
#include <linux/init.h> 
#include <linux/fs.h> 
#include <linux/string.h>
#include <linux/tty.h>          /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>           /* For KDSETLED */
#include <linux/vt.h>
#include <linux/console_struct.h>       /* For vc_cons */
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/configfs.h>

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");

struct timer_list my_timer;
struct tty_driver *my_driver;
static int _kbledstatus = 0;


static struct kobject *example_kobject;
static int test = 3;

#define BLINK_DELAY   HZ/5
#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS  0xFF

static void my_timer_func(struct timer_list *ptr)
{
        //int *pstatus = (int *)ptr;
        int *pstatus = &_kbledstatus;
        if (*pstatus == test)
                *pstatus = RESTORE_LEDS;
        else
                *pstatus = test;
        (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,
                            *pstatus);
        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);
}
 
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, //чтение из переменной
                      char *buf)
{          
        return sprintf(buf, "%d\n", test);
}
 
static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr, //запись в переменную
                      const char *buf, size_t count)
{
        sscanf(buf, "%du", &test);
        return count;
}
 
 
static struct kobj_attribute foo_attribute =__ATTR(test, 0660, foo_show,
                                                   foo_store);
 
static int __init sys_init (void)
{
        int error = 0;
        int i;
 
        pr_debug("Module initialized successfully \n"); //сообщение
 
        example_kobject = kobject_create_and_add("systest",
                                                 kernel_kobj); // инициализация объёкта структуры kernel
        if(!example_kobject)
                return -ENOMEM; //защита
 
        error = sysfs_create_file(example_kobject, &foo_attribute.attr); //инциализация перменной/файла с атрибутами foo_attribute.attr по пути /sys/kernel/systest
        if (error) {
                pr_debug("failed to create the foo file in /sys/kernel/systest \n");
        }
        
        printk(KERN_INFO "kbleds: loading\n"); //вывод инфы модуля kernel
        printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console); //вывод текущей активной консоли
        for (i = 0; i < MAX_NR_CONSOLES; i++) {
                if (!vc_cons[i].d) //получение данных текущей консоли
                        break;
                printk(KERN_INFO "poet_atkm: console[%i/%i] #%i, tty %lx\n", i,
                       MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
                       (unsigned long)vc_cons[i].d->port.tty);
        }

        printk(KERN_INFO "kbleds: finished scanning consoles\n");
        my_driver = vc_cons[fg_console].d->port.tty->driver; //драйвер устройства порта активной консоли
        printk(KERN_INFO "kbleds: tty driver magic %x\n", my_driver); //число для проверки
        
        
        /*
         * Set up the LED blink timer the first time
         */
        
        //init_timer(&my_timer);
        timer_setup(&my_timer, my_timer_func, 0);
        //my_timer.function = my_timer_func;
        //my_timer.data = (unsigned long)&kbledstatus;
        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);

        return error;
}
 
static void __exit sys_exit (void)
{
        printk(KERN_INFO "kbleds: unloading...\n");
        del_timer(&my_timer);
        (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,
                            RESTORE_LEDS);
        pr_debug ("Module un initialized successfully \n");
        kobject_put(example_kobject);
}
 
MODULE_LICENSE("GPL");
module_init(sys_init);
module_exit(sys_exit);

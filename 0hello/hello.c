#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("me");
MODULE_DESCRIPTION("hello kernel");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
    printk(KERN_INFO "hello, kernel\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "goodbye, kernel\n");
}

module_init(hello_init);
module_exit(hello_exit);

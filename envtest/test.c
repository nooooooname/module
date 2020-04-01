#include <linux/init.h>
#include <linux/module.h>

#include "a.h"

static int __init main_init(void)
{
	//printk(KERN_EMERG "init@@@@@@@@\n");
	alert("init@@@@@@@@@@\n");
	return 0;
}

static void __exit main_exit(void)
{
	//printk(KERN_DEBUG "exit########\n");
	alert("exit##########\n");
}

module_init(main_init);
module_exit(main_exit);
MODULE_LICENSE("Dual BSD/GPL");

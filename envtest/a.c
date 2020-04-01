#include <linux/module.h>

void alert(char *str)
{
	printk(KERN_ALERT "%s", str);
}

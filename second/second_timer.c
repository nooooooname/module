#include "second_timer.h"
#include "second.h"

extern struct second_dev *second_devp;

void second_timer_handler(unsigned long arg)
{
	mod_timer(&second_devp->s_timer, jiffies + HZ);
	atomic_inc(&second_devp->counter);

	printk(KERN_INFO "当前的jiffes=%ld\n", jiffies);
}

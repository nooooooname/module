#include "second_fops.h"
#include "second.h"
#include "second_timer.h"
#include <linux/uaccess.h>

extern struct second_dev *second_devp;

int second_open(struct inode *inode, struct file *filp)
{
	init_timer(&second_devp->s_timer);
	second_devp->s_timer.function = second_timer_handler;
	second_devp->s_timer.expires = jiffies + HZ;

	add_timer(&second_devp->s_timer);

	atomic_set(&second_devp->counter, 0);

	return 0;
}

int second_release(struct inode *inode, struct file *filp)
{
	del_timer(&second_devp->s_timer);

	return 0;
}

ssize_t second_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	int counter = atomic_read(&second_devp->counter);

	if(put_user(counter, (int*)buf))
		return -EFAULT;

	return sizeof(int);
}

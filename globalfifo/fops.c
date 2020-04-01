#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "globalfifo.h"

extern struct globalfifo_dev *globalfifo_devp;

ssize_t globalfifo_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	int ret = 0;
	size_t count1, count2;
	struct globalfifo_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->r_wait, &wait);

	while(dev->current_len == 0)
	{
		if(filp->f_flags & O_NONBLOCK)
		{
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);

		schedule();

		if(signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}

		mutex_lock(&dev->mutex);
	}

	if(count > dev->current_len)
		count = dev->current_len;

	if(count > GLOBALFIFO_SIZE - dev->offset)
	{
		count1 = GLOBALFIFO_SIZE - dev->offset;
		count2 = count - count1;
	}
	else
	{
		count1 = count;
		count2 = 0;
	}
	if(copy_to_user(buf, dev->mem + dev->offset, count1) ||
			copy_to_user(buf + count1, dev->mem, count2))
	{
		ret = -EFAULT;
	}
	else
	{
		printk(KERN_NOTICE "从%lu读取了%lu个字节到%#lx，当前长度为%lu\n", dev->offset, count, (unsigned long)buf, dev->current_len - count);
		dev->offset += count;
		dev->offset %= GLOBALFIFO_SIZE;
		dev->current_len -= count;
		wake_up_interruptible(&dev->w_wait);
		ret = count;
	}

out:
	mutex_unlock(&dev->mutex);
out2:
	remove_wait_queue(&dev->r_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}

ssize_t globalfifo_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	int ret = 0;
	size_t count1, count2;
	struct globalfifo_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->w_wait, &wait);

	while(dev->current_len == GLOBALFIFO_SIZE)
	{
		if(filp->f_flags & O_NONBLOCK)
		{
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);

		schedule();

		if(signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}

		mutex_lock(&dev->mutex);
	}

	if(count > GLOBALFIFO_SIZE - dev->current_len)
		count = GLOBALFIFO_SIZE - dev->current_len;

	if(count > GLOBALFIFO_SIZE - (dev->offset + dev->current_len))
	{
		count1 = GLOBALFIFO_SIZE - (dev->offset + dev->current_len);
		count2 = count - count1;
	}
	else
	{
		count1 = count;
		count2 = 0;
	}
	if(copy_from_user(dev->mem + dev->offset + dev->current_len, buf, count1) ||
			copy_from_user(dev->mem, buf + count1, count2))
	{
		ret = -EFAULT;
	}
	else
	{
		dev->current_len += count;
		wake_up_interruptible(&dev->r_wait);
		ret = count;

		printk(KERN_NOTICE "从%#lx写入了%lu个字节到%lu，当前长度为%d\n", (unsigned long)buf, count, dev->offset, dev->current_len);
	}

out:
	mutex_unlock(&dev->mutex);
out2:
	remove_wait_queue(&dev->w_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}

loff_t globalfifo_llseek(struct file *filp, loff_t offset, int orig)
{
	loff_t ret = 0;

	switch(orig)
	{
		case 0:
			if(offset < 0)
			{
				ret = -EINVAL;
				break;
			}
			if(offset > GLOBALFIFO_SIZE)
			{
				ret = -EINVAL;
				break;
			}
			filp->f_pos = offset;
			ret = filp->f_pos;
			break;
		case 1:
			if((filp->f_pos + offset) > GLOBALFIFO_SIZE)
			{
				ret = -EINVAL;
				break;
			}
			if((filp->f_pos + offset) < 0)
			{
				ret = -EINVAL;
				break;
			}
			filp->f_pos += offset;
			ret = filp->f_pos;
			break;
		default:
			ret = -EINVAL;
			break;
	}
	return 0;
}

long globalfifo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalfifo_dev *dev = filp->private_data;

	switch(cmd)
	{
		case MEM_CLEAR:
			mutex_lock(&dev->mutex);
			memset(dev->mem, 0, GLOBALFIFO_SIZE);
			mutex_unlock(&dev->mutex);

			printk(KERN_INFO "globalfifo已清零\n");
			break;

		default:
			return -EINVAL;
	}

	return 0;
}

int globalfifo_open(struct inode *inode, struct file *filp)
{
	struct globalfifo_dev *dev = container_of(inode->i_cdev, struct globalfifo_dev, cdev);
	filp->private_data = dev;
	return 0;
}

int globalfifo_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "%s：%d:globalfifo_release();\n", __FILE__, __LINE__);	//@
	return 0;
}

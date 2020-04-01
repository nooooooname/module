#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "globalmem.h"

extern struct globalmem_dev *globalmem_devp;

ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;

	printk(KERN_ALERT "%s：%d:globalmem_read();\n", __FILE__, __LINE__); //@

	if(p >= GLOBALMEM_SIZE)
		return 0;
	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;

	mutex_lock(&dev->mutex);

	if(copy_to_user(buf, dev->mem + p, count))
	{
		ret = -EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;

		printk(KERN_NOTICE "从%lu读取了%u个字节到%#lx\n", p, count, (unsigned long)buf);
	}

	mutex_unlock(&dev->mutex);

	return ret;
}

ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;

	printk(KERN_ALERT "%s：%d:globalmem_write();\n", __FILE__, __LINE__);	//@

	if(p >= GLOBALMEM_SIZE)
		return 0;
	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;

	mutex_lock(&dev->mutex);

	if(copy_from_user(dev->mem + p, buf, count))
	{
		ret = -EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;

		printk(KERN_NOTICE "从%#lx写入了%u个字节到%lu\n", (unsigned long)buf, count, p);
	}

	mutex_unlock(&dev->mutex);

	return ret;
}

loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig)
{
	loff_t ret = 0;

	printk(KERN_ALERT "%s：%d:globalmem_llseek();\n", __FILE__, __LINE__);	//@

	switch(orig)
	{
		case 0:
			if(offset < 0)
			{
				ret = -EINVAL;
				break;
			}
			if(offset > GLOBALMEM_SIZE)
			{
				ret = -EINVAL;
				break;
			}
			filp->f_pos = offset;
			ret = filp->f_pos;
			break;
		case 1:
			if((filp->f_pos + offset) > GLOBALMEM_SIZE)
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

long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;

	printk(KERN_ALERT "%s：%d:globalmem_ioctl();\n", __FILE__, __LINE__);	//@

	switch(cmd)
	{
		case MEM_CLEAR:
			mutex_lock(&dev->mutex);
			memset(dev->mem, 0, GLOBALMEM_SIZE);
			mutex_unlock(&dev->mutex);

			printk(KERN_INFO "globalmem已清零\n");
			break;

		default:
			return -EINVAL;
	}

	return 0;
}

int globalmem_open(struct inode *inode, struct file *filp)
{
	struct globalmem_dev *dev = container_of(inode->i_cdev, struct globalmem_dev, cdev);

	printk(KERN_ALERT "%s：%d:globalmem_open();\n", __FILE__, __LINE__);	//@

	filp->private_data = dev;
	return 0;
}

int globalmem_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "%s：%d:globalmem_release();\n", __FILE__, __LINE__);	//@
	return 0;
}

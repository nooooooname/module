#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#include "globalmem.h"
#include "fops.h"

static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int, S_IRUGO);

struct globalmem_dev *globalmem_devp;

static const struct file_operations globalmem_fops = {
	.owner = THIS_MODULE,
	.llseek = globalmem_llseek,
	.read = globalmem_read,
	.write = globalmem_write,
	.unlocked_ioctl = globalmem_ioctl,
	.open = globalmem_open,
	.release = globalmem_release,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
	int err, devno = MKDEV(globalmem_major, index);

	cdev_init(&dev->cdev, &globalmem_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	if(err)
		printk(KERN_NOTICE "添加globalmem%d错误:%d\n", index, err);
}

static int __init globalmem_init(void)
{
	int i, ret;
	dev_t devno = MKDEV(globalmem_major, 0);

	if(globalmem_major)
		ret = register_chrdev_region(devno, DEVICE_NUM, "globalmem");
	else
	{
		ret = alloc_chrdev_region(&devno, 0, DEVICE_NUM, "globalmem");
		globalmem_major = MAJOR(devno);
	}
	if(ret < 0)
		return ret;

	globalmem_devp = kzalloc(sizeof(struct globalmem_dev) * DEVICE_NUM, GFP_KERNEL);
	if(!globalmem_devp)
	{
		ret = -ENOMEM;
		goto fail_malloc;
	}

	for(i = 0; i < DEVICE_NUM; i++)
	{
		mutex_init(&(globalmem_devp + i)->mutex);
		globalmem_setup_cdev(globalmem_devp + i, i);
	}

	return 0;

fail_malloc:
	unregister_chrdev_region(devno, DEVICE_NUM);
	return ret;
}

static void __exit globalmem_exit(void)
{
	int i;
	for(i = 0; i < DEVICE_NUM; i++)
		cdev_del(&(globalmem_devp + i)->cdev);
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), DEVICE_NUM);
}

module_init(globalmem_init);
module_exit(globalmem_exit);
MODULE_LICENSE("GPL v2");

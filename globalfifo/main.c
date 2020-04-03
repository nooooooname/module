#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>

#include "globalfifo.h"
#include "fops.h"

static int globalfifo_major = GLOBALFIFO_MAJOR;
module_param(globalfifo_major, int, S_IRUGO);

struct globalfifo_dev *globalfifo_devp;

static const struct file_operations globalfifo_fops = {
	.owner = THIS_MODULE,
	.llseek = globalfifo_llseek,
	.read = globalfifo_read,
	.write = globalfifo_write,
	.unlocked_ioctl = globalfifo_ioctl,
	.open = globalfifo_open,
	.release = globalfifo_release,
	.poll = globalfifo_poll,
};

static void globalfifo_setup_cdev(struct globalfifo_dev *dev, int index)
{
	int err, devno = MKDEV(globalfifo_major, index);

	cdev_init(&dev->cdev, &globalfifo_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	if(err)
		printk(KERN_NOTICE "添加globalfifo%d错误:%d\n", index, err);
}

static int __init globalfifo_init(void)
{
	int i, ret;
	dev_t devno = MKDEV(globalfifo_major, 0);

	if(globalfifo_major)
		ret = register_chrdev_region(devno, DEVICE_NUM, "globalfifo");
	else
	{
		ret = alloc_chrdev_region(&devno, 0, DEVICE_NUM, "globalfifo");
		globalfifo_major = MAJOR(devno);
	}
	if(ret < 0)
		return ret;

	globalfifo_devp = kzalloc(sizeof(struct globalfifo_dev) * DEVICE_NUM, GFP_KERNEL);
	if(!globalfifo_devp)
	{
		ret = -ENOMEM;
		goto fail_malloc;
	}

	for(i = 0; i < DEVICE_NUM; i++)
	{
		mutex_init(&(globalfifo_devp + i)->mutex);
		init_waitqueue_head(&(globalfifo_devp + i)->r_wait);
		init_waitqueue_head(&(globalfifo_devp + i)->w_wait);
		globalfifo_setup_cdev(globalfifo_devp + i, i);
	}

	return 0;

fail_malloc:
	unregister_chrdev_region(devno, DEVICE_NUM);
	return ret;
}

static void __exit globalfifo_exit(void)
{
	int i;
	for(i = 0; i < DEVICE_NUM; i++)
		cdev_del(&(globalfifo_devp + i)->cdev);
	kfree(globalfifo_devp);
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), DEVICE_NUM);
}

module_init(globalfifo_init);
module_exit(globalfifo_exit);
MODULE_LICENSE("GPL v2");

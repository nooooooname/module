#include <linux/module.h>
#include <linux/slab.h>

#include "second.h"
#include "second_fops.h"

static int second_major = SECOND_MAJOR;
module_param(second_major, int, S_IRUGO);

struct second_dev *second_devp;

static const struct file_operations second_fops = {
	.owner = THIS_MODULE,
	.open = second_open,
	.release = second_release,
	.read = second_read
};

static void second_setup_cdev(struct second_dev *dev, int index)
{
	int err;
	dev_t devno = MKDEV(second_major, index);

	cdev_init(&dev->cdev, &second_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	if(err)
		printk(KERN_ERR "添加second设备失败！\n");
}

static int __init second_init(void)
{
	dev_t devno = MKDEV(second_major, 0);
	int ret = register_chrdev_region(devno, 1, "second");
	if(ret < 0)
		return ret;

	second_devp = kzalloc(sizeof(struct second_dev), GFP_KERNEL);
	if(!second_devp)
	{
		ret = -ENOMEM;
		goto fail_malloc;
	}

	second_setup_cdev(second_devp, 0);

	return 0;

fail_malloc:
	unregister_chrdev_region(devno, 1);
	return ret;
}

static void __exit second_exit(void)
{
	cdev_del(&second_devp->cdev);
	kfree(second_devp);
	unregister_chrdev_region(MKDEV(second_major, 0), 1);
}

module_init(second_init);
module_exit(second_exit);
MODULE_LICENSE("GPL");

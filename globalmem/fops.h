#ifndef _FOPS_H
#define _FOPS_H

ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos);
ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos);
loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig);
long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
int globalmem_open(struct inode *inode, struct file *filp);
int globalmem_release(struct inode *inode, struct file *filp);

#endif

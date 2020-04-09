#ifndef _FOPS_H
#define _FOPS_H

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/poll.h>

ssize_t globalfifo_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos);
ssize_t globalfifo_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos);
loff_t globalfifo_llseek(struct file *filp, loff_t offset, int orig);
long globalfifo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
int globalfifo_open(struct inode *inode, struct file *filp);
int globalfifo_release(struct inode *inode, struct file *filp);
unsigned int globalfifo_poll(struct file *filp, poll_table *wait);
int globalfifo_fasync(int fd, struct file *filp, int mode);

#endif

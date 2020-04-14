#ifndef _SECOND_FOPS_H
#define _SECOND_FOPS_H

#include <linux/fs.h>
#include <linux/types.h>

int second_open(struct inode *inode, struct file *filp);
int second_release(struct inode *inode, struct file *filp);
ssize_t second_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos);

#endif

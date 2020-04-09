#ifndef _GLOBALFIFO_H
#define _GLOBALFIFO_H

#define GLOBALFIFO_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALFIFO_MAJOR 231
#define DEVICE_NUM 10

struct globalfifo_dev
{
	struct cdev cdev;
	unsigned int current_len;
	unsigned char mem[GLOBALFIFO_SIZE];
	struct mutex mutex;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
	size_t offset;
	struct fasync_struct *async_queue;
};

#endif

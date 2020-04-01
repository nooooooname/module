#ifndef _GLOBALMEM_H
#define _GLOBALMEM_H

#define GLOBALMEM_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALMEM_MAJOR 230
#define DEVICE_NUM 10

struct globalmem_dev
{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
	struct mutex mutex;
};

#endif

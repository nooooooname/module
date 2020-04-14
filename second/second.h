#ifndef _SECOND_H
#define _SECOND_H

#include <linux/cdev.h>

#define SECOND_MAJOR 232

struct second_dev
{
	struct cdev cdev;
	atomic_t counter;
	struct timer_list s_timer;
};

#endif

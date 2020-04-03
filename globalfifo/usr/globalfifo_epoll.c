#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>

#define FIFO_CLEAR 0x1
#define TIME_MS 10000

int main(void)
{
	int fd;

	fd = open("/dev/globalfifo", O_RDONLY | O_NONBLOCK);
	if(fd != -1)
	{
		int epfd, err;
		struct epoll_event ev_globalfifo;
		bzero(&ev_globalfifo, sizeof(struct epoll_event));

		if(ioctl(fd, FIFO_CLEAR, 0) < 0)
			printf("ioctl命令执行失败！\n");

		epfd = epoll_create(1);
		if(epfd < 0)
		{
			perror("epoll_create()");
			return epfd;
		}

		ev_globalfifo.events = EPOLLIN | EPOLLPRI;

		err = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev_globalfifo);
		if(err < 0)
		{
			perror("epoll_ctl()");
			return err;
		}
		err = epoll_wait(epfd, &ev_globalfifo, 1, TIME_MS);
		if(err < 0)
			perror("epoll_wait()");
		else if(err == 0)
			printf("globalfifo在%d秒内无数据输入。\n", TIME_MS / 1000);
		else
			printf("globalfifo不是空的。\n");
		err = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev_globalfifo);
		if(err < 0)
			perror("epoll_ctl()");
	}
	else
		printf("设备文件/dev/globalfifo打开失败！\n");

	return 0;
}

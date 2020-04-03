#include <sys/select.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#define FIFO_CLEAR 0x1
#define BUFFER_LEN 20

int main(void)
{
	int fd, num;
	unsigned long out_num = 0;
	char rd_ch[BUFFER_LEN];
	fd_set rfds, wfds;

	fd = open("/dev/globalfifo", O_RDONLY | O_NONBLOCK);
	if(fd != -1)
	{
		if(ioctl(fd, FIFO_CLEAR, 0) < 0)
			printf("ioctl命令执行失败！\n");

		while(1)
		{
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_SET(fd, &rfds);
			FD_SET(fd, &wfds);

			select(fd + 1, &rfds, &wfds, NULL, NULL);
			if(FD_ISSET(fd, &rfds))
				printf("%d.Poll监视器：可以读取。\n", ++out_num);
			if(FD_ISSET(fd, &wfds))
				printf("%d.Poll监视器：可以写入。\n", ++out_num);

			sleep(1);
		}
	}
	else
		printf("设备打开失败！\n");

	return 0;
}

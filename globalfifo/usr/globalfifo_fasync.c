#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#define BUF_LEN 4097

int fd;

void sigio_handler(int signum)
{
	static int count = 0;
	printf("%d>>> 收到信号SIGIO(%d)，fd = %d\n", ++count, SIGIO, fd);

	char buf[BUF_LEN];
	unsigned int len = read(fd, buf, BUF_LEN - 1);
	buf[len] = '\0';
	printf("读取了%d字节，内容为：%s\n", len, buf);
}

int main(int argc, char *argv[])
{
	int oflags;
	fd = open(argc > 1 ? argv[1] : "/dev/globalfifo", O_RDWR, S_IRUSR | S_IWUSR);
	if(fd != -1)
	{
		signal(SIGIO, sigio_handler);
		fcntl(fd, F_SETOWN, getpid());
		oflags = fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, FASYNC | oflags);
		while(true);
	}
	else
		printf("设备文件打开失败！\n");

	return 0;
}

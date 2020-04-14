#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#define CHRDEV "/dev/second"

int main(int argc, char *argv[])
{
	int fd, counter = 0, old_counter = 0;
	char *fname = argc > 1 ? argv[1] : CHRDEV;

	fd = open(fname, O_RDONLY);
	if(fd != -1)
		while(true)
		{
			read(fd, &counter, sizeof(int));
			if(counter != old_counter)
			{
				printf("%s打开后的第%d秒\n", fname, counter);
				old_counter = counter;
			}
		}
	else
		printf("设备打开失败！\n");

	return 0;
}

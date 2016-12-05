#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/errno.h>

#define IOCTL_SWITCH_ON     0
#define IOCTL_SWITCH_OFF    1

int main(void) {
    unsigned char currentNum = 0;
    int fd;
    int i = 0;
    int ret = 0;

    fd = open("/dev/gpio_switch", O_WRONLY);
    if (fd < 0) {
        printf("Open device file fail!\n");
        return -1;
    }

    while (1) {
        currentNum = currentNum%16;

        for (i=0; i<4; i++) {
            if ((currentNum>>i)&0x01) {
                ret = ioctl(fd, IOCTL_SWITCH_OFF, i);
            } else {
                ret = ioctl(fd, IOCTL_SWITCH_ON, i);
            }
            if (-1 == ret) {
                printf("Set led fail!\n");
                return -1;
            }
        }

        currentNum++;
        sleep(1);
    }

    return 0;
}

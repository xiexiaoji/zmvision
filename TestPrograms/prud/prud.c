#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/errno.h>
#include "prud.h"

void ShowHelp ( void ) {
    printf("prud [opts]\n");
    printf("\topts:\n");
    printf("\t-u:PRU num, 0:ICSS0_PRU0, 1:ICSS0_PRU1, 2:ICSS1_PRU0, 3:ICSS1_PRU1\n");
    printf("\t-t:Ioctl command type,\n");
    printf("\t\t0:Enable, 1:Disable, 2:Reset, 3:Load firmware\n");
    printf("\t\t4:Read data, 5:Write data, 6:Send event, 7:Clear event\n");
    printf("\t-f:File name for firmware or data\n");
    printf("\t-e:Event number\n");
    printf("\t-h:Show this help\n");
    return;
}

int main ( int argc, char * argv[] ) {
    int ch;
    unsigned int pruId = PRUSS0_PRU0;
    unsigned int cmdType = PRU_IOCTL_ENABLE;
    char filePath[1024] = {0};
    unsigned int eventNum = 0;
    struct pru_ioctl_data pruData;
    int fd;

    while ((ch = getopt(argc, argv, "u:t:f:e:h")) != -1) {
        switch (ch) {
            case 'u':
                pruId = atoi(optarg);
                break;

            case 't':
                cmdType = 3100 + atoi(optarg);
                break;

            case 'f':
                strcpy(filePath, (const char *)optarg);
                break;

            case 'e':
                eventNum = atoi(optarg);
                break;

            case 'h':
                ShowHelp();
                return 0;

            default:
                ShowHelp();
                return 0;
        }
    }

    fd = open(PRU_DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("Open device file fail!\n");
        return -1;
    }

    memset(&pruData, 0x00, sizeof(struct pru_ioctl_data));
    pruData.pruId = pruId;

    switch (cmdType) {
        case PRU_IOCTL_ENABLE:
            if (-1 == ioctl(fd, PRU_IOCTL_ENABLE, &pruData)) {
                printf("Enable pru%d fail!\n", pruData.pruId);
                close(fd);
                return -1;
            }
            break;

        case PRU_IOCTL_DISABLE:
            if (-1 == ioctl(fd, PRU_IOCTL_DISABLE, &pruData)) {
                printf("Disable pru%d fail!\n", pruData.pruId);
                close(fd);
                return -1;
            }
            break;

        case PRU_IOCTL_RESET:
            if (-1 == ioctl(fd, PRU_IOCTL_RESET, &pruData)) {
                printf("Reset pru%d fail!\n", pruData.pruId);
                close(fd);
                return -1;
            }
            break;

        case PRU_IOCTL_LOAD_FW:
            {
                FILE * fw_fd;
                int i = 0;
                fw_fd = fopen(filePath, "r");
                if (fw_fd < 0) {
                    printf("Open firmware fail!\n");
                    close(fd);
                    return -1;
                }
                while (fread(pruData.data.pruFw.buf + i, sizeof(char), 1, fw_fd)) {
                    i++;
                    if (i > PRU_FW_MAX_LEN) {
                        printf("Firmware too large!\n");
                        fclose(fw_fd);
                        close(fd);
                        return -1;
                    }
                }
                fclose(fw_fd);
                pruData.data.pruFw.len = i;
                if (-1 == ioctl(fd, PRU_IOCTL_LOAD_FW, &pruData)) {
                    printf("Load pru%d firmware fail!\n", pruData.pruId);
                    close(fd);
                    return -1;
                }
            }
            break;

        case PRU_IOCTL_READ_MEM:
            break;

        case PRU_IOCTL_WRITE_MEM:
            break;

        case PRU_IOCTL_SEND_EVT:
            pruData.data.eventNum = eventNum;
            if (-1 == ioctl(fd, PRU_IOCTL_SEND_EVT, &pruData)) {
                printf("Send event %d to pru%d fail!\n", pruData.data.eventNum, pruData.pruId);
                close(fd);
                return -1;
            }
            break;

        case PRU_IOCTL_CLEAR_EVT:
            pruData.data.eventNum = eventNum;
            if (-1 == ioctl(fd, PRU_IOCTL_CLEAR_EVT, &pruData)) {
                printf("Clear event %d on pru%d fail!\n", pruData.data.eventNum, pruData.pruId);
                close(fd);
                return -1;
            }
            break;
        
        default:
            break;
    }

    close(fd);
    return 0;
}

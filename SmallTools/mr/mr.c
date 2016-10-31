#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifndef OK
#define OK      0
#define FAIL    -1
#endif

int HexStrToNum (char * str, unsigned int * hex) {
    unsigned int res = 0;
    unsigned int temp = 0;
    int i = 0;

    if ((NULL == str) || (strlen(str) > 8)) {
        return FAIL;
    }

    for (i=0; i<strlen(str); i++) {
        if ((str[i] >= '0') && (str[i] <='9')) {
            temp = (unsigned int)(str[i] - '0');
        } else if ((str[i] >= 'a') && (str[i] <='f')) {
            temp = (unsigned int)(str[i] - 'a' + 10);
        } else if ((str[i] >= 'A') && (str[i] <='F')) {
            temp = (unsigned int)(str[i] - 'A' + 10);
        } else {
            return FAIL;
        }
        res = res | (temp << 4*(strlen(str)-i-1));
    }

    *hex = res;

    return OK;
}

int main (int argc, char * argv[]) {
    unsigned int addr = 0x00000000;
    unsigned int len = 0;
    const char memBaseDev[] = "/dev/mem";
    unsigned int * memBase = NULL;
    unsigned int * memStart = NULL;
    int fd = 0;
    int printOffset = 0;
    int i = 0;
    int j = 0;

    if (3 != argc) {
        return FAIL;
    }

    if (OK != HexStrToNum(argv[1], &addr)) {
        printf("Input args error!\n");
        return FAIL;
    }
    if (0 != addr%4) {
        printf("Input args error!\n");
        return FAIL;
    }

    len = (unsigned int)atoi((const char *)argv[2]);

    fd = open(memBaseDev, O_RDONLY);
    if (fd < 0) {
        return FAIL;
    }

    memBase = (unsigned int *)mmap(0, 0x2000, PROT_READ, MAP_SHARED, fd, addr&0xfffff000);
    if (NULL == memBase) {
        return FAIL;
    }
    memStart = (unsigned int *)((unsigned int)memBase + (addr&0x00000fff));
    printOffset = ((unsigned int)memStart%16)/4;

    printf("Start address:0x%08x, length:%d\n\n", addr, len);
    printf("0x%08x : ", addr&0xfffffff0);
    for (i=0; i<printOffset; i++) {
        printf("         ");
    }

    for (i=0; i<len; i++) {
        printf("%08x ", *(memStart + i));
        if (3 == (i+printOffset)%4) {
            printf("\n");
            j ++;
            printf("0x%08x : ", (addr&0xfffffff0)+(16*j));
        }
    }
    printf("\n");

    munmap(memBase, 0x2000);

    return OK;
}

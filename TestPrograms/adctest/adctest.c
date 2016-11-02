#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "adctest.h"

#define ADC0_STEPDELAY_CONFIG    0x00000000
#define ADC1_STEPDELAY_CONFIG    0x00000000

void ClearReg( unsigned int * regBase, unsigned int reg ) {
    volatile unsigned int * regAddr = (unsigned int *)((unsigned int)regBase + reg);
    *regAddr = 0x00000000;
    return;
}

void SetReg( unsigned int * regBase, unsigned int reg, unsigned int value ) {
    volatile unsigned int * regAddr = (unsigned int *)((unsigned int)regBase + reg);
    *regAddr = value;
    return;
}

//Bit 1 in mask means this bit in register will be set
void SetRegBitsWithMask( unsigned int * regBase, unsigned int reg, unsigned int mask, unsigned int bits ) {
    volatile unsigned int * regAddr = (unsigned int *)((unsigned int)regBase + reg);
    *regAddr = *regAddr & (~mask);
    *regAddr = *regAddr | bits;
    return;
}

unsigned int GetReg( unsigned int * regBase, unsigned int reg) {
    volatile unsigned int * regAddr = (unsigned int *)((unsigned int)regBase + reg);
    return *regAddr;
}

void AdcFifo0Check( int adcSelect ) {
    const char memBase[] = "/dev/mem";
    unsigned int fifo0Count = 0;
    unsigned int fifo0Data = 0x00000000;
    unsigned short fifo0ShortData = 0x0000;
    unsigned int * adcRegBase = NULL;
    char buf[16];
    int fd_mem = 0;
    int fd = 0;
    int i = 0;
    int totalWrite = 100000;
    unsigned int delayMs = 10;

    fd_mem = open(memBase, O_RDWR);
    if (fd_mem < 0) {
        return;
    }
    /* Init ADC register */
    if (ADC_SELECT_ADC0 == adcSelect) {
        adcRegBase = (unsigned int *)mmap(0, ADC_REGISTER_LEN, 
                                           PROT_READ|PROT_WRITE, MAP_SHARED, 
                                           fd_mem, ADC0_REGISTER_ADDR);
    } else {
        adcRegBase = (unsigned int *)mmap(0, ADC_REGISTER_LEN, 
                                           PROT_READ|PROT_WRITE, MAP_SHARED, 
                                           fd_mem, ADC1_REGISTER_ADDR);
    }
    if (NULL == adcRegBase) {
        close(fd_mem);
        return;
    }

    fd = open("/tmp/adcfifo0data", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        printf("/tmp/adcfifo0data file open error!\n");
        munmap(adcRegBase, ADC_REGISTER_LEN);
        close(fd_mem);
        return;
    }

    while (1) {
        fifo0Count = GetReg(adcRegBase, ADC_FIFO0COUNT);
        for (i=0; i<fifo0Count; i++) {
            fifo0Data = GetReg(adcRegBase, ADC_FIFO0DATA) & ADC_FIFOMASK;
            fifo0ShortData = (unsigned short)fifo0Data;
            memset(buf, 0x00, 6);
            sprintf(buf, "%d ", fifo0ShortData);
            write(fd, buf, strlen(buf));

            totalWrite --;
            if (0 == totalWrite) {
                close(fd);
                close(fd_mem);
                munmap(adcRegBase, ADC_REGISTER_LEN);
                return;
            }
        }
        //usleep(delayMs * 1000);
    }

    close(fd);
    close(fd_mem);
    munmap(adcRegBase, ADC_REGISTER_LEN);
    return;
}

int AdcInit( int adcSelect, int adcInput ) {
    const char memBase[] = "/dev/mem";
    unsigned int * controlModuleRegBase = NULL;
    unsigned int * adcRegBase = NULL;
    int fd = 0;

    fd = open(memBase, O_RDWR);
    if (fd < 0) {
        return FAIL;
    }

    /* Set ADC Capture trigger */
    controlModuleRegBase = (unsigned int *)mmap(0, CONTROL_MODULE_REGISTER_LEN, 
                                                PROT_READ|PROT_WRITE, MAP_SHARED, 
                                                fd, CONTROL_MODULE_REGISTER_ADDR);
    if (NULL == controlModuleRegBase) {
        close(fd);
        return FAIL;
    }

    if (ADC_SELECT_ADC0 == adcSelect) {
        ClearReg(controlModuleRegBase, ADC0_EVT_CAPT);
    } else {
        ClearReg(controlModuleRegBase, ADC1_EVT_CAPT);
    }
    munmap(controlModuleRegBase, CONTROL_MODULE_REGISTER_LEN);

    /* Init ADC register */
    if (ADC_SELECT_ADC0 == adcSelect) {
        adcRegBase = (unsigned int *)mmap(0, ADC_REGISTER_LEN, 
                                           PROT_READ|PROT_WRITE, MAP_SHARED, 
                                           fd, ADC0_REGISTER_ADDR);
    } else {
        adcRegBase = (unsigned int *)mmap(0, ADC_REGISTER_LEN, 
                                           PROT_READ|PROT_WRITE, MAP_SHARED, 
                                           fd, ADC1_REGISTER_ADDR);
    }
    if (NULL == adcRegBase) {
        close(fd);
        return FAIL;
    }

    if (ADC_SELECT_ADC0 == adcSelect) {
        SetRegBitsWithMask(adcRegBase, ADC_CTRL, 0x000001FF, 0x00000104);
        SetReg(adcRegBase, ADC_SYSCONFIG, 0x00000000);
        SetReg(adcRegBase, ADC_IRQEN_CLR, 0x000007FF);
        SetReg(adcRegBase, ADC_DMAEN_CLR, 0x00000003);
        SetReg(adcRegBase, ADC0_TS_CHARGE_STEPCONFIG, 0x00000000);
        SetReg(adcRegBase, ADC0_TS_CHARGE_DELAY, 0x00000000);
        SetReg(adcRegBase, ADC_CLKDIV, 0x00000010);
        if (ADC_INPUT_SINGLE == adcInput) {
            SetReg(adcRegBase, ADC_STEPCONFIG1, 0x00040001);
            SetReg(adcRegBase, ADC_STEPDELAY1, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPEN, 0x00000002);
        } else if (ADC_INPUT_DIFF == adcInput) {
            SetReg(adcRegBase, ADC_STEPCONFIG1, 0x02008001);
            SetReg(adcRegBase, ADC_STEPDELAY1, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPEN, 0x00000002);
        } else {
            SetReg(adcRegBase, ADC_STEPCONFIG1, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY1, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG2, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY2, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG3, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY3, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG4, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY4, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG5, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY5, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG6, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY6, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG7, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY7, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG8, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY8, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG9, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY9, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG10, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY10, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG11, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY11, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG12, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY12, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG13, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY13, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG14, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY14, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG15, 0x00140001);
            SetReg(adcRegBase, ADC_STEPDELAY15, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPCONFIG16, 0x001C0001);
            SetReg(adcRegBase, ADC_STEPDELAY16, ADC0_STEPDELAY_CONFIG);
            SetReg(adcRegBase, ADC_STEPEN, 0x0001FFFE);
        }
        SetRegBitsWithMask(adcRegBase, ADC_CTRL, 0x000001FF, 0x00000101);
    } else {
        SetRegBitsWithMask(adcRegBase, ADC_CTRL, 0x00000177, 0x00000140);
        SetReg(adcRegBase, ADC_SYSCONFIG, 0x00000000);
        SetReg(adcRegBase, ADC_DMAEN_CLR, 0x00000003);
        SetReg(adcRegBase, ADC_CLKDIV, 0x00000010);
        if (ADC_INPUT_SINGLE == adcInput) {
            SetReg(adcRegBase, ADC_IDLECONFIG, 0x00440000);
            SetReg(adcRegBase, ADC_STEPCONFIG1, 0x00040001);
            SetReg(adcRegBase, ADC_STEPDELAY1, ADC1_STEPDELAY_CONFIG);
        } else {
            SetReg(adcRegBase, ADC_IDLECONFIG, 0x02440000);
            SetReg(adcRegBase, ADC_STEPCONFIG1, 0x02008001);
            SetReg(adcRegBase, ADC_STEPDELAY1, ADC1_STEPDELAY_CONFIG);
        }
        SetReg(adcRegBase, ADC_STEPEN, 0x00000002);
        SetRegBitsWithMask(adcRegBase, ADC_CTRL, 0x00000177, 0x00000141);
    }

    munmap(adcRegBase, ADC_REGISTER_LEN);
    close(fd);

    return OK;
}

void ShowHelp( void ) {
    printf("adctest [opts]\n");
    printf("opts:\n");
    printf("\t-0 : Select ADC0(Default)\n");
    printf("\t-1 : Select ADC1\n");
    printf("\t-s : Single input in channel 1(Default)\n");
    printf("\t-d : Diff input of channel 1 and channel 2\n");
    printf("\t-m : Multi input of ADC0's channel 3 and channel 4\n");
    printf("\t-h : Show help\n");
    return;
}

int main( int argc, char * argv[] ) {
    int ch;
    int adcSelect = ADC_SELECT_ADC0;
    int adcInput = ADC_INPUT_SINGLE;
    struct timeval tTime;
    int startTime = 0;
    int endTime = 0;

    while ((ch = getopt(argc, argv, "01sdmh")) != -1) {
        switch(ch) {
            case '0':
                adcSelect = ADC_SELECT_ADC0;
                break;

            case '1':
                adcSelect = ADC_SELECT_ADC1;
                break;

            case 's':
                adcInput = ADC_INPUT_SINGLE;
                break;

            case 'd':
                adcInput = ADC_INPUT_DIFF;
                break;

            case 'm':
                adcSelect = ADC_SELECT_ADC0;
                adcInput = ADC_INPUT_MULTI;
                break;

            case 'h':
                ShowHelp();
                return 0;

            default:
                ShowHelp();
                return 0;
        }
    }
    printf("Init adc\n");

    if (OK == AdcInit(adcSelect, adcInput)) {
        sleep(1);
        printf("Get fifo data...\n");
        gettimeofday(&tTime, NULL);
        startTime = 1000000L*tTime.tv_sec + tTime.tv_usec;
        AdcFifo0Check(adcSelect);
        gettimeofday(&tTime, NULL);
        endTime = 1000000L*tTime.tv_sec + tTime.tv_usec;
        printf("Get fifo data finish, spend time %d\n", endTime-startTime);
    } else {
        printf("ADC init fail!\n");
    }

    return 0;
}

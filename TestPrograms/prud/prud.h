#ifndef __PRUD_H__
#define __PRUD_H__

#define PRU_DEV_NAME    "/dev/pru"

/* PRU CONTROL */
#define PRU_IOCTL_ENABLE    3100
#define PRU_IOCTL_DISABLE   3101
#define PRU_IOCTL_RESET     3102
#define PRU_IOCTL_LOAD_FW   3103
#define PRU_IOCTL_READ_MEM  3104
#define PRU_IOCTL_WRITE_MEM 3105
#define PRU_IOCTL_SEND_EVT  3106
#define PRU_IOCTL_CLEAR_EVT 3107

#define PRUSS0_PRU0 0
#define PRUSS0_PRU1 1
#define PRUSS1_PRU0 2
#define PRUSS1_PRU1 3

#define PRU_FW_MAX_LEN  (4*1024)

enum pruss_mem_id {

	PRUSS1_MEM_DATARAM0 = 0,
	PRUSS1_MEM_DATARAM1,
	PRUSS1_MEM_SHAREDRAM,
	PRUSS1_MEM_INTC,
	PRUSS1_MEM_PRU0CONTROL,
	PRUSS1_MEM_PRU0DEBUG,
	PRUSS1_MEM_PRU1CONTROL,
	PRUSS1_MEM_PRU1DEBUG,
	PRUSS1_MEM_CFG,
	PRUSS1_MEM_PRU0IRAM,
	PRUSS1_MEM_PRU1IRAM,

	PRUSS0_MEM_DATARAM0,
	PRUSS0_MEM_DATARAM1,
	PRUSS0_MEM_INTC,
	PRUSS0_MEM_PRU0CONTROL,
	PRUSS0_MEM_PRU0DEBUG,
	PRUSS0_MEM_PRU1CONTROL,
	PRUSS0_MEM_PRU1DEBUG,
	PRUSS0_MEM_CFG,
	PRUSS0_MEM_PRU0IRAM,
	PRUSS0_MEM_PRU1IRAM,

	PRUSS_MEM_MAX,
};

struct pru_fw {
    unsigned int len;
    char buf[PRU_FW_MAX_LEN];
};

struct pru_write_data {
    enum pruss_mem_id pru_mem;
    unsigned int wordOffset;
    unsigned int bufLen;
    char buf[PRU_FW_MAX_LEN];
};

struct pru_read_data {
    enum pruss_mem_id pru_mem;
    unsigned int wordOffset;
    unsigned int bufLen;
    char * buf;
};

struct pru_ioctl_data {
    unsigned int pruId;
    union {
        struct pru_fw pruFw;
        struct pru_write_data writeData;
        struct pru_read_data readData;
        unsigned int eventNum;
    } data;
};

#endif /* __PRUD_H__ */

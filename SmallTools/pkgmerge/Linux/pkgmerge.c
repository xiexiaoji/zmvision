#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(_MSC_VER)
#define BOOT2_START_STR "$A90000400,\n"
#define FW_START_STR    "$A90010000,\n"
#elif defined(__GNUC__)
#define BOOT2_START_STR "$A90000400,\r\n"
#define FW_START_STR    "$A90010000,\r\n"
#else
#define BOOT2_START_STR "$A90000400,"
#define FW_START_STR    "$A90010000,"
#endif

int main(int argc, char * argv[]) {
    FILE * pfSrc1;
    FILE * pfSrc2;
    FILE * pfHex;
    char buf[256]={0};

    if (4 != argc) {
        printf("Params error!\n");
        return -1;
    }

    pfSrc1 = fopen(argv[1], "r");
    if (NULL == pfSrc1) {
        printf("Open input file 1 %s failed!\n", argv[1]);
        return -1;
    }

    pfSrc2 = fopen(argv[2], "r");
    if (NULL == pfSrc2) {
        printf("Open input file 2 %s failed!\n", argv[2]);
        fclose(pfSrc1);
        return -1;
    }

    pfHex = fopen(argv[3], "w");
    if (NULL == pfHex) {
        printf("Open output file 1 %s failed!\n", argv[1]);
        return -1;
    }

	memset(buf, 0x00, 256);
	while (NULL != fgets(buf, 256, pfSrc1)) {
		if ((strlen(FW_START_STR) != strlen(buf)) || (0 != strcmp(FW_START_STR, buf))) {
			fputs(buf, pfHex);
		}
		else {
			break;
		}
		memset(buf, 0x00, 256);
	}

	fgets(buf, 256, pfSrc2);
	memset(buf, 0x00, 256);
	while (NULL != fgets(buf, 256, pfSrc2)) {
		fputs(buf, pfHex);
		memset(buf, 0x00, 256);
	}

    fclose(pfSrc1);
    fclose(pfSrc2);
    fclose(pfHex);
    return 0;
}

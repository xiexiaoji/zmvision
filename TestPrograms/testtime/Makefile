TOOLCHAIN_PATH = /opt/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin
CROSS_COMPILE = arm-linux-gnueabihf-
CC = $(TOOLCHAIN_PATH)/$(CROSS_COMPILE)gcc
C_OBJ =
CFLAGS :=

TARGET = testtime

C_OBJ += testtime.c
CFLAGS += -lpthread -lrt

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(C_OBJ)

clean:
	rm -f $(TARGET)

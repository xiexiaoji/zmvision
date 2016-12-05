#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <bits/local_lim.h>

#ifndef OK
#define OK      0
#define FAIL    -1
#endif

int lastStatus = 0;

int GpioInit( void ) {
    int fd = 0;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        return FAIL;
    }
    write(fd, "186", 3);
    close(fd);
    fd = 0;

    fd = open("/sys/class/gpio/gpio186/direction", O_WRONLY | O_TRUNC);
    if (fd < 0) {
        return FAIL;
    }
    write(fd, "out", 3);
    close(fd);
    fd = 0;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        return FAIL;
    }
    write(fd, "183", 3);
    close(fd);
    fd = 0;

    fd = open("/sys/class/gpio/gpio183/direction", O_WRONLY | O_TRUNC);
    if (fd < 0) {
        return FAIL;
    }
    write(fd, "out", 3);
    close(fd);
    fd = 0;

    return OK;
}

void StatusChg( void ) {
    int fd3 = 0;
    int fd6 = 0;

    fd3 = open("/sys/class/gpio/gpio183/value", O_WRONLY | O_TRUNC);
    fd6 = open("/sys/class/gpio/gpio186/value", O_WRONLY | O_TRUNC);
    if (0 == lastStatus) {
        write(fd3, "1", 1);
        //write(fd6, "0", 1);
    } else if (5 == lastStatus) {
        write(fd6, "1", 1);
    } else if (3 == lastStatus) {
        write(fd3, "0", 1);
        //write(fd6, "1", 1);
    } else if (2 == lastStatus) {
        write(fd6, "0", 1);
    }
    close(fd3);
    close(fd6);

    lastStatus++;
    if (lastStatus >= 6) {
        lastStatus = 0;
    }
        
    return;
}

void SetRTThreadAttr(pthread_attr_t *attr, int ipriority, int istacksize){
    int ret;
    int policy, inher;
    struct sched_param param;

    int priority = ipriority;
    int stacksize = istacksize;

    //初始化线程属性
    pthread_attr_init(attr);
    //获取继承的调度策略
    ret = pthread_attr_getinheritsched(attr, &inher);
    if (ret != 0) {
        printf("pthread_attr_getinheritsched\n%s\n", strerror(ret));
        exit(1);
    }
    //
    if (inher == PTHREAD_EXPLICIT_SCHED) {
        //  printf("PTHREAD_EXPLICIT_SCHED\n");
    } else if (inher == PTHREAD_INHERIT_SCHED) {
        //  printf("PTHREAD_INHERIT_SCHED\n");
        inher = PTHREAD_EXPLICIT_SCHED;
    }
    //设置继承的调度策略
    //必需设置inher的属性为 PTHREAD_EXPLICIT_SCHED，否则设置线程的优先级会被忽略 </span>
    ret = pthread_attr_setinheritsched(attr, inher);
    if (ret != 0) {
        printf("pthread_attr_setinheritsched\n%s\n", strerror(ret));
        exit(1);
    }

    policy = SCHED_FIFO;        //在Ubuntu9.10上需要root权限
    //设置线程调度策略
    ret = pthread_attr_setschedpolicy(attr, policy);
    if (ret != 0) {
        printf(" pthread_attr_setschedpolicy\n%s\n", strerror(ret));
        exit(1);
    }
    param.sched_priority = priority;
    //设置调度参数
    ret = pthread_attr_setschedparam(attr, &param);
    if (ret != 0) {
        printf(" pthread_attr_setschedparam\n%s\n", strerror(ret));
        exit(1);
    }
    //设置堆栈大小
    if(stacksize < PTHREAD_STACK_MIN){
        //  stacksize += PTHREAD_STACK_MIN;
        stacksize = PTHREAD_STACK_MIN;
    }
    ret = pthread_attr_setstacksize(attr, stacksize);
    if (ret != 0) {
        printf(" pthread_attr_set stacksize\n%s\n", strerror(ret));
        exit(1);
    }
}

int main( int argc, char * argv[] ) {
    timer_t timerid;
    pthread_attr_t attr;
    struct sigevent evp;
    struct itimerspec it;
    struct timespec it_interval;
    int interval = 50;  //default to 50ms

    if (OK != GpioInit()) {
        printf("fail to init gpio!\n");
        return -1;
    }

    if (argc >= 2) {
        interval = atoi(argv[1]);
        printf("interval = %dms\n", interval);
        if (0 == interval) {
            interval = 50;
        }
    }

    SetRTThreadAttr(&attr, 25, 20480);
    memset((void *)&evp, 0x00, sizeof(struct sigevent));
    evp.sigev_value.sival_int = 0;
    evp.sigev_notify = SIGEV_THREAD;
    evp.sigev_notify_function = (void*)StatusChg;
    evp.sigev_notify_attributes = &attr;

    if (timer_create(CLOCK_MONOTONIC, &evp, &timerid) == -1)
    {
        printf("fail to timer_create\n");
        return -1;
    }

    it_interval.tv_sec = interval/1000;
    it_interval.tv_nsec = (interval%1000)*1000000;
    printf("tv_sec=%d, tv_nsec=%d\n", it_interval.tv_sec, it_interval.tv_nsec);

    it.it_interval = it_interval;
    it.it_value= it_interval;//.tv_sec

    if (timer_settime(timerid, CLOCK_MONOTONIC, &it, NULL) == -1) {
        printf("fail to start timer\n");
        return -1;
    }

    while(1) {
        sleep(1000*1000*1000);
    }

    return 0;
}

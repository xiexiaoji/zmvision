#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <bits/local_lim.h>

int TimeCount = 0;
int lastTime = 0;
int startTime[10000];

void SingleProc( void ) {
    struct timeval tBegin;
    int currTime = 0;

    if (TimeCount < 10000) {
        gettimeofday(&tBegin, NULL);
        currTime = 1000000L*tBegin.tv_sec + tBegin.tv_usec;
        startTime[TimeCount] = currTime - lastTime;
        lastTime = currTime;
    }

    if (TimeCount == 10000) {
        int i = 0;
        int fd;

        fd = open("/tmp/startTime", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (-1 == fd) {
            printf("Open file err!\n");
        }
        for (i=0; i<10000; i++) {
            char buf[16];
            memset((void *)buf, 0x00, 16);
            sprintf(buf, "%d\n", startTime[i]);
            write(fd, buf, strlen((const char *)buf)+1);
        }
        close(fd);
    }

    TimeCount++;
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

int main( void ) {
    timer_t timerid;
    pthread_attr_t attr;
    struct sigevent evp;
    struct itimerspec it;
    struct timespec it_interval;

    SetRTThreadAttr(&attr, 25, 20480);
    memset((void *)&evp, 0x00, sizeof(struct sigevent));
    evp.sigev_value.sival_int = 0;
    evp.sigev_notify = SIGEV_THREAD;
    evp.sigev_notify_function = SingleProc;
    evp.sigev_notify_attributes = &attr;

    if (timer_create(CLOCK_MONOTONIC, &evp, &timerid) == -1)
    {
        printf("fail to timer_create\n");
    }

    it_interval.tv_sec = 0;
    it_interval.tv_nsec = 1000000;

    it.it_interval = it_interval;
    it.it_value= it_interval;//.tv_sec

    if (timer_settime(&timerid, CLOCK_MONOTONIC, &it, NULL) == -1) {
        printf("fail to start timer\n");
    }

    while(1) {
        sleep(1000*1000*1000);
    }

    return;
}

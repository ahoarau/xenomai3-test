#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <sstream>
// Xenomai 2 and 3
#include <xeno_config.h>
#if CONFIG_XENO_VERSION_MAJOR == 3
#include <xenomai/init.h>
#include <alchemy/mutex.h>
#include <alchemy/cond.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#elif CONFIG_XENO_VERSION_MAJOR == 2
#include <native/mutex.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/cond.h>
#endif

std::string to_string(int i)
{
    std::ostringstream os;
    os << i;
    return os.str();
}

static const int ntasks = 5;
RT_MUTEX m;
RT_COND c;
RT_TASK demo_task[ntasks];

int print_mutex_lock_status(int ret)
{
    switch(ret)
    {
        case -ETIMEDOUT: printf("ETIMEDOUT is returned if abs_timeout is reached before the mutex is available.\n");break;
        case -EWOULDBLOCK: printf("EWOULDBLOCK is returned if timeout is { .tv_sec = 0, .tv_nsec = 0 } and the mutex is not immediately available.\n");break;
        case -EINTR: printf("EINTR is returned if rt_task_unblock() was called for the current task.\n");break;
        case -EINVAL: printf("EINVAL is returned if mutex is not a valid mutex descriptor.\n");break;
        case -EIDRM: printf("EIDRM is returned if mutex is deleted while the caller was waiting on it. In such event, mutex is no more valid upon return of this service.\n");break;
        case -EPERM: printf("EPERM is returned if this service should block, but was not called from a Xenomai thread.\n");break;
    }
    return ret;
}

void demo(void *arg)
{
    RT_TASK *curtask = rt_task_self();
    RT_TASK_INFO curtaskinfo;
    rt_task_inquire(curtask,&curtaskinfo);
    // curtask=rt_task_self();
    RTIME now = rt_timer_read();
    //printf("%s cond wait      at %lld\n",curtaskinfo.name,now);
    int ret = print_mutex_lock_status(rt_cond_wait_until(&c,&m,now + 1000000000ULL));
    RTIME then = rt_timer_read();
    printf("%s cond wait done at %lld with ret %d\n",curtaskinfo.name,then-now,ret);
    //
    // // hello world
    // printf("Hello World!\n");
    //
    // // inquire current task
    // RT_MUTEX_INFO info;
    //
    //
    // printf("%s rt_mutex_acquire() -> %d\n",curtaskinfo.name,print_mutex_lock_status(rt_mutex_acquire(&m,TM_NONBLOCK)));
    // rt_mutex_inquire(&m,&info);
    // printf("%s mutex owned by %d\n",curtaskinfo.name,info.owner);
    // rt_task_sleep(rt_timer_ns2ticks(1E9));
    // rt_mutex_release(&m);
    // printf("%s rt_mutex_acquire() -> %d\n",curtaskinfo.name,print_mutex_lock_status(rt_mutex_acquire_timed(&m,TM_INFINITE)));
    // rt_mutex_inquire(&m,&info);
    // printf("%s mutex owned by %d\n",curtaskinfo.name,info.owner);
    // rt_mutex_release(&m);
    // printf("%s rt_mutex_acquire() -> %d\n",curtaskinfo.name,print_mutex_lock_status(rt_mutex_acquire(&m,TM_INFINITE)));
    // rt_mutex_release(&m);
    // printf("%s rt_mutex_acquire() -> %d\n",curtaskinfo.name,print_mutex_lock_status(rt_mutex_acquire(&m,TM_INFINITE)));
    // rt_mutex_release(&m);
    // // print task name
    // printf("Task name : %s \n", curtaskinfo.name);

    rt_task_sleep(rt_timer_ns2ticks(2E9));
}



int main(int argc, char* argv[])
{

#if CONFIG_XENO_VERSION_MAJOR == 3
    char *const* argvp = const_cast<char*const*>(argv);
    xenomai_init(&argc,&argvp);
#endif

    int r = rt_mutex_create(&m,0);
    rt_cond_create(&c,0);
    printf("rt_mutex_create : %s\n",(r == 0 ? "OK" : "ERROR"));

    char  str[ntasks][10] ;
    printf("Starting test\n");
    rt_task_sleep(rt_timer_ns2ticks(5E8));

    for(int i = 0; i < ntasks; i++)
    sprintf(str[i],("Hello " + to_string(i)).c_str());

    for(int i = 0; i < ntasks; i++)
    rt_task_create(&demo_task[i], str[i], 0, 50 + i, T_JOINABLE);

    for(int i = 0; i < ntasks; i++)
    rt_task_start(&demo_task[i], &demo, 0);

    for(int i = 0; i < ntasks; i++)
    rt_task_join(&demo_task[i]);

    // std::cout << "Press Enter to Exit";
    // std::cin.ignore();
    return 0;
}
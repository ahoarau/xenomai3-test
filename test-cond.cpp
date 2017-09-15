#include <stdio.h>
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

RT_MUTEX mutex;
RT_COND cond;
RT_TASK demo_task;

void demo(void *arg)
{
    RTIME now = rt_timer_read();
    int ret = rt_cond_wait_until(&cond,&mutex,now + 10000000000ULL);
    RTIME then = rt_timer_read();

    printf("rt_cond_wait_until took %lld ticks, ret %d\n",then-now,ret);

    rt_task_sleep(rt_timer_ns2ticks(2E9));
}

int main(int argc, char* argv[])
{
    printf("rt_mutex_create -> %d\n",rt_mutex_create(&mutex,"Mutex"));
    printf("rt_cond_create  -> %d\n",rt_cond_create(&cond,"Cond"));
    int prio = 0;
    printf("rt_task_spawn   -> %d\n",rt_task_spawn(&demo_task, "TestCond", 0, prio, T_JOINABLE,&demo,NULL));
    printf("rt_task_join    -> %d\n",rt_task_join(&demo_task));
    return 0;
}
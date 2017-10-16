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
RT_TASK task1,task2;

void wait(void *arg)
{
    RTIME now = rt_timer_read();
	rt_mutex_acquire(&mutex,TM_INFINITE);

    int ret = rt_cond_wait(&cond,&mutex,TM_INFINITE);
    RTIME then = rt_timer_read();
	rt_mutex_release(&mutex);

    printf("task %p waited %lld ticks, ret %d\n",rt_task_self(),then-now,ret);
}

void broadcast(void *arg)
{
	rt_mutex_acquire(&mutex,TM_INFINITE);

	printf("task %p broadcast cond ret=%d\n",rt_task_self(),rt_cond_broadcast(&cond));
	rt_mutex_release(&mutex);
}


int main(int argc, char* argv[])
{
    printf("rt_mutex_create -> %d\n",rt_mutex_create(&mutex,"Mutex"));
    printf("rt_cond_create  -> %d\n",rt_cond_create(&cond,"Cond"));
    int prio = 0;
    printf("rt_task_spawn   -> %d\n",rt_task_spawn(&task1, "Wait", 0, prio, T_JOINABLE,&wait,NULL));
    printf("rt_task_spawn   -> %d\n",rt_task_spawn(&task2, "Broadcast", 0, prio, T_JOINABLE,&broadcast,NULL));
    printf("rt_task_join    -> %d\n",rt_task_join(&task1));
    printf("rt_task_join    -> %d\n",rt_task_join(&task2));
    return 0;
}
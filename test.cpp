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
#include <alchemy/task.h>
#include <alchemy/timer.h>
#elif CONFIG_XENO_VERSION_MAJOR == 2
#include <native/mutex.h>
#include <native/task.h>
#include <native/timer.h>
#endif

std::string to_string(int i)
{
    std::ostringstream os;
    os << i;
    return os.str();
}

static const int ntasks = 5;

RT_TASK demo_task[ntasks];

void demo(void *arg)

{

    RT_TASK *curtask;
    RT_TASK_INFO curtaskinfo;

    // hello world
    printf("Hello World!\n");

    // inquire current task
    curtask=rt_task_self();
    rt_task_inquire(curtask,&curtaskinfo);
    RT_MUTEX m;
    int r = rt_mutex_create(&m,0);
    printf("rt_mutex_create : %s\n",(r == 0 ? "OK" : "ERROR"));
    // print task name
    printf("Task name : %s \n", curtaskinfo.name);

    rt_task_sleep(rt_timer_ns2ticks(2E9));
}



int main(int argc, char* argv[])
{

#if CONFIG_XENO_VERSION_MAJOR == 3
    char *const* argvp = const_cast<char*const*>(argv);
    xenomai_init(&argc,&argvp);
#endif

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
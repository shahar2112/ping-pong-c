/**************************************/
/* author: Shahar maimon              */
/* date: 02.27.20                     */
/**************************************/

#include <stdio.h> /* fprintf */
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>   
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h> /* abort */

#define RED	printf("\033[1;31m")
#define BLUE printf("\033[1;36m")
#define MAGENTA	printf("\033[1;35m")
#define YELLOW printf("\033[1;33m")
#define GREEN printf("\033[1;32m")
#define DEFAULT	printf("\033[0m")

static void Pong1Handler(int signum);
static void Ping2Handler(int signum);

static void Exs1();
static void Exs2();
static void Exs3();

sig_atomic_t sigusr_flag_2 = 0;
sig_atomic_t sigusr_flag_1 = 0;

int main()
{
    
       Exs1();   
     /* Exs2(); */
    /* Exs3(); */
    
    return 0;
}


static void Exs1()
{
    pid_t child_pid = 0;
    struct sigaction sa_child;
    struct sigaction sa_parent;
    sa_child.sa_handler = &Pong1Handler;
    sa_parent.sa_handler = &Ping2Handler;
    sigemptyset(&sa_child.sa_mask);
    sigemptyset(&sa_parent.sa_mask);


    if (sigaction(SIGUSR1, &sa_child, NULL) == -1)
    {
        printf("Failed to set SIGUSR1 handler");
    }

    if (sigaction(SIGUSR2, &sa_parent, NULL) == -1)
    {
        printf("Failed to set SIGUSR2 handler");
    }

    child_pid = fork();

    if(child_pid < 0)
    {
        RED;
        printf("ERROR, no child created\n");
        exit(1);
        DEFAULT;
    }

    while(child_pid > 0)
    {
        if(sigusr_flag_2 == 1)
        {
            sigusr_flag_2 = 0;
            kill(child_pid, SIGUSR1);
        }
    }


    kill(getppid(), SIGUSR2);
    while(child_pid == 0)
    {
        if(sigusr_flag_1 == 1)
        {
            sigusr_flag_1 = 0;
            kill(getppid(), SIGUSR2);
        }
    }
} 





static void Exs2()
{
    pid_t child_pid = 0;
    char *args[] = {"./pong", NULL};
    struct sigaction sa_parent;

    sa_parent.sa_handler = &Ping2Handler;

    sigemptyset(&sa_parent.sa_mask);

    if(sigaction(SIGUSR2, &sa_parent, NULL) == -1)
    {
        printf("Failed to set SIGUSR2 handler");
    }

    child_pid = fork();

    if(child_pid < 0)
    {
        RED;
        printf("ERROR, no child created\n");
        DEFAULT;
        exit(1);
    }

    if(child_pid == 0)
    {
        printf("child pid is : %d\n", getpid());
        printf("parent pid is : %d\n", getppid());
        execvp(args[0], args);
        printf("if i am here execv failed\n!");
        abort();
    }

    while(child_pid > 0)
    {
        if(sigusr_flag_2 == 1)
        {
            sigusr_flag_2 = 0;
            kill(child_pid, SIGUSR1);
        }
    }
}



static void Exs3()
{
    int status = 0;
    int pong_pid = 0;
    pid_t child_pid = 0;
    struct sigaction sa_parent;

    sa_parent.sa_handler = &Ping2Handler;

    sigemptyset(&sa_parent.sa_mask);

    if(sigaction(SIGUSR2, &sa_parent, NULL) == -1)
    {
        printf("Failed to set SIGUSR2 handler");
    }

    printf("Process Ping PID is %d\n", getpid());

    printf("first insert Pong PID \n");
    scanf(" %d", &pong_pid);

    while(pong_pid != 0)
    {
        if(sigusr_flag_2 == 1)
        {
            sigusr_flag_2 = 0;
            kill(pong_pid, SIGUSR1);
        }
    }  
}


static void Pong1Handler(int signum)
{
    const char *msg = "\033[1;32m""PONG\n""\033[0m";
    sigusr_flag_1 = 1;
    write(STDOUT_FILENO, msg, strlen(msg));
    sleep(1);
}

static void Ping2Handler(int signum)
{
    const char *msg = "\033[1;33m""     PING\n""\033[0m";
    sigusr_flag_2 = 1;
    write(STDOUT_FILENO, msg, strlen(msg));
    sleep(1);
}

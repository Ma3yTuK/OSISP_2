#include <features.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


void handler(int signal)
{
    if (signal == SIGINT)
    {
        pid_t pid;
        if (pid = fork())
        {
            printf("\n%s %d\n", "New pid:", pid);
            exit(0);
        }
        
        struct sigaction sa;
        sa.sa_handler = handler;
        sa.sa_mask = set;

        sigaction(SIGINT, &sa, 0);
    }
}


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("%s", "Invalid number of arguments!");
    }

    sigset_t set;
    sigfillset(&set);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask = set;

    sigaction(SIGINT, &sa, 0);

    int counter = 1;

    while(1)
    {
        sleep(2);
        FILE* file = fopen(argv[1], "w");
        fprintf(file, "%d", counter++);
        fclose(file);
    }
}
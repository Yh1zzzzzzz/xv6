#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    int p1[2];
    int p2[2];
    int pid;
    pipe(p1);
    pipe(p2);
    pid = fork();
    char *p22 = "ping";
    char *p11 = "pong";

    /* code */
    if (pid == 0)
    {
        char buf1[512];

        close(p1[1]);
        close(p2[0]);
        //for (int i = 0; i < 100; i++)
        {   int pid1 = getpid();
            write(p2[1], p11, strlen(p11));
            read(p1[0], buf1, strlen(p22));
            printf("%d: received ping\n",pid1);
        }
        close(p1[0]);
        close(p2[1]);
    }
    else
    {
        char buf2[512];

        close(p1[0]);
        close(p2[0]);
        //for (int i = 0; i < 100; i++)
        {
            // p2[1] -> stdin
            int pid2 = getpid();
            write(p1[1], p22, strlen(p22));
            wait((int *)0);
            read(p2[0], buf2, strlen(p11));
            printf("%d: received pong\n",pid2);

            
        }
        close(p1[1]);
        close(p2[0]);
    }
    
    return 0;
}
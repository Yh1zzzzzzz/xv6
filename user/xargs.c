#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#define MAXARG 32 // max exec arguments
#define MAXARGLEN 64
int main(int argc, char *argv[])
{
    int number = argc - 1;
    char *xargv[MAXARG]; // 传递给xarg第一个user call的参；
    for (int i = 0; i < number; i++)
    {
        xargv[i] = argv[i + 1];
    }
    char buff[64];
    char *pointer = buff;
    int pid;
    pid = fork();
    if (pid == 0)
    {
        while (read(0, pointer, 1) > 0)
        {
            char temp = *pointer;
            if (temp == '\n')
            {
                xargv[number] = buff;
                number += 1;
                *(pointer++) = 0;
                pointer = buff;
            }
            pointer++;
        }
        exec(xargv[0], xargv);
        printf("xarg error\n");
        exit(1);
    }
    else
    {
        int wait_sign;
        wait(&wait_sign);
        if (wait_sign)
        {
            printf("son process error");
        }
        exit(0);
    }
}
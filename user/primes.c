#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define MAX_LEN 35
int isprime(int a)
{
    if(a == 1)
        return 0;
    if (a == 2)
    {
        return 1;
        /* code */
    }
    if (a % 2 ==0) {
        return 0;
    }
    
    for (int i = 3; i * i<= a; i += 2)
    {
        if(a % i ==0)
            return 0;
    }
    return 1;
}

int recursive(char *a){
    if(strlen(a) == 1 || a[1]==0){
        int temp = (int)a[0];
        printf("prime %d\n",temp);
        return 0;
    }



    int pipe1[2];
    if(pipe(pipe1) == -1){
        printf("pipe error");
    }

    int pid = fork();
    if (pid == 0) {
        char buffer[MAX_LEN] = {0};
        close(pipe1[1]);
        while(read(pipe1[0],buffer,MAX_LEN));
        close(pipe1[0]);

        char first = buffer[0];
        char next[MAX_LEN]={0};
        int counter = 0;
        printf("prime %d\n", (int)first);
        for(int i=1;i<MAX_LEN;i++){
            int P_first = (int)first;
            int Next = (int)buffer[i];
            if(Next % P_first !=0 && isprime((int)buffer[i])){
                next[counter] = buffer[i];
                counter += 1;
            }
        }
        recursive(next);
        return 0;
    }
    //下面是父的逻辑
    else {
        int wait_sign;
        close(pipe1[0]);
        for(int i = 0; i < strlen(a); i ++) {
            {
                write(pipe1[1], a, MAX_LEN);
            }
        }
        close(pipe1[1]);
        wait(&wait_sign);
        if (wait_sign != 0)
        {
            printf("son process error\n");
            exit(1);
        }
        return 0;
        
    }
}
int main()
{

    char t[MAX_LEN] = {0};
    for (int i = 0; i < MAX_LEN - 2; i++)
    {
        /* code */
        t[i] = (i + 2);
    }
    recursive(t);
    exit(0);
}
    // int pipe1[2];
    // int p = pipe(pipe1);
    // if(p == -1){
    //     printf("pipe error");
    // }
    // int pid = fork();


    // if (pid == 0) {
    //     char *buffer;
    //     close(pipe1[1]);
    //     while(read(pipe[0],buffer,1) > 0);
    //     close(pipe1[0]);

    //     char first = buffer[0];
    //     char *next;
    //     int counter = 0;
    //     printf("primes %c", first);
    //     for(int i=1;i<strlen(buffer);i++){
    //         int P_first = (int)first;
    //         int Next = (int)buffer[i];
    //         if(Next % P_first !=0){
    //             next[counter] = buffer[i]
    //             counter += 1;
    //         }
    //     }
    //     //这里call 递归 recursive(next);
    //     exit(1);
    // }
    // //下面是父的逻辑
    // else {
    //     int *wait_sign;
    //     close(pipe1[0]);
    //     for(int i =1; i <= MAX_LEN; i ++) {
    //         if(isprime(i)) {
    //             char temp = (char)i;
    //             write(pipe1[0], &temp, sizeof(char));
    //         }
    //     }
    //     close(pipe1[1]);
    //     wait(wait_sign);
    //     if (*wait_sign != 0)
    //     {
    //         printf("son process error\n");
    //         exit(1);
    //     }
    //     exit(0);
        
    // }


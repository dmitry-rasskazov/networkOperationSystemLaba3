#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int cargs, char ** vargs)
{
    int firstChildPID, secondChildPID;

    if ((firstChildPID = fork()) == -1 ) {
        perror ("Can't fork first process"); exit(1);
    } else if(firstChildPID == 0 ) {
        /* child */
        int pid = getpid();
        printf ("First child:PID=%d, PPID=%d\n", pid ,getppid());

        sleep(15);
        exit(0);
    }

    if ((secondChildPID = fork()) == -1 ) {
        perror ("Can't fork second process"); exit(1);
    } else if(secondChildPID == 0 ) {
        /* child */
        int pid = getpid();
        printf ("Second child:PID=%d, PPID=%d\n", pid ,getppid());

        sleep(10);
        exit(0);
    }

    int pid = getpid();
    printf ("Parent:PID=%d, PPID=%d\n", pid ,getppid());
    pause();

    return 0;
}

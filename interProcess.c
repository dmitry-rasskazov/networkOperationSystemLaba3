#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 1259

void closePipeDescriptors(int *);
int openFile(char *, int);
void printErrno();

int cigintDetected = 0;
static void sigintHandler(int sig)
{
    printf ("\nSIGINT detected! Value: %d\n", sig);
    cigintDetected = 1;
}

int main(int cargs, char ** vargs)
{
    signal(SIGINT, sigintHandler);

    int firstChildPID, secondChildPID;
    FILE *file;
    unsigned long n;
    int pipeDescriptors[2], err, fileDescriptor;
    char buff[BUFFER_SIZE];

    if ((err = pipe(pipeDescriptors) < 0)) {
        perror ("Error creating pipe");
        exit(1);
    }

    if ((firstChildPID = fork()) == -1 ) {
        perror ("Can't fork first process"); exit(1);
    } else if(firstChildPID == 0 ) {
        /* child */
        int pid = getpid();
        printf ("First child:PID=%d, PPID=%d\n", pid ,getppid());

        file = fopen("CMakeLists.txt", "r");

        while((n = fread(buff, 1, BUFFER_SIZE, file)) > 0) {
            write(pipeDescriptors[1], buff, n);
        }

        closePipeDescriptors(pipeDescriptors);
        fclose(file);

        printf ("First child close!\n");

        exit(0);
    }

    if ((secondChildPID = fork()) == -1 ) {
        perror ("Can't fork second process"); exit(1);
    } else if(secondChildPID == 0 ) {
        /* child */
        int pid = getpid();
        printf ("Second child:PID=%d, PPID=%d\n", pid ,getppid());

        fileDescriptor = dup(pipeDescriptors[0]);
        closePipeDescriptors(pipeDescriptors);

        printf("Second child output: \n");;
        while((n = read(fileDescriptor, buff, BUFFER_SIZE)) > 0) {
            printf("%s\n", buff);
        };

        close(fileDescriptor);
        printf ("Second child close!\n");

        exit(0);
    }

    closePipeDescriptors(pipeDescriptors);

    int pid = getpid();
    printf ("Parent:PID=%d, PPID=%d\n", pid ,getppid());
    while (!cigintDetected) {
        sleep(1);
    }

    printf ("Parent: Close!\n");

    return 0;
}

int openFile(char * fileName, int flags) {
    int fileDescriptor = open(fileName, flags);
    if(fileDescriptor == -1) {
        printErrno();
        exit(1);
    }

    return fileDescriptor;
}

void closePipeDescriptors(int * pipeDescriptors)
{
    close(pipeDescriptors[0]);
    close(pipeDescriptors[1]);
}

void printErrno()
{
    printf("errno_№: %d, error_message: %s\n", errno, strerror(errno));
}

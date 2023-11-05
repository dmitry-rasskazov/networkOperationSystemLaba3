#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFF_MAX 100U

int createFile(char *, int);
int openFile(char *, int);
void closeFile(int);
void printErrno();
void fullCopy(int, int);
void chmodFile(int, int);
void removeFile(const char *);

int main(int cargs, char ** vargs) {
    int fdNew, fdOld;

    char oldFileName[100], newFileName[100], dialogAck;

    printf("Введите имя файла назначения: ");
    scanf("%s", newFileName);

    fdNew = createFile(newFileName, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH);

    printf("File descriptor TARGET: %d\n", fdNew);

    printf("Введите имя файла источника: ");
    scanf("%s", oldFileName);

    fdOld = openFile(oldFileName, O_RDONLY);

    printf("File descriptor SOURCE: %d\n", fdOld);

    fullCopy(fdOld, fdNew);

    printf("Отменить право владельца на выполнение? (y/n): ");
    scanf("%s", &dialogAck);
    if(dialogAck == 'y') {
        chmodFile(fdNew, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        printf("Права на выполнения изменены\n");
    }

    printf("Удалить созданный файл? (y/n): ");
    scanf("%s", &dialogAck);
    if(dialogAck == 'y') {
        removeFile(newFileName);
        printf("Файл удалён\n");
    }

    closeFile(fdNew);
    closeFile(fdOld);
    return 0;
}

int createFile(char * fileName, int mode)
{
    int fileDescriptor = creat(fileName, mode);
    if(fileDescriptor == -1) {
        printErrno();
        exit(1);
    }

    return fileDescriptor;
}

int openFile(char * fileName, int flags) {
    int fileDescriptor = open(fileName, flags);
    if(fileDescriptor == -1) {
        printErrno();
        exit(1);
    }

    return fileDescriptor;
}

void closeFile(int fileDescriptor)
{
    if(-1 == close(fileDescriptor)) {
        printErrno();
        exit(0);
    }
}

void chmodFile(int fd, int mode)
{
    if(-1 == fchmod(fd, mode)) {
        printErrno();
    }
}

void removeFile(const char * fileName)
{
    if(unlink(fileName) == -1) {
        printErrno();
    }
}

void printErrno()
{
    printf("errno_№: %d, error_message: %s\n", errno, strerror(errno));
}

void fullCopy(int fdSource, int fdTarget)
{
    char buff[BUFF_MAX];
    int readCount, writenCount;

    while((readCount = read(fdSource, buff, BUFF_MAX)) > 0) {
        if ((writenCount = write(fdTarget,buff,readCount)) < 0) {
            printErrno();
            return;
        }
    }
}
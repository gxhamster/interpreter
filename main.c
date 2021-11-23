#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef int unsigned int 

int token;
char *src;
int poolsize;


void next() 
{
    token = *src++;
    return;
}

void program()
{
    next();
    while (token > 0) {
        printf("token: %c\n", token);
        next();
    }
    return;
}

int main(int argc, char **argv)
{
    int fd;
    int i;
    poolsize = 8 * 1000;
    char *name = "main.c";

    if ((fd = open(name, 0)) < 0) {
        fprintf(stderr, "ERROR: could not open file\n");
        return -1;
    }
    if ((src = (char *)malloc(poolsize)) == 0) {
        fprintf(stderr, "ERROR: could not allocate memory\n");
        return -1;
    }

    if ((i = read(fd, src, poolsize-1)) <= 0) {
        fprintf(stderr, "ERROR: could not read()\n");
        return -1;
    }
    
    program();
    return 0;
}



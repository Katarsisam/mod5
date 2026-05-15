// String reversal done by device driver

#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h> // for memset and strlen

#define BUF_LEN 80 

int main(int argc ,char *argv[])
{
    assert(argc > 1);
    char buff[BUF_LEN] ;
    char i = 0;
    memset(buff, 0, BUF_LEN);
    

    int fp = open("/dev/chardev", O_RDWR);

    write(fp, argv[1], strlen(argv[1]));

    while(read(fp, &buff[i++], 1));

    printf("chardev: %s\n", buff);
}
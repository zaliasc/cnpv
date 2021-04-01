#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    FILE *fp1,*fp2;
    fp1=fopen("README.en.md","rw");
    if(fp1==NULL)
    {
        printf("fp1 open failed!\n");
    }
    else
    {
        printf("fp1 open success!\n");
    }
    fp2=fopen("filename.txt","r");
    if(fp2==NULL)
    {
        printf("fp2 open failed!\n");
    }
    else
    {
        printf("fp2 open success!\n");
    }
    return 0;
}
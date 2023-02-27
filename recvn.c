#include "process_pool.h"
int recvn(int sfd,void*pstart,int len){//循环接收
    int total=0,ret;
    char *p=(char*)pstart;
    while(total<len){
        ret=recv(sfd,p+total,len-total,0);
        if(ret==0)
        {
            printf("\n");
            return -1;
        }
        total+=ret;
    }
    return 0;
} 




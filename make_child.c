#include "process_pool.h"
int make_child(process_data_t *p,int process_num){
    int i;
    pid_t pid;//子进程pid
    int ret;
    int fds[2];
    for(i=0;i<process_num;i++){
        ret=socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
        ERROR_CHECK(ret,-1,"socketpair");
        pid=fork();
        if(pid==0){
            close(fds[0]);//子进程读管道
            child_handle(fds[1]);//子进程处理任务,获得从父进程传来的文件描述符
        }   
        else{
            close(fds[1]);
            p[i].pid=pid;
            p[i].fd=fds[0];
            p[i].busy_flag=0;
        }   
    }   
    return 0;
}


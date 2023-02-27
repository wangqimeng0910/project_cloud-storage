#include "process_pool.h"
int main(int argc,char*argv[])
{
    if(argc!=4){
        printf("args error,please use ./process_pool_server  ip port process_num\n");
        return -1;
    }
    int process_num=atoi(argv[3]);
    process_data_t *pdata=(process_data_t *)calloc(process_num,sizeof(process_data_t ));//申请结构体指针数组
    make_child(pdata,process_num);//初始化子进程
    int sfd;
    tcp_init(argv[1],argv[2],&sfd);
    int epfd;
    epfd=epoll_create(1);
    struct epoll_event evs[process_num+1];
    epoll_add(epfd,sfd);//监听是否有客户端连接
    int i;
    for(i=0;i<process_num;++i){
        epoll_add(epfd,pdata[i].fd);//监听每个来自管道的信息
    }
    int new_fd,j;
    int epoll_ready_num=0;
    char finish_flag=0;
    while(1){
        epoll_ready_num=epoll_wait(epfd,evs,process_num+1,-1);//等待连接
        for(i=0;i<epoll_ready_num;++i){//看是否有消息来临
            if(evs[i].data.fd==sfd){//如果是客户端连接
                new_fd=accept(sfd,NULL,NULL);//获得new_fd
               // printf("get new_fd\n");
                for(j=0;j<process_num;j++){
                    if(pdata[j].busy_flag==0){
                        send_fd(pdata[j].fd,new_fd);
                        pdata[j].busy_flag=1;
                        printf("send task%d to %d,set busy\n",new_fd,pdata[j].pid);
                        printf("finish_flag=%d\n",finish_flag);
                        break;
                    }
                }
                close(new_fd);//结束连接
            }
        }
    }
    return 0;
}


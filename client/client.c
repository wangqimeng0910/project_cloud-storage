#include "process_pool.h"
int recvn(int sfd,void*pstart,int len){
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

int main(int argc,char *argv[])
{
    ARGS_CHECK(argc,3);//初始化一个socket描述符，用于tcp通信
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sfd,-1,"socket");
    struct sockaddr_in ser_addr;
    bzero(&ser_addr,sizeof(ser_addr));
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_port=htons(atoi(argv[2]));//端口转为网络字节序
    ser_addr.sin_addr.s_addr=inet_addr(argv[1]);
    int ret=connect(sfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    ERROR_CHECK(ret,-1,"connect");
    char cmd[128];
    int send_len=0;
    int data_len=0;
    while(bzero(cmd,128),(ret=read(0,cmd,128))>0){
        trim_space(cmd);
        send_len =strlen(cmd);
        send(sfd,&send_len,sizeof(int),0);//把命令的长度发过去
        if(strncmp("cd",cmd,2)==0){
            send(sfd,cmd,send_len,0);//把命令发过去
            printf("send\n");
            char buf[1024]={0};
            bzero(buf,sizeof(buf));
            bzero(&data_len,sizeof(data_len));
            recvn(sfd,&data_len,4);//接收服务器发来的数据长度
            recvn(sfd,buf,data_len);//接收服务器发给来的cd的数据
            puts(buf);
        }
        else if (strncmp("ls",cmd,2)==0){
            send(sfd,cmd,send_len,0);//把命令发过去
            char buf[1024]={0};
            while(1){
                bzero(buf,sizeof(buf));
                bzero(&data_len,sizeof(data_len));
                recvn(sfd,&data_len,4);//接收服务器发来的数据长度
                if(data_len==0){
                    break;
                }
                recvn(sfd,buf,data_len);//接收服务器发来的数据内容
                puts(buf);
            }
            printf("recv over\n");
        }
        else if(strncmp("puts", cmd, 4) == 0){
            send(sfd,cmd,send_len,0);//把命令发过去
            char buf[1024]={0};
            bzero(buf,sizeof(buf));
            bzero(&data_len,sizeof(data_len));
            tran_file(sfd);
            int ret=recvn(sfd,&data_len,4);//接收服务器发来的数据长度
            ERROR_CHECK(ret,-1,"recvn");
            ret=recvn(sfd,buf,data_len);//接收数据内容
            ERROR_CHECK(ret,-1,"recvn");
            puts(buf);
        }
        else if(strncmp("gets", cmd, 4)==0){
            send(sfd,cmd,send_len,0);//把命令发过去
            char buf[1024]={0};
            bzero(buf,sizeof(buf));
            bzero(&data_len,sizeof(data_len));
            ret =recv(sfd,&data_len,4,0);//接收数据的大小
            if(ret==-1){return -1;}
            ret=recv(sfd,buf,data_len,0);//接收文件名
            if(ret==-1){return -1;}
            int fd=open(buf,O_RDWR|O_CREAT,0666);
            ERROR_CHECK(fd,-1,"open");//接收文件内容
            off_t filesize=0,download_size=0;
            recvn(sfd,&data_len,4);//接收文件长度的长度
            ERROR_CHECK(ret,-1,"recvn");
            recvn(sfd,&filesize,data_len);//接收文件大小
            ERROR_CHECK(ret,-1,"recvn");
            printf("filesize=%ld\n",filesize);
            int fds[2];
            pipe(fds);
            while(download_size<filesize){//接收文件a
                ret=splice(sfd,NULL,fds[1],NULL,4096,SPLICE_F_MORE|SPLICE_F_MOVE);
                ERROR_CHECK(ret,-1,"splice");
                download_size+=ret;
                ret=splice(fds[0],NULL,fd,NULL,ret,SPLICE_F_MORE|SPLICE_F_MOVE);
                ERROR_CHECK(ret,-1,"splice");
            }
            printf("gets success!\n");
        }
        else if(strncmp("remove", cmd, 6) == 0){
            send(sfd,cmd,send_len,0);
        }
        else if(strncmp("pwd", cmd, 3) == 0){
            send(sfd,cmd,send_len,0);//把命令发到服务器
            char buf[1024]={0};
            bzero(buf,sizeof(buf));
            bzero(&data_len,sizeof(data_len));
            recvn(sfd,&data_len,4);
            recvn(sfd,buf,data_len);
            puts(buf);
        }
    }
}



#include "process_pool.h"
int do_cd(int new_fd,char*cmd){
    char dir[128]="";
    char buf[128]={0};
    strcpy(buf,cmd);
    sscanf(buf+3,"%s",dir);
    chdir(dir);
    getcwd(dir,sizeof(dir));
    int len=strlen(dir);
    int ret=send(new_fd,&len,sizeof(int),0);//发送数据的大小
    ERROR_CHECK(ret,-1,"send");
    ret=send(new_fd,dir,len,0);//发送数据内容
    ERROR_CHECK(ret,-1,"send");
    return 0;
}
int do_ls(int new_fd){
    DIR *dir=opendir("./");
    if(dir==NULL){
        perror("opendir");
        return -1;
    }
    else{
        struct dirent *pdirent;
        int len;
        char buf[1024];
        bzero(buf,sizeof(buf));
          //  struct stat statbuf;
        while((pdirent=readdir(dir))){
           // bzero(&statbuf,sizeof(stat));
           // stat(pdirent->d_name,&statbuf);
            sprintf(buf,"type:%d name:%s",pdirent->d_type,pdirent->d_name);
            len=strlen(buf);
            int ret=send(new_fd,&len,sizeof(int),0);//发送数据大小
            ERROR_CHECK(ret,-1,"send");
            ret=send(new_fd,buf,len,0);//发送数据内容
            ERROR_CHECK(ret,-1,"send");
        }
        closedir(dir);
        printf("over\n");
        int zero=0;
        send(new_fd,&zero,4,0);
    }
    return 0;
}
int do_puts(int new_fd){
    int data_len,ret;
    char buf[1024]={0};
    bzero(&data_len,sizeof(data_len));
    bzero(buf,sizeof(buf));
    ret =recv(new_fd,&data_len,4,0);//接收数据的大小
    if(ret==-1){return -1;}
    ret=recv(new_fd,buf,data_len,0);//接收文件名
    if(ret==-1){return -1;}
    int fd=open(buf,O_RDWR|O_CREAT,0666);
    ERROR_CHECK(fd,-1,"open");//接收文件内容
    off_t filesize=0,download_size=0;
    recvn(new_fd,&data_len,4);//接收文件长度的长度
    ERROR_CHECK(ret,-1,"recvn");
    recvn(new_fd,&filesize,data_len);//接收文件大小
    ERROR_CHECK(ret,-1,"recvn");
    printf("filesize=%ld\n",filesize);
    int fds[2];
    pipe(fds);
    while(download_size<filesize){//接收文件a
        ret=splice(new_fd,NULL,fds[1],NULL,4096,SPLICE_F_MORE|SPLICE_F_MOVE);
        ERROR_CHECK(ret,-1,"splice");
        download_size+=ret;
        ret=splice(fds[0],NULL,fd,NULL,ret,SPLICE_F_MORE|SPLICE_F_MOVE);
        ERROR_CHECK(ret,-1,"splice");
    }
    bzero(&data_len,sizeof(data_len));
    bzero(buf,sizeof(buf));
    sprintf(buf,"puts success!\n");
    int len=strlen(buf);
    ret=send(new_fd,&len,sizeof(int),0);
    ERROR_CHECK(ret,-1,"send");
    ret=send(new_fd,buf,len,0);
    ERROR_CHECK(ret,-1,"send");
    return 0;
}
int do_gets(int new_fd){
    tran_file(new_fd);
    return 0;
}
int do_remove(int new_fd,char *cmd){
  // char cmd[128]={0};
  // recv(new_fd,cmd,128,0);
   char * p=cmd+7;
   int ret=unlink(p);
   ERROR_CHECK(ret,-1,"unlink");
   return 0;
}
int do_pwd(int new_fd){
    char buf[1024];
    bzero(buf,sizeof(buf));
    getcwd(buf,1024);
    int len=strlen(buf);
    int ret=send(new_fd,&len,sizeof(int),0);
    ERROR_CHECK(ret,-1,"send");
    ret=send(new_fd,buf,len,0);
    ERROR_CHECK(ret,-1,"send");
    return 0;
}
void child_handle(int pipefd){
     int new_fd;
     char finish_flag=1;//表示是否完成；
     train_t t;
     int ret;
     int cmd_len=0;
     char cmd[128]={0};
     while(1){
         recv_fd(pipefd,&new_fd);//接收父进程发送的new_fd
         printf("recv task  new_fd=%d,waiting for your order!\n",new_fd);
         while(1){
             bzero(&t,sizeof(train_t));
             bzero(cmd,sizeof(cmd));
             bzero(&cmd_len,sizeof(cmd_len));
             ret=recv(new_fd,&cmd_len,sizeof(int),0);//接收命令的大小
             if(ret==0){
                 printf("client exit !\n");
                 close(new_fd);
                 exit(1);
             }
             recv(new_fd,cmd,cmd_len,0);//接收命令
             if(strncmp("cd",cmd,2)==0){
                 do_cd(new_fd,cmd);
             }
             else if (strncmp("ls",cmd,2)==0){
                 do_ls(new_fd);
             }
             else if(strncmp("puts",cmd,4)== 0){
                 do_puts(new_fd);
             }
             else if(strncmp("gets",cmd,4)== 0){
                 do_gets(new_fd);
             }
             else if( strncmp("remove",cmd,6)== 0 ){
                 do_remove(new_fd,cmd);
             }
             else if(strncmp("pwd",cmd, 3)==0){
                 do_pwd(new_fd);
             }
         }
     }
}



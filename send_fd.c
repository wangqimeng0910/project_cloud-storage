#include "process_pool.h"
int send_fd(int pipefd,int fd){//接收文件描述符
      struct msghdr msg;
      bzero(&msg,sizeof(msg));
      struct iovec iov[1];//给iov初始化
      char buf[10]="hello";
      iov[0].iov_base=buf;
      iov[0].iov_len=5;
      msg.msg_iov=iov;
      msg.msg_iovlen=1;
      struct cmsghdr *cmsg;
      int cmsg_len=CMSG_LEN(sizeof(int));
      cmsg=(struct cmsghdr*)calloc(1,cmsg_len);
      cmsg->cmsg_level=SOL_SOCKET;
      cmsg->cmsg_type=SCM_RIGHTS;
      cmsg->cmsg_len=cmsg_len; 
      fd=*(int*)CMSG_DATA(cmsg)=fd;
      msg.msg_control=cmsg;
      msg.msg_controllen=cmsg_len;
      int ret;
      ret=sendmsg(pipefd,&msg,0);
      ERROR_CHECK(ret,-1,"sendmsg");
      return 0;
  }


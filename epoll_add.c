#include "process_pool.h"
int epoll_add(int epfd,int fd){//添加监听
      struct epoll_event event;
      event.events =EPOLLIN;
      event.data.fd=fd;
      int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
      ERROR_CHECK(ret,-1,"epoll_ctl");
      return 0;
  }




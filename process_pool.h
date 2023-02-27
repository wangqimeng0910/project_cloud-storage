#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <syslog.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <strings.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <fcntl.h>
typedef struct {
    pid_t pid;//子进程的pid
    int fd;//子进程管道对端，全双工管道
    short busy_flag;//检查子进程是否忙碌，0表示不忙碌，1表示忙碌
}process_data_t;
typedef struct{
    int data_len;//控制数据，火车头，记录火车装载内容长度
    char buf[1024];//火车车厢
}train_t;
#define ARGS_CHECK(argc,num){if(argc !=num){fprintf(stderr,"Argc Error!\n");return -1;}}
#define ERROR_CHECK(ret,num,msg){if(ret == num){perror(msg);return -1;}}
#define THREAD_ERROR_CHECK(ret,funcname){if(ret!=0){printf("%s failed %s\n",funcname,strerror(ret));return -1 ;}}
#define FILENAME "file1"
#define BUF_SIZE 8*1024
int recv_fd(int pipefd,int *fd);
void child_handle(int pipefd);
int make_child(process_data_t *p,int process_num);
int tran_file(int new_fd);
int tcp_init(char* ip,char * port,int *psfd);
int epoll_add(int epfd,int fd);
int send_fd(int pipefd,int fd);
void trim_space(char* src);
int recvn(int sfd,void*pstart,int len);

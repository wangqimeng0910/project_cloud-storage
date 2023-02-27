#include "process_pool.h"

int tran_file(int new_fd){                                                        
    train_t t;//先发文件名
    t.data_len=strlen(FILENAME);
    strcpy(t.buf,FILENAME);
    int ret=send(new_fd,&t,4+t.data_len,MSG_NOSIGNAL);
    ERROR_CHECK(ret,-1,"send");
    int fd=open(FILENAME,O_RDONLY);
    ERROR_CHECK(fd,-1,"open");
    struct stat buf;
    ret=fstat(fd,&buf);
    ERROR_CHECK(ret,-1,"fstat");
    t.data_len=sizeof(buf.st_size);
    memcpy(t.buf,&buf.st_size,t.data_len);
    ret=send(new_fd,&t,4+t.data_len,MSG_NOSIGNAL);
    ERROR_CHECK(ret,-1,"send");
    ret=sendfile(new_fd,fd,NULL,buf.st_size);
    ERROR_CHECK(ret,-1,"sendfile");
    return 0;
}

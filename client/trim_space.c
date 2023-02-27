#include "process_pool.h"
void trim_space(char * src){
    unsigned long index;
    int cur;
    for(cur =-1,index=0;index<strlen(src);index ++){
        if(src[index]!=' '&& src[index]!='\n'){
           src[++cur]=src[index];
        }
        else{
            if(cur !=-1 &&src[cur]!=' '&&src[cur]!='\n')
            {
                src[++cur]=' ';
            }
        }
    }
    for(;cur>=0;cur --)
    {
        if(src[cur]!=' ')
        {
            break;
        }
    }
    src[++cur]='\0';
}



#include<stdio.h>
#include"kvstore.h"
#include<string.h>
#include<stdlib.h>
extern kvs_rbtree_t* rbtrees[100];//暂定100个用户
extern kvs_rbtree_t* hash[100000];
extern mempool_t* memp[100000];
extern mempool_t global_mempool;
extern kvs_array_t global_array;
#define ARRAY 0
#define RBTREE 1
void* kvs_malloc(size_t size,int fd){
    return mempool_alloc(memp[fd],size);
    //return malloc(size);
}
void kvs_free(void* ptr,int fd){
    mempool_free(memp[fd],ptr);
    //free(ptr);
}
int kvs_split_token(char* msg,char* tokens[]){
    if(msg==NULL||tokens==NULL){
        printf("kvs_split_token error\n");
        return -1;
    }
    int ind=0;
    char* token=strtok(msg," ");
    while(token!=NULL){
        tokens[ind++]=token;
        token=strtok(NULL," ");
    }
    return ind;

}
int kvs_filter_protocol(char* tokens[],int count,char* response,int fd){
    if(tokens==NULL||count<=0||response==NULL){
        printf("kvs_filter_protocol\n");
        return -1;
    }
    int cmd=KVS_CMD_START;
    for(cmd=KVS_CMD_START;cmd<=KVS_CMD_COUNT;cmd++){
        if(strcmp(tokens[0],command[cmd])==0){
            break;
        }
    }
    int ret=0;
    char* get_ret={0};
    /*
     * @ret
        -1:failed
        0:success or exist
        >0:not exist
     */
    /*
    *@get_ret
    NULL:not exist
    not NULL:exist
    */
    switch (cmd)
    {
    //arr
    /*
    case KVS_CMD_SET:
        ret=kvs_array_set(&global_array,tokens[1],tokens[2]);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"ok\r\n");
        }else{
            sprintf(response,"exist\r\n");
        }
        break;
    case KVS_CMD_GET:
        get_ret=kvs_array_get(&global_array,tokens[1]);
        if(get_ret==NULL){
            sprintf(response,"not exist\r\n");
        }
        else{
            sprintf(response,"%s\r\n",get_ret);
        }
        break;
    case KVS_CMD_DEL:
        ret=kvs_array_del(&global_array,tokens[1]);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"ok\r\n");
        }
        else{
            sprintf(response,"not exist\r\n");
        }
        break;
    case KVS_CMD_MOD:
        ret=kvs_array_mod(&global_array,tokens[1],tokens[2]);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"ok\r\n");
        }
        else{
            sprintf(response,"not exist\r\n");
        }
        break;
    case KVS_CMD_EXIST:
        ret=kvs_array_exist(&global_array,tokens[1]);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"exist\r\n");
        }
        else{
            sprintf(response,"not exist\r\n");
        }
        break;
    */
    //rbtree
    #if RBTREE
    case KVS_CMD_RSET:
        //ret=kvs_array_set(&global_array,tokens[1],tokens[2]);
        ret=kvs_rbtree_set(hash[fd],tokens[1],tokens[2],fd);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"ok\r\n");
        }else{
            sprintf(response,"exist\r\n");
        }
        break;
    case KVS_CMD_RGET:
        //get_ret=kvs_array_get(&global_array,tokens[1]);
        get_ret=kvs_rbtree_get(hash[fd],tokens[1],fd);
        if(get_ret==NULL){
            
            sprintf(response,"not exist\r\n");
        }
        else{
            
            sprintf(response,"%s\r\n",get_ret);
        }
        break;
    case KVS_CMD_RDEL:
        //ret=kvs_array_del(&global_array,tokens[1]);
        ret=kvs_rbtree_del(hash[fd],tokens[1],fd);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"ok\r\n");
        }
        else{
            sprintf(response,"not exist\r\n");
        }
        break;
    case KVS_CMD_RMOD:
        //ret=kvs_array_mod(&global_array,tokens[1],tokens[2]);
        ret=kvs_rbtree_mod(hash[fd],tokens[1],tokens[2],fd);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"ok\r\n");
        }
        else{
            sprintf(response,"not exist\r\n");
        }
        break;
    case KVS_CMD_REXIST:
        //ret=kvs_array_exist(&global_array,tokens[1]);
        ret=kvs_rbtree_exist(hash[fd],tokens[1],fd);
        if(ret==-1){
            sprintf(response,"failed\r\n");
        }
        else if(ret==0){
            sprintf(response,"exist\r\n");
        }
        else{
            sprintf(response,"not exist\r\n");
        }
        break;
    #endif
    
    }
    
}
int kvs_protocol(char* msg,int length,char* response,int fd){
    if(msg==NULL||length<0||response==NULL){
        printf("kvs_protocol\n");
        return -1;
    }
    //printf("msg:%s\n",msg);
    //memcpy(response,msg,strlen(msg));
    char* tokens[KVS_MAX_TOKENS]={0};
    int count=kvs_split_token(msg,tokens);
    if(count==-1){
        printf("kvs_protocol:count error\n");
        return -1;
    }
    return kvs_filter_protocol(tokens,count,response,fd);
}
int kvs_init_kvsengine(kvs_rbtree_t* tree,int fd){
    #if ARRAY
    memset(&global_array,0,sizeof(kvs_array_t));
    kvs_array_create(&global_array);
    #endif

    #if RBTREE
    memset(tree,0,sizeof(kvs_rbtree_t));
    kvs_rbtree_create(tree,fd);
    #endif
    return 0;
}
void kvs_dest_kvsengine(kvs_rbtree_t* tree,int fd){
    #if ARRAY
    kvs_array_destory(&global_array);
    #endif

    #if RBTREE
    kvs_rbtree_destory(tree,fd);
    #endif
}
int main(int argc,char* argv[]){
    //mempool_create(&global_mempool,sizeof(rbtree_node));
    //kvs_init_kvsengine();
    
    reactor_start(kvs_protocol);
    
    //kvs_dest_kvsengine();
    //mempool_destory(&global_mempool);
    return 0;
}
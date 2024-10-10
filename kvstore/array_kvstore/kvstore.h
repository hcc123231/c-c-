#ifndef __KVSTORE_H__
#define __KVSTORE_H__
#include<stddef.h>
#define KVS_MAX_TOKENS 128

typedef int (*msg_handler)(char* msg,int length,char* response);
int reactor_start(msg_handler handler);
static char* command[]={
    "SET","GET","DEL","MOD","EXIST"
};
enum{
    KVS_CMD_START=0,
    KVS_CMD_SET=KVS_CMD_START,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_EXIST,
    
};
typedef struct kvs_array_item_s{
    char* key;
    char* value;
}kvs_array_item_t;
#define KVS_ARRAY_SIZE 1024
typedef struct kvs_array_s{
    kvs_array_item_t* table;
    int idx;
    int total;
}kvs_array_t;
void* kvs_malloc(size_t size);
void kvs_free(void* ptr);
int kvs_array_create(kvs_array_t* inst);
void kvs_array_destory(kvs_array_t* inst);
char* kvs_array_get(kvs_array_t* inst,char* key);
int kvs_array_set(kvs_array_t* inst,char* key,char* value);
int kvs_array_del(kvs_array_t* inst,char* key);
int kvs_array_mod(kvs_array_t* inst,char* key,char* value);
int kvs_array_exist(kvs_array_t* inst,char* key);

#endif
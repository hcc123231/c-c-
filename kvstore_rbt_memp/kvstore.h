#ifndef __KVSTORE_H__
#define __KVSTORE_H__
#include<stddef.h>
#define KVS_MAX_TOKENS 128
#define ARRAY 0
#define RBTREE 1
typedef int (*msg_handler)(char* msg,int length,char* response);
int reactor_start(msg_handler handler);
static char* command[]={
    "SET","GET","DEL","MOD","EXIST",
    "RSET","RGET","RDEL","RMOD","REXIST"
};
enum{
    KVS_CMD_START=0,
    //arr
    KVS_CMD_SET=KVS_CMD_START,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_EXIST,
    //rbtree
    KVS_CMD_RSET,
    KVS_CMD_RGET,
    KVS_CMD_RDEL,
    KVS_CMD_RMOD,
    KVS_CMD_REXIST,

    KVS_CMD_COUNT
    
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
#define MEM_PAGE_SIZE 4800000
typedef struct mempool_s{
    int blockSize;
    int unusedCount;
    char* unusedPtr;
    char* mem;
}mempool_t;
int mempool_create(mempool_t* mempool,int block_sz);
void mempool_destory(mempool_t* mempool);
char* mempool_alloc(mempool_t* mempool,size_t size);
void mempool_free(mempool_t* mempool,void* ptr);

void* kvs_malloc(size_t size);
void kvs_free(void* ptr);
#if ARRAY
int kvs_array_create(kvs_array_t* inst);
void kvs_array_destory(kvs_array_t* inst);
char* kvs_array_get(kvs_array_t* inst,char* key);
int kvs_array_set(kvs_array_t* inst,char* key,char* value);
int kvs_array_del(kvs_array_t* inst,char* key);
int kvs_array_mod(kvs_array_t* inst,char* key,char* value);
int kvs_array_exist(kvs_array_t* inst,char* key);
#endif

#if RBTREE
    typedef char* KEY_TYPE;

    typedef struct _rbtree_node {
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;
	KEY_TYPE key;
	char *value;
    } rbtree_node;
    
    typedef struct _rbtree {
	rbtree_node *root;
	rbtree_node *nil;
    } rbtree;
typedef struct _rbtree kvs_rbtree_t;
extern kvs_rbtree_t global_rbtree;
void kvs_rbtree_create(kvs_rbtree_t* inst);
void kvs_rbtree_destory(kvs_rbtree_t* inst);
char* kvs_rbtree_get(kvs_rbtree_t* inst,char* key);
int kvs_rbtree_set(kvs_rbtree_t* inst,char* key,char* value);
int kvs_rbtree_del(kvs_rbtree_t* inst,char* key);
int kvs_rbtree_mod(kvs_rbtree_t* inst,char* key,char* value);
int kvs_rbtree_exist(kvs_rbtree_t* inst,char* key);
#endif

#endif
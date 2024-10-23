#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"kvstore.h"
mempool_t global_mempool;
int mempool_create(mempool_t* mempool,int block_sz){
    if(mempool==NULL)return -1;
    memset(mempool,0,sizeof(mempool_t));
    mempool->blockSize=block_sz;
    mempool->unusedCount=MEM_PAGE_SIZE/block_sz;
    mempool->mem=(char*)malloc(MEM_PAGE_SIZE);
    mempool->unusedPtr=mempool->mem;
    if(!mempool->mem)return -1;
    memset(mempool->mem,0,MEM_PAGE_SIZE);

    int i=0;
    char* ptr=mempool->mem;
    for(i=0;i<mempool->unusedCount-1;i++){
        *(char**)ptr=ptr+block_sz;
        ptr=ptr+block_sz;
    }
    *(char**)ptr=NULL;
    return 0;
}
void mempool_destory(mempool_t* mempool){
    if(mempool==NULL)return;
    free(mempool->mem);
    mempool->mem=NULL;
    free(mempool->unusedPtr);
    mempool->unusedPtr=NULL;
    free(mempool);
    mempool=NULL;
    return;
}
char* mempool_alloc(mempool_t* mempool,size_t size){
    if(mempool==NULL||size<=0||size>mempool->blockSize||mempool->unusedCount<=0)return NULL;
    char* ptr=mempool->unusedPtr;
    
    mempool->unusedPtr=*(char**)ptr;
    
    mempool->unusedCount--;
    return ptr;
}
void mempool_free(mempool_t* mempool,void* ptr){
    *(char**)ptr=mempool->unusedPtr;
    mempool->unusedPtr=ptr;
    mempool->unusedCount++;
    return;
}



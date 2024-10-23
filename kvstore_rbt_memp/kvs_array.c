#include"kvstore.h"
#include<stdio.h>
#include<string.h>
#include<stddef.h>
kvs_array_t global_array={0};
int kvs_array_create(kvs_array_t* inst){
    if(inst==NULL){
        printf("kvs_array_create\n");
        return -1;
    }
    if(inst->table){
        printf("kvs_array_create:table has exist\n");
        return -1;
    }
    inst->table=kvs_malloc(KVS_ARRAY_SIZE*sizeof(kvs_array_item_t));
    if(!inst->table){
        printf("kvs_array_create:inst failed to malloc\n");
        return -1;
    }
    inst->idx=0;
    inst->total=0;
    return 0;
}
void kvs_array_destory(kvs_array_t* inst){
    if(inst==NULL){
        printf("kvs_array_destory\n");
        return;
    }
    if(inst->table){
        kvs_free(inst->table);
    }
    return;

}
char* kvs_array_get(kvs_array_t* inst,char* key){
    if(inst==NULL||key==NULL){
        printf("kvs_array_get\n");
        return NULL;
    }
    if(inst->idx>=KVS_ARRAY_SIZE)return NULL;
    int i=0;
    for(i=0;i<KVS_ARRAY_SIZE;i++){
        if(inst->table[i].key==NULL){
            continue;
        }
        if(strcmp(inst->table[i].key,key)==0){
            return inst->table[i].value;
        }
    }
    return NULL;
}
int kvs_array_set(kvs_array_t* inst,char* key,char* value){
    if(inst==NULL||key==NULL||value==NULL){
        printf("kvs_array_set\n");
        return -1;
    }
    if(inst->idx>=KVS_ARRAY_SIZE) return -1;
    if(kvs_array_get(inst,key)){
        return 2;
    }
    char* kcopy=kvs_malloc(strlen(key)+1);
    if(kcopy==NULL)return -1;
    memset(kcopy,0,strlen(key)+1);
    strncpy(kcopy,key,strlen(key));
    char* vcopy=kvs_malloc(strlen(value)+1);
    if(vcopy==NULL){
        printf("vcopy failed\n");
        return -1;
    }
    memset(vcopy,0,strlen(value)+1);
    strncpy(vcopy,value,strlen(value));
    int i=0;
    for(i=0;i<KVS_ARRAY_SIZE;i++){
        if(inst->table[i].key==NULL){
            inst->table[i].key=kcopy;
            inst->table[i].value=vcopy;
            inst->total++;
            inst->idx=i;
            return 0;
        }
    }
    if(i==inst->total&&i<KVS_ARRAY_SIZE){
        inst->table[i].key=kcopy;
        inst->table[i].value=vcopy;
        inst->total++;
        return 0;
    }
    return -1;
    
}

int kvs_array_del(kvs_array_t* inst,char* key){
    if(inst==NULL||key==NULL){
        printf("kvs_array_del\n");
        return -1;
    }
    if(inst->idx>=KVS_ARRAY_SIZE)return -1;
    int i=0;
    for(i=0;i<KVS_ARRAY_SIZE;i++){
        if(strcmp(inst->table[i].key,key)==0){
            kvs_free(inst->table[i].key);
            inst->table[i].key=NULL;
            kvs_free(inst->table[i].value);
            inst->table[i].value=NULL;
            
            return 0;
        }
    }
    return i;
}
int kvs_array_mod(kvs_array_t* inst,char* key,char* value){
    if(inst==NULL||key==NULL){
        printf("kvs_array_mod\n");
        return -1;
    }
    if(inst->idx>=KVS_ARRAY_SIZE)return -1;
    int i=0;
    for(i=0;i<KVS_ARRAY_SIZE;i++){
        if(inst->table[i].key==NULL){
            continue;
        }
        if(strcmp(inst->table[i].key,key)==0){
            char* vcopy=kvs_malloc(strlen(inst->table[i].value));
            if(vcopy==NULL){
                return -1;
            }
            memset(vcopy,0,strlen(vcopy));
            strncpy(vcopy,value,strlen(value));
            inst->table[i].value=vcopy;
            return 0;
        }
    }
    return i;
}
int kvs_array_exist(kvs_array_t* inst,char* key){
    if(inst==NULL||key==NULL){
        printf("kvs_array_exist\n");
        return -1;
    }
    if(inst->idx>=KVS_ARRAY_SIZE)return -1;
    char* str=kvs_array_get(inst,key);
    if(!str){
        return 2;
    }
    return 0;
}
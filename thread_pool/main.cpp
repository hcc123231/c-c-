#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"thread_pool.h"
void* count_c(void* arg) {
    int* a = (int*)arg;
    *a += 1;
    printf("tid:%d  a:%d\n", pthread_self(), *a);
    usleep(1000);
    return NULL;
}
int main()
{
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    thread_pool_init(pool, 3, 10, 1000);
    int* a = (int*)malloc(sizeof(int));
    *a = 0;
    for (int i = 0; i < 900; i++) {
        task_add(pool, count_c, a);
    }
    sleep(30);
    thread_pool_destory(pool);
    return 0;
}
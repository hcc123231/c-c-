#pragma once
#include<pthread.h>
#define CHANGE_COUNT 3
typedef struct Task
{
	void* (*func)(void* arg);
	void* arg;
}Task;
typedef struct ThreadPool
{
	Task* task_queue;//任务队列
	pthread_t* threadIds;//线程id数组
	int max_tasks;//最大的任务数
	int cur_tasks;//当前任务数
	int queue_front;//队头下标
	int queue_rear;//队尾下标
	int min_thread_count;//最小的线程数
	int max_thread_count;//最大线程数
	int thread_active_count;//线程活跃数
	int thread_count;//当前状态下总线程数
	pthread_t managerId;//管理线程的id
	pthread_cond_t task_queue_empty_cond;
	pthread_cond_t task_queue_full_cond;
	pthread_mutex_t thread_pool_mutex;
	pthread_mutex_t active_count_mutex;
	int exitCount;
	bool shutdown;

}ThreadPool;
//线程池初始化
void* thread_pool_init(ThreadPool* pool,int min_thread_count, int max_thread_count, int max_tasks);
//添加任务
void task_add(ThreadPool* pool,void* (*func)(void* arg),void* arg);
//管理者线程的回调函数
void* manager_callback(void* arg);
//工作线程的回调函数
void* work_callback(void* arg);
//线程池销毁函数
void thread_pool_destory(ThreadPool* pool);

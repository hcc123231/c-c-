#include"thread_pool.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void* thread_pool_init(ThreadPool* pool, int min_thread_count, int max_thread_count, int max_tasks) {
	pool->managerId = 0;
	pool->max_tasks = max_tasks;
	pool->max_thread_count = max_thread_count;
	pool->min_thread_count = min_thread_count;
	pool->queue_front = 0;
	pool->queue_rear = 0;
	pool->shutdown = false;
	pool->thread_active_count = 0;
	pool->thread_count = min_thread_count;
	pool->cur_tasks = 0;
	pool->exitCount = 0;
	pool->task_queue = (Task*)malloc(sizeof(Task) * max_tasks);
	pool->threadIds = (pthread_t*)malloc(sizeof(pthread_t) * max_thread_count);
	memset(pool->threadIds, 0, sizeof(pool->threadIds));
	if (pthread_cond_init(&pool->task_queue_empty_cond, NULL) != 0 ||
		pthread_cond_init(&pool->task_queue_full_cond, NULL)!=0 ||
		pthread_mutex_init(&pool->active_count_mutex, NULL)!=0 ||
		pthread_mutex_init(&pool->thread_pool_mutex, NULL)!=0) {
		perror("init failed");
		return NULL;
	}
	if (pthread_create(&pool->managerId, NULL, manager_callback, pool) != 0) {
		perror("failed to create manager");
		return NULL;
	}
	for (int i = 0; i < min_thread_count; i++) {
		if (pthread_create(&pool->threadIds[i], NULL, work_callback, pool) != 0) {
			perror("failed to create work thread");
			return NULL;
		}
		pthread_detach(pool->threadIds[i]);
	}
}
void thread_pool_destory(ThreadPool* pool) {
	if (pool == NULL) {
		perror("pool is null");
		return;
	}
	if (pthread_mutex_destroy(&pool->active_count_mutex) != 0 ||
		pthread_mutex_destroy(&pool->thread_pool_mutex) != 0 ||
		pthread_cond_destroy(&pool->task_queue_empty_cond) != 0 ||
		pthread_cond_destroy(&pool->task_queue_full_cond) != 0) {
		perror("failed to destroy mutex or cond");
		return;
	}
	free(pool->threadIds);
	pool->threadIds = NULL;
	free(pool->task_queue);
	pool->task_queue = NULL;
}
void* manager_callback(void* arg) {
	
	ThreadPool* pool = (ThreadPool*)arg;
	while (!pool->shutdown) {
		sleep(3);
		//如果任务比总的线程两倍还多那么增加线程数
		if (pool->cur_tasks >= 2 * pool->thread_count && pool->thread_count <=( pool->max_thread_count - CHANGE_COUNT)) {
			for (int i = 0; i < CHANGE_COUNT; i++) {
				for (int j = 0; j < pool->max_thread_count; j++) {
					if (pool->threadIds[j] == 0) {
						pthread_mutex_lock(&pool->active_count_mutex);
						pthread_create(&pool->threadIds[j], NULL, work_callback, pool);
						pthread_detach(pool->threadIds[j]);
						pool->thread_count++;
						pthread_mutex_unlock(&pool->active_count_mutex);
						break;
					}
				}
			}
		}
		//如果活跃线程比总线程两倍还少，就减少线程
		if (pool->thread_active_count <= 2 * pool->thread_count && pool->thread_count - CHANGE_COUNT >= pool->min_thread_count) {
			for (int i = 0; i < CHANGE_COUNT; i++) {
				pthread_cond_signal(&pool->task_queue_empty_cond);
			}
		}
	}
	pthread_exit(NULL);
	
}
void* work_callback(void* arg) {
	if (arg == NULL) {
		return NULL;
	}
	ThreadPool* pool = (ThreadPool*)arg;
	while (1) {
		if (pool->shutdown) {
			pthread_exit(NULL);
		}
		pthread_mutex_lock(&pool->active_count_mutex);
		while (pool->cur_tasks == 0) {
			pthread_cond_wait(&pool->task_queue_empty_cond,&pool->active_count_mutex);

		}
		pthread_mutex_unlock(&pool->active_count_mutex);
		if (pool->exitCount) {
			pthread_mutex_lock(&pool->active_count_mutex);
			pool->exitCount--;
			pthread_mutex_unlock(&pool->active_count_mutex);
			pthread_exit(NULL);
		}
		pthread_mutex_lock(&pool->active_count_mutex);
		Task task = pool->task_queue[pool->queue_front];
		pool->queue_front = (pool->queue_front + 1) % pool->max_tasks;
		pool->cur_tasks--;
		pthread_mutex_unlock(&pool->active_count_mutex);
		pthread_cond_signal(&pool->task_queue_full_cond);
		
		pthread_mutex_lock(&pool->active_count_mutex);
		pool->thread_active_count++;
		pthread_mutex_unlock(&pool->active_count_mutex);
		task.func(task.arg);
		pthread_mutex_lock(&pool->active_count_mutex);
		pool->thread_active_count--;
		pthread_mutex_unlock(&pool->active_count_mutex);
	}
	return NULL;
}
void task_add(ThreadPool* pool, void* (*func)(void* arg), void* arg) {
	if (pool == NULL || func == NULL) {
		return;
	}
	Task task;
	task.arg = arg;
	task.func = func;
	pthread_mutex_lock(&pool->active_count_mutex);
	while (pool->cur_tasks >= pool->max_tasks && !pool->shutdown) {
		pthread_cond_wait(&pool->task_queue_full_cond, &pool->active_count_mutex);
	}
	if (pool->shutdown) {
		return;
	}
	pthread_mutex_unlock(&pool->active_count_mutex);

	pthread_mutex_lock(&pool->active_count_mutex);
	pool->task_queue[pool->queue_rear] = task;
	pool->queue_rear = (pool->queue_rear + 1) % pool->max_tasks;
	pool->cur_tasks++;
	pthread_mutex_unlock(&pool->active_count_mutex);

	pthread_cond_signal(&pool->task_queue_empty_cond);
	
	
	
}
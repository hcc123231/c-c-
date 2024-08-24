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
	Task* task_queue;//�������
	pthread_t* threadIds;//�߳�id����
	int max_tasks;//����������
	int cur_tasks;//��ǰ������
	int queue_front;//��ͷ�±�
	int queue_rear;//��β�±�
	int min_thread_count;//��С���߳���
	int max_thread_count;//����߳���
	int thread_active_count;//�̻߳�Ծ��
	int thread_count;//��ǰ״̬�����߳���
	pthread_t managerId;//�����̵߳�id
	pthread_cond_t task_queue_empty_cond;
	pthread_cond_t task_queue_full_cond;
	pthread_mutex_t thread_pool_mutex;
	pthread_mutex_t active_count_mutex;
	int exitCount;
	bool shutdown;

}ThreadPool;
//�̳߳س�ʼ��
void* thread_pool_init(ThreadPool* pool,int min_thread_count, int max_thread_count, int max_tasks);
//�������
void task_add(ThreadPool* pool,void* (*func)(void* arg),void* arg);
//�������̵߳Ļص�����
void* manager_callback(void* arg);
//�����̵߳Ļص�����
void* work_callback(void* arg);
//�̳߳����ٺ���
void thread_pool_destory(ThreadPool* pool);

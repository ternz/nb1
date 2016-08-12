/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tsqueue.h
 * Author: chenwz5
 *
 * Created on 2016年7月30日, 上午10:30
 */

#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <pthread.h>
#include <queue>

template <typename T>
class TSQueue {
public:
	explicit TSQueue(int size);
	~TSQueue();
	T Pop();
	void Push(T);
	int size();
	bool IsFull();
private:
	pthread_mutex_t mutex_;
	pthread_cond_t cond_read_;
	pthread_cond_t cond_write_;
	std::queue<T> queue_;
	int max_size_;
};

template <typename T>
TSQueue<T>::TSQueue(int size)
	:max_size_(size) {
	pthread_mutex_init(&mutex_, NULL);
	pthread_cond_init(&cond_read_, NULL);
	pthread_cond_init(&cond_write_, NULL);
}

template <typename T>
TSQueue<T>::~TSQueue(){
	pthread_mutex_destroy(&mutex_);
	pthread_cond_destroy(&cond_read_);
	pthread_cond_destroy(&cond_write_);
}

template <typename T>
T TSQueue<T>::Pop() {
	pthread_mutex_lock(&mutex_);
	while(queue_.empty()) {
		pthread_cond_wait(&cond_read_, &mutex_);
	}
	T data = queue_.front();
	queue_.pop();
	pthread_mutex_unlock(&mutex_);
	pthread_cond_signal(&cond_write_);
	return data;
}

template <typename T>
void TSQueue<T>::Push(T data) {
	pthread_mutex_lock(&mutex_);
	while(queue_.size() >= max_size_) {
		pthread_cond_wait(&cond_write_, &mutex_);
	}
	queue_.push(data);
	pthread_mutex_unlock(&mutex_);
	pthread_cond_signal(&cond_read_);
}

template <typename T>
int TSQueue<T>::size() {
	return queue_.size();
}

template <typename T>
int TSQueue<T>::IsFull() {
	return queue_.size() == max_size_;
}



#endif /* TSQUEUE_H */


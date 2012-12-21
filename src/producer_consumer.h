#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#include "thread_pool.h"
#include <pthread.h>


typedef struct {
  pthread_mutex_t *cond_mutex;
  pthread_mutex_t *exclusion_mutex;
  pthread_cond_t *full;
  pthread_cond_t *empty;
  thread_pool *producer_pool;
  thread_pool *consumer_pool;
  void **buffer;
  size_t buffer_item_size;
  uint32_t buffer_tail;
  uint32_t buffer_head;
  uint32_t buffer_max;
  void *(*produce_function)(void *);
  void *(*consume_function)(void *);
} producer_consumer;

void produce(producer_consumer *, void *);
bool init_producer_consumer(producer_consumer *prod_cons,
                        uint32_t producers,
                        uint32_t consumers,
                        size_t size_of_obj,
                        uint32_t max_queue,
                        void *(*produce_function)(void *),
                        void *(*consume_function)(void *));
void start_producer_consumer(producer_consumer *);
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>
#include "producer_consumer.h"
#include "thread_pool.h"

static void *internal_consume(void *);

void 
produce(producer_consumer *prod_cons, void *obj)
{ 
  pthread_mutex_lock(prod_cons->cond_mutex);
  produce_loop: 
  pthread_mutex_lock(prod_cons->exclusion_mutex);
  uint32_t buffer_items = prod_cons->buffer_tail - prod_cons->buffer_head;
  if (buffer_items < prod_cons->buffer_max) {
    if (prod_cons->buffer_head > 0) {
      int counter = 0;
      //shift
      for (int i = prod_cons->buffer_head; i < prod_cons->buffer_tail; ++i) {
        *(prod_cons->buffer + counter) = *(prod_cons->buffer + i);
      }
      prod_cons->buffer_head = 0;
      prod_cons->buffer_head = 0;
      prod_cons->buffer_tail = buffer_items;
    } 
    *(prod_cons->buffer + buffer_items) = obj; 
    prod_cons->buffer_tail += 1;
    pthread_mutex_unlock(prod_cons->exclusion_mutex);
    pthread_mutex_unlock(prod_cons->cond_mutex);
    pthread_cond_signal(prod_cons->full);
  } else {
    pthread_mutex_unlock(prod_cons->exclusion_mutex);
    pthread_cond_wait(prod_cons->empty, prod_cons->cond_mutex);
    goto produce_loop;
  }
}

void *
internal_consume(void *pc)
{
  producer_consumer * prod_cons = (producer_consumer *)pc;
  while(1) {
    pthread_mutex_lock(prod_cons->cond_mutex);
    consume_loop:
    pthread_mutex_lock(prod_cons->exclusion_mutex);
    int32_t buffer_items = prod_cons->buffer_tail - prod_cons->buffer_head;
    if (buffer_items > 0) {
      void *item = (void *)malloc(prod_cons->buffer_item_size);
      if (item == NULL) {
        printf("MALLOC FAIL IN internal_consume\n");
        exit(1);
      }
      memcpy(item, *(prod_cons->buffer + prod_cons->buffer_head), prod_cons->buffer_item_size); 
      free(*(prod_cons->buffer + prod_cons->buffer_head));
      prod_cons->buffer_head += 1;
      pthread_mutex_unlock(prod_cons->exclusion_mutex);
      pthread_mutex_unlock(prod_cons->cond_mutex);
      pthread_cond_signal(prod_cons->empty);
      prod_cons->consume_function(item);
    } else {
      pthread_mutex_unlock(prod_cons->exclusion_mutex);
      pthread_cond_wait(prod_cons->full, prod_cons->cond_mutex);  
      goto consume_loop;
    }
  }
}


bool 
init_producer_consumer( producer_consumer *prod_cons,
                        uint32_t producers,
                        uint32_t consumers,
                        size_t size_of_obj,
                        uint32_t max_queue,
                        void *(*produce_function)(void *),
                        void *(*consume_function)(void *))
{
  prod_cons->produce_function = produce_function;
  prod_cons->consume_function = consume_function;
  prod_cons->producer_pool = (thread_pool *)malloc(sizeof(thread_pool));
  prod_cons->consumer_pool = (thread_pool *)malloc(sizeof(thread_pool));
  if (prod_cons->producer_pool == NULL || prod_cons->consumer_pool == NULL) {
  return false;
  }
  if (!init_pool( prod_cons->producer_pool,
                  producers,
                  produce_function,
                  NULL) || 
      !init_pool( prod_cons->consumer_pool,
                  consumers,
                  internal_consume,
                  prod_cons)) {
    printf("POOL(S) FAILED TO INIT\n");
    return false;
  }
  prod_cons->buffer_max = max_queue;
  prod_cons->buffer_item_size = size_of_obj;
  prod_cons->buffer_tail = 0;
  prod_cons->buffer_head = 0;
  prod_cons->buffer = (void **)malloc(sizeof(void *) * max_queue);
  if (prod_cons->buffer == NULL) {
    printf("init_producer_consumer failed to allocate buffer\n");
    return false;
  }
  prod_cons->exclusion_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); 
  prod_cons->cond_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  prod_cons->full = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)); 
  prod_cons->empty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)); 
  int r1 = pthread_mutex_init(prod_cons->exclusion_mutex, NULL);
  int r2 = pthread_mutex_init(prod_cons->cond_mutex, NULL);
  int r3 = pthread_cond_init(prod_cons->full, NULL);
  int r4 = pthread_cond_init(prod_cons->empty, NULL);  
  if (!r1 && !r2 && !r3 && !r4) {
    return true;
  }
  return false;
} //END init_producer_consumer

void 
start_producer_consumer(producer_consumer *prod_cons)
{
  int r1 = start_pool(prod_cons->producer_pool);
  int r2 = start_pool(prod_cons->consumer_pool);           
  if (!r1 && !r2) {
    wait_pool(prod_cons->producer_pool);
    wait_pool(prod_cons->consumer_pool);
  }
}

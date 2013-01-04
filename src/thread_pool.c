#include "thread_pool.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>


bool 
init_pool(thread_pool *pool, 
          uint8_t thread_count, 
          void *(*boot)(void *),
          void *pass)
{
  pool->thread_count = thread_count;
  //TODO - ERROR CHECKING
  pool->startup_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(pool->startup_mutex, NULL);
  pool->id_mapping = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
  pool->boot = boot;
  if (pass != NULL) {
    pool->use_pass = true;
    pool->pass = pass;  
  } else {
    pool->use_pass = false;
  }
  pool->startup_counter = 0;
  pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
  if (pool->threads != NULL) {
    return true;
  } else {
    return false;
  }
}

void *
internal_boot(void *pool_reference)
{
  thread_pool *tp = (thread_pool *)pool_reference;
  pthread_mutex_lock(tp->startup_mutex);
  //START HERE
  //TODO - ADD PTHREAD_T ID TO POOL LISTING  
  tp->startup_counter += 1; 
  pthread_mutex_unlock(tp->startup_mutex);
  if (tp->use_pass) {
    tp->boot(tp->pass);   
  } else {
    tp->boot(NULL); 
  }
}

uint8_t 
start_pool(thread_pool *pool)
{
  uint8_t c = pool->thread_count;

  uint8_t result;
  for (uint64_t i = 0; i < c; ++i) {
    result = pthread_create(pool->threads + i,
                            NULL,
                            internal_boot,
                            (void *)pool);
    if (result != 0) {
      return result;
    }
  }
  return 0;
}

void
wait_pool(thread_pool *pool)
{
  uint8_t c = pool->thread_count;

  for (uint8_t i = 0; i < c; ++i) {
    pthread_join(*(pool->threads + i), NULL);
  }
}

uint8_t 
free_pool(thread_pool *pool)
{
  free(pool->threads);
  return 0;
}


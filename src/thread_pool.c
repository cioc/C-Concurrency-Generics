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
  pool->boot = boot;
  if (pass != NULL) {
    pool->use_pass = true;
    pool->pass = pass;  
  } else {
    pool->use_pass = false;
  }
  pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
  if (pool->threads != NULL) {
    return true;
  } else {
    return false;
  }
}

uint8_t 
start_pool(thread_pool *pool)
{
  uint8_t c = pool->thread_count;

  uint8_t result;
  for (uint64_t i = 0; i < c; ++i) {
    if (pool->use_pass) {
      result = pthread_create(pool->threads + i, 
                              NULL, 
                              pool->boot, 
                              pool->pass);    
    } else {
      result = pthread_create(pool->threads + i, 
                              NULL, 
                              pool->boot, 
                              (void *)i);       
    }
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


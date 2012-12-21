#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>

typedef struct {
  uint8_t thread_count;
  pthread_t *threads;
  void *(*boot)(void *);
  bool use_pass;
  void *pass;
} thread_pool;

bool init_pool(thread_pool *, uint8_t, void *(*)(void *), void *);
uint8_t start_pool(thread_pool *pool);
void wait_pool(thread_pool *pool);
uint8_t free_pool(thread_pool *pool);

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "../src/producer_consumer.h"

//TODO - NEED TO RENAME FUNCTIONS

producer_consumer prod_cons;

void *
sleep_producer(void *data)
{
  int i = 0; 
  while (i < 10) {
    int64_t *number = (int64_t *)malloc(sizeof(int64_t));
    *number = i;
    produce(&prod_cons, (void *)number);
    ++i;
  }
}

void *
sleep_handler(void *data)
{
  pthread_t self = pthread_self(); 
  printf("%d says Woopie number: %d\n", (int)self, (int)*((int64_t *)(data)));
  free(data);
  sleep(1);  
}

int 
main(int argc, char **args)
{
  printf("STARTING SLEEP EXAMPLE\n");
  bool init_result = init_producer_consumer(&prod_cons, 
                                            1, 
                                            5, 
                                            sizeof(int64_t),
                                            100,
                                            sleep_producer,
                                            sleep_handler,
                                            NULL,
                                            NULL);
  if (init_result) {  
    printf("INIT SUCCESS\n");
    start_producer_consumer(&prod_cons);
    exit(0);
  } else {
    printf("INIT FAILURE\n");
    exit(1);
  }
}

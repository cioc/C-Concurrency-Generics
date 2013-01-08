#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../src/producer_consumer.h"
#include "../src/net_support.h"

producer_consumer prod_cons;

int server_socket;

void *
echo_listener(void *data)
{
  server_socket = tcp_server(8080);
  if (server_socket < 0) {
    printf("Failed to create socket\n");
    exit(1);
  }
  
  int connection;
  printf("Begin accept\n");
  while (1) {
    connection = accept(server_socket, NULL,NULL);
    if (connection < 0) {
      printf("Accept error\n");
      exit(1);
    }
    printf("accepted connection\n");
    int *number = (int *)malloc(sizeof(int));
    *number = connection;
    produce(&prod_cons, (void *)number);
  }
  printf("End accept\n");
  exit(0);
}

void *
echo_handler(void *data)
{
  pthread_t self = pthread_self(); 
  
  int connection = (int)*((int *)(data));

  char buf[100];

  int received = recv(connection, buf, 99, 0);

  printf("Consumer thread %i has received %s\n", (int)self, buf);

  if (received > 0) {
    write(connection, buf, received);
  }

  close(connection);
  free(data);
}

int 
main(int argc, char **args)
{
  printf("STARTING ECHO SERVER\n");
  bool init_result = init_producer_consumer(&prod_cons, 
                                            1, 
                                            5, 
                                            sizeof(int),
                                            100,
                                            echo_listener,
                                            echo_handler,
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

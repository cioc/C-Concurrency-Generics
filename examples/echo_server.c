#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "../src/producer_consumer.h"

producer_consumer prod_cons;

int sock;
struct sockaddr_in echo_server;

void *
echo_listener(void *data)
{

  memset(&echo_server, 0, sizeof(echo_server));
  echo_server.sin_family = AF_INET;
  echo_server.sin_addr.s_addr = htonl(INADDR_ANY);
  echo_server.sin_port = 8080;

  sock = socket(AF_INET, 
                SOCK_STREAM, 
                0);
  if (sock < 0) {
    printf("Failed to create socket\n");
    exit(1);
  }
  
  int r1 = bind(sock, 
                (struct sockaddr *)&echo_server, 
                sizeof(echo_server));
  if (r1 < 0) {
    printf("Failed to bind\n");
    exit(1);
  }
 
  r1 = listen(sock, 20);
  if (r1 < 0) {
    printf("Failed to listen\n");
    exit(1);
  }

  int connection;

  printf("Begin accept\n");
  while (1) {
    connection = accept(sock, NULL,NULL);
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
                                            echo_handler);
  if (init_result) {  
    printf("INIT SUCCESS\n");
    start_producer_consumer(&prod_cons);
    exit(0);
  } else {
    printf("INIT FAILURE\n");
    exit(1);
  }
}

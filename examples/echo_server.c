#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../src/producer_consumer.h"

producer_consumer prod_cons;

int sock;
struct addrinfo try, *result;

void *
echo_listener(void *data)
{
  memset(&try, 0, sizeof(try));
  try.ai_family = AF_INET;
  try.ai_socktype = SOCK_STREAM;
  try.ai_flags = AI_PASSIVE;
  getaddrinfo(NULL, "8080", &try, &result);

  sock = socket(result->ai_family, 
                result->ai_socktype, 
                result->ai_protocol);
  if (sock < 0) {
    printf("Failed to create socket\n");
    exit(1);
  }
  
  int r1 = bind(sock, 
                result->ai_addr, 
                result->ai_addrlen);
  if (r1 < 0) {
    printf("Failed to bind\n");
    exit(1);
  }
 
  r1 = listen(sock,
              20);
  if (r1 < 0) {
    printf("Failed to listen\n");
    exit(1);
  }

  int connection;
  
  socklen_t addr_size;
  struct sockaddr_storage connect_addr;

  while (connection = accept(sock, 
                             (struct sockaddr *)connect_addr),
                             &addr_size > 0) {
    int64_t *number = (int64_t *)malloc(sizeof(int64_t));
    *number = connection;
    produce(&prod_cons, (void *)number);
  }
 
 }
}

void *
echo_handler(void *data)
{
  pthread_t self = pthread_self(); 
  
  int64_t connection = (int)*((int64_t *)(data));
 
  char buf[100];

  int received = recv(connection, buf, 100, 0);

  if (received > 0) {
    send(connection, buf, received, 0);
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
                                            sizeof(int64_t),
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

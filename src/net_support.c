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

#define CONNECTION_QUEUE_SIZE 100


//NOTE: Binds to localhost for now
int 
tcp_server(int port)
{
  int socket_out;
  struct sockaddr_in *server;
  
  server = (sockaddr_in *)malloc(sizeof(struct sockaddr_in));
  if (server == NULL) {
    return -1;
  }
  
  memset(server, 0, sizeof(struct sockaddr_in));
  server->sin_family = AF_INET;
  server->sin_addr.s_addr = htonl(INADDR_ANY);
  server->sin_port = port;

  socket_out = socket(AF_INET, 
                      SOCK_STREAM, 
                      0);
  if (socket_out < 0) {
    free(server);
    return -1;
  }
  
  int r1 = bind(socket_out, 
                (struct sockaddr *)server, 
                sizeof(struct sockaddr_in));
  if (r1 < 0) {
    free(server);
    close(socket_out);
    return -1;
  }

  r1 = listen(sock, CONNECTION_QUEUE_SIZE);
  if (r1 < 0) {
    free(server);
    close(socket_out);
    return -1;
  } 
  free(server);
  return socket_out;
}

int 
tcp_client(char *hostname, int port)
{
  int socket_out;
  socket_out = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_out < 0) {
    return -1;
  }

  struct hostent *host = gethostbyname(hostname);
  if (host == NULL) {
    return -1;
  }

  struct sockaddr_in *server;
  server = (sockaddr_in *)malloc(sizeof(struct sockaddr_in));
  if (server == NULL) {
    free(host);
    return -1;
  }

  memset(server, 0, sizeof(struct sockaddr_in));
  server->sin_family = AF_INET;
  server->sin_port = port;
  memcpy( &(server->sin_addr.s_addr), 
          &(host->h_addr),
          host->h_length);

   if (connect(socket_out, 
              (struct sockaddr *)server, 
              sizeof(struct sockaddr_in)) < 0) {
    free(host);
    free(server);
    return -1;
  }

  free(host);
  free(server);
  return socket_out;
}

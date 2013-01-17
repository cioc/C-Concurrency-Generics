#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include "../src/net_support.h"

int client_socket;

int 
main(int argc, char **args) {
  //this will use UNIX domain sockets 
  client_socket = reliable_connection(NET_ACTIVE, 0, true,"/tmp/tcpipiv3.serv");//tcp_client("localhost", 8080); 
  //to use tcp instead:
  //client_socket = reliable_connection(NET_ACTIVE, 8080, false, "localhost");
  
  if (client_socket < 0) {
    printf("Failed to create tcp_client\n");
    exit(1);
  }
  
  char buf[100];
  
  bzero(buf, 100);
  fgets(buf, 99, stdin);

  int n = write(client_socket, buf, 100);
  
  char buf2[100];
  bzero(buf2, 100);
  n = read(client_socket, buf2, 99);

  if (n < 0 ) {
    close(client_socket);
    printf("Reading error\n");
    exit(1);
  }
  printf("%s\n", buf2);
  close(client_socket);
}

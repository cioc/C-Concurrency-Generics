#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int sock;
struct sockaddr_in server_addr;
struct hostent *echo_server;

int 
main(int argc, char **args) {
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    printf("Failed to create socket\n");
    exit(1);
  }

  echo_server = gethostbyname("localhost");

  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = 8080;
  bcopy((char *)echo_server->h_addr, (char *)&server_addr.sin_addr.s_addr, echo_server->h_length);
  
  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("Failed to connect: %s\n", strerror(errno));
    exit(1);
  }
  
  char buf[100];
  

  bzero(buf, 100);
  fgets(buf, 99, stdin);

  int n = write(sock, buf, 100);
  
  char buf2[100];
  bzero(buf2, 100);
  n = read(sock, buf2, 99);

  if (n < 0 ) {
    close(sock);
    printf("Reading error\n");
    exit(1);
  }
  printf("%s\n", buf2);
  close(sock);
}

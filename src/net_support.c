#include "net_support.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

int 
reliable_connection(int active_passive, int port,bool interprocess, char *host_name) {
  if (active_passive == NET_ACTIVE) {
    if (interprocess) {
      struct sockaddr_un serv;
      memset(&serv, 0, sizeof(serv));
      serv.sun_family = AF_UNIX;
      //bcopy(host_name, serv.sun_path, strlen(host_name));
      strncpy(serv.sun_path, host_name, sizeof(serv.sun_path));
      int fd;
      
      if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        return -1;  
      }
      
      if (connect(fd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        return -1;
      }
      return fd;
    }
    else {
      struct sockaddr_in serv;
      memset(&serv, 0, sizeof(serv));
      struct hostent *host = gethostbyname(host_name);
     
      if (host == NULL) {
        printf("host problems\n");
        return -1;  
      }
      
      serv.sin_family = AF_INET;
      serv.sin_port = port;
      bcopy((char *)host->h_addr, (char *)&serv.sin_addr.s_addr, host->h_length);
      
      int fd;
      if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("died here 1\n");
        return -1;
      }

      if (connect(fd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        printf("died here 2\n");
        return -1;
      }
      return fd;
    }
    
  }
  else {
    if (interprocess) {
      struct sockaddr_un serv;
      memset(&serv, 0, sizeof(serv));
      serv.sun_family = AF_UNIX;
      //bcopy(host_name, serv.sun_path, strlen(host_name));
      strncpy(serv.sun_path, host_name, sizeof(serv.sun_path));
      

      int fd;
      if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("failed to init socket\n");
        return -1;
      }
      
      if(bind(fd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        printf("failed to bind\n");
        return -1;
      } 
      
      if (listen(fd, SOMAXCONN) < 0) {
        printf("failed to listen\n");
        return -1;
      }
      return fd;
       
    }
    else {
      struct sockaddr_in serv;
      memset(&serv, 0, sizeof(serv));
      serv.sin_family = AF_INET;
      serv.sin_addr.s_addr = htonl(INADDR_ANY);
      serv.sin_port = port;

      int fd;
      if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;  
      }
      
      if (bind(fd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        return -1;
      }
      
      if (listen(fd, SOMAXCONN) < 0) {
        return -1;
      }
      return fd;
    }
  }       
}

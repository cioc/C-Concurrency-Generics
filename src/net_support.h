#ifndef NET_SUPPORT_H
#define NET_SUPPORT_H

#include <stdbool.h>

enum {
  NET_ACTIVE = 1,
  NET_PASSIVE = 0  
};

int reliable_connection(int active_passive, int port, bool interprocess, char *host_name);

#endif

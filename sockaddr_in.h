#ifndef SYSLOGREAD__SOCKADDR_IN__H__
#define SYSLOGREAD__SOCKADDR_IN__H__

#include <netinet/in.h>
void make_sockaddr_in(const char* host, const char* port,
                      struct sockaddr_in* sa);

#endif

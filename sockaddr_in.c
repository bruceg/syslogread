#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

extern void usage(const char*);

static int parse_port(const char* str)
{
  struct servent* serv;
  char* ptr;
  int num = strtol(str, &ptr, 10);
  if(!*ptr)
    return num;
  serv = getservbyname(str, "udp");
  if(!serv)
    usage("Unknown port address");
  return serv->s_port;
}

static char* parse_host(const char* str)
{
  struct hostent* host = gethostbyname(str);
  if(!host)
    usage("Unknown host address");
  return host->h_addr;
}

void make_sockaddr_in(const char* host, const char* port,
		      struct sockaddr_in* sa)
{
  memset(sa, 0, sizeof *sa);
  sa->sin_family = AF_INET;
  memcpy(&sa->sin_addr, parse_host(host), 4);
  sa->sin_port = htons(parse_port(port));
}

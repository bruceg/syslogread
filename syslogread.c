#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "syslogread.h"

extern void error(const char*);

static void handle_msg(char* str, ssize_t len)
{
  char* ptr = str;
  while(str[len-1] == 0 || str[len-1] == '\n')
    --len;
  str[len] = '\n';
  /* Try to strip off the timestamp after the facility/level stamp */
  if(*ptr++ == '<') {
    while(isdigit(*ptr))
      ++ptr;
      /* syslog date stamps are of the form:
       * "MMM DD HH:MM:SS "
       * This is just a quick parse for the non-alpha parts */
    if(*ptr++ == '>' &&
       ptr[3] == ' ' &&
       (isdigit(ptr[4]) || isspace(ptr[4])) &&
       isdigit(ptr[5]) && ptr[6] == ' ' &&
       isdigit(ptr[7]) && isdigit(ptr[8]) && ptr[9] == ':' &&
       isdigit(ptr[10]) && isdigit(ptr[11]) && ptr[12] == ':' &&
       isdigit(ptr[13]) && isdigit(ptr[14]) && ptr[15] == ' ') {
      memcpy(str + 15, str, ptr - str);
      len -= 15;
      str += 15;
    }
  }
  write(1, str, len+1);
}
  
void main_loop(unsigned num_sockets, int* sockets)
{
  int max_socket = 0;
  unsigned i;
  for(i = 0; i < num_sockets; i++)
    if(sockets[i] > max_socket)
      max_socket = sockets[i];
  for(;;) {
    fd_set fds;
    FD_ZERO(&fds);
    for(i = 0; i < num_sockets; i++)
      FD_SET(sockets[i], &fds);
    select(max_socket+1, &fds, 0, 0, 0);
    for(i = 0; i < num_sockets; i++)
      if(FD_ISSET(sockets[i], &fds)) {
	char buf[1024];
	ssize_t r = recvfrom(sockets[i], buf, sizeof buf - 1, 0, 0, 0);
	if(r == -1)
	  error("Receive from selected socket failed");
	else
	  handle_msg(buf, r);
      }
  }
}

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "syslogread.h"

extern void error(const char*);

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
	else {
	  while(buf[r-1] == 0 || buf[r-1] == '\n')
	    --r;
	  buf[r] = '\n';
	  write(1, buf, r+1);
	}
      }
  }
}

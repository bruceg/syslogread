#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "syslogread.h"

gid_t parse_gid(const char* str)
{
  gid_t gid;
  if(!str)
    usage("GID is not set");
  gid = atoi(str);
  if(gid <= 0)
    usage("GID is not a positive number");
  return gid;
}

uid_t parse_uid(const char* str)
{
  uid_t uid;
  if(!str)
    usage("UID is not set");
  uid = atoi(str);
  if(uid <= 0)
    usage("UID is not a positive number");
  return uid;
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
	else {
	  while(buf[r-1] == 0 || buf[r-1] == '\n')
	    --r;
	  buf[r] = '\n';
	  write(1, buf, r+1);
	}
      }
  }
}

void setuidgid(uid_t uid, gid_t gid)
{
  if(gid)
    if(setgid(gid))
      die("Could not set GID");
  if(uid)
    if(setuid(uid))
      die("Could not set UID");
}

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "syslogread.h"

void write_err(const char* str)
{
  write(2, str, strlen(str));
}

void error(const char* msg)
{
  write_err("sysloglread: Error: ");
  write_err(msg);
  write_err("\n");
}

void die(const char* msg)
{
  error(msg);
  exit(1);
}

void usage(const char* msg)
{
  if(msg)
    error(msg);
  write_err("Usage: sysloglread [-g GID] [-u UID] [-U] [-m MASK] PATH ...\n");
  exit(1);
}

static int* sockets;
static char** socket_names;
static int num_sockets;

static gid_t opt_gid = 0;
static uid_t opt_uid = 0;
static mode_t opt_umask = 0111;

void parse_args(int argc, char* argv[])
{
  int ch;
  while((ch = getopt(argc, argv, "g:m:u:U")) != EOF) {
    switch(ch) {
    case 'm':
      opt_umask = strtoul(optarg, &optarg, 8);
      if(*optarg)
	usage("MASK is not a positive octal integer");
      break;
    case 'g': opt_gid = parse_gid(optarg); break;
    case 'u': opt_uid = parse_uid(optarg); break;
    case 'U':
      opt_gid = parse_gid(getenv("GID"));
      opt_uid = parse_uid(getenv("UID"));
      break;
    default:
      usage(0);
    }
  }
  num_sockets = argc - optind;
  if(num_sockets <= 0)
    usage("You must specify at least one socket");
  socket_names = argv + optind;
  sockets = calloc(num_sockets, sizeof(int));
}

void make_sockets(void)
{
  int i;
  umask(opt_umask);
  for(i = 0; i < num_sockets; i++)
    if((sockets[i] = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
      die("Could not create socket");
  for(i = 0; i < num_sockets; i++) {
    struct sockaddr_un sa;
    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, socket_names[i]);
    unlink(socket_names[i]);
    if(bind(sockets[i], &sa, sizeof sa))
      die("Could not bind socket to path");
  }
  setuidgid(opt_uid, opt_gid);
}

int main(int argc, char* argv[])
{
  parse_args(argc, argv);
  make_sockets();
  main_loop(num_sockets, sockets);
  return 0;
}


#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "syslogread.h"
#include "setuidgid.h"

void write_err(const char* str)
{
  write(2, str, strlen(str));
}

void error(const char* msg)
{
  write_err("sysloguread: Error: ");
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
  write_err("Usage: sysloguread [-g GID] [-u UID] [-U] IP PORT ...\n");
  exit(1);
}

static int* sockets;
static char** socket_names;
static int num_sockets;

static gid_t opt_gid = 0;
static uid_t opt_uid = 0;

void parse_args(int argc, char* argv[])
{
  int ch;
  while((ch = getopt(argc, argv, "g:u:U")) != EOF) {
    switch(ch) {
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
  if((argc - optind) % 2)
    usage("Missing port number");
  num_sockets = (argc - optind) / 2;
  if(num_sockets <= 0)
    usage("You must specify at least one socket");
  socket_names = argv + optind;
  sockets = calloc(num_sockets, sizeof(int));
}

int parse_port(const char* str)
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

char* parse_host(const char* str)
{
  struct hostent* host = gethostbyname(str);
  if(!host)
    usage("Unknown host address");
  return host->h_addr;
}

void make_sockets(void)
{
  int i;
  for(i = 0; i < num_sockets; i++)
    if((sockets[i] = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      die("Could not create socket");
  for(i = 0; i < num_sockets; i++) {
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    memcpy(&sa.sin_addr, parse_host(socket_names[i*2]), 4);
    sa.sin_port = htons(parse_port(socket_names[i*2+1]));
    if(bind(sockets[i], &sa, sizeof(sa)))
      die("Could not bind socket to address");
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


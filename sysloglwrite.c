#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "syslogwrite.h"
#include "setuidgid.h"

void write_err(const char* str)
{
  write(2, str, strlen(str));
}

void error(const char* msg)
{
  write_err("sysloglwrite: Error: ");
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
  write_err("Usage: sysloglwrite [-g GID] [-u UID] [-U] [-R] PATH\n");
  exit(1);
}

static char* socket_name;

static gid_t opt_gid = 0;
static uid_t opt_uid = 0;
static int opt_reopen = 0;

void parse_args(int argc, char* argv[])
{
  int ch;
  while((ch = getopt(argc, argv, "g:Ru:U")) != EOF) {
    switch(ch) {
    case 'g': opt_gid = parse_gid(optarg); break;
    case 'R': opt_reopen = 1;              break;
    case 'u': opt_uid = parse_uid(optarg); break;
    case 'U':
      opt_gid = parse_gid(getenv("GID"));
      opt_uid = parse_uid(getenv("UID"));
      break;
    default:
      usage(0);
    }
  }
  if(optind != argc - 1)
    usage("You must specify exactly one socket path");
  socket_name = argv[optind];
}

int make_socket(const char* path)
{
  struct sockaddr_un sa;
  int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if(fd == -1)
    die("Could not create socket");
  sa.sun_family = AF_UNIX;
  strcpy(sa.sun_path, path);
  if(connect(fd, (struct sockaddr*)&sa, sizeof sa))
    die("Could not connect to path");
  if(!opt_reopen)
    setuidgid(opt_uid, opt_gid);
  return fd;
}

void write_message(int fd, const char* prefix, const char* message)
{
  if(opt_reopen)
    fd = make_socket(socket_name);
  write(fd, prefix, strlen(prefix));
  write(fd, message, strlen(message));
  if(opt_reopen)
    close(fd);
}

int main(int argc, char* argv[])
{
  parse_args(argc, argv);
  main_loop(opt_reopen ? 0 : make_socket(socket_name));
  return 0;
}


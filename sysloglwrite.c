/* sysloglwrite.c - Local syslog writer.
 * Copyright (C) 2001  Bruce Guenter <bruceg@em.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "names.h"
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
  write_err("Usage: sysloglwrite [-g GID] [-u UID] [-U] [-p] [-r] PATH FACILITY [PREFIX]\n");
  exit(1);
}

static const char* socket_name;
static int facility;
static const char* prefix = "";

static gid_t opt_gid = 0;
static uid_t opt_uid = 0;
static int opt_reopen = 0;
static int opt_strip_priority = 0;

void parse_args(int argc, char* argv[])
{
  int ch;
  while((ch = getopt(argc, argv, "g:pru:U")) != EOF) {
    switch(ch) {
    case 'g': opt_gid = parse_gid(optarg); break;
    case 'p': opt_strip_priority = 1;      break;
    case 'r': opt_reopen = 1;              break;
    case 'u': opt_uid = parse_uid(optarg); break;
    case 'U':
      opt_gid = parse_gid(getenv("GID"));
      opt_uid = parse_uid(getenv("UID"));
      break;
    default:
      usage(0);
    }
  }
  if(argc - optind < 2)
    usage("Missing PATH or FACILITY argument");
  if(argc - optind > 3)
    usage("Too many command-line arguments");
  socket_name = argv[optind++];
  facility = facility_number(argv[optind]);
  if(facility < 0)
    usage("Invalid facility name");
  if(argc - optind >= 1)
    prefix = argv[optind++];
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

void write_message(int fd, const char* message, size_t msg_len)
{
  if(opt_reopen)
    fd = make_socket(socket_name);
  if(send(fd, message, msg_len, 0) == -1)
    error("Could not send message to server");
  if(opt_reopen)
    close(fd);
}

int main(int argc, char* argv[])
{
  parse_args(argc, argv);
  main_loop(opt_reopen ? 0 : make_socket(socket_name),
	    prefix, facility, opt_strip_priority);
  return 0;
}


/* sysloguwrite.c - UDP syslog writer.
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
#include <unistd.h>
#include "names.h"
#include "setuidgid.h"
#include "sockaddr_in.h"
#include "syslogwrite.h"

void write_err(const char* str)
{
  write(2, str, strlen(str));
}

void error(const char* msg)
{
  write_err("sysloguwrite: Error: ");
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
  write_err("Usage: sysloguwrite [-g GID] [-u UID] [-U] [-p] HOST PORT FACILITY [PREFIX]\n");
  exit(1);
}

static int sockfd;
static int facility;
static const char* prefix = "";

static gid_t opt_gid = 0;
static uid_t opt_uid = 0;
static int opt_strip_priority = 0;

void parse_args(int argc, char* argv[])
{
  struct sockaddr_in sa;
  const char* host;
  const char* port;
  int ch;
  while((ch = getopt(argc, argv, "g:u:Up")) != EOF) {
    switch(ch) {
    case 'g': opt_gid = parse_gid(optarg); break;
    case 'p': opt_strip_priority = 1;      break;
    case 'u': opt_uid = parse_uid(optarg); break;
    case 'U':
      opt_gid = parse_gid(getenv("GID"));
      opt_uid = parse_uid(getenv("UID"));
      break;
    default:
      usage(0);
    }
  }
  if(argc - optind < 3)
    usage("Missing HOST, PORT, or FACILITY argument");
  if(argc - optind > 4)
    usage("Too many command-line arguments");
  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    die("Could not create socket");
  host = argv[optind++];
  port = argv[optind++];
  make_sockaddr_in(host, port, &sa);
  if(connect(sockfd, &sa, sizeof sa))
    die("Could not connect to server");
  facility = facility_number(argv[optind++]);
  if(facility < 0)
    usage("Invalid facility name");
  if(argc - optind >= 1)
    prefix = argv[optind++];
}

void write_message(int fd, const char* message, size_t msg_len)
{
  if(send(fd, message, msg_len, 0) == -1)
    error("Could not send message to server");
}

int main(int argc, char* argv[])
{
  parse_args(argc, argv);
  main_loop(sockfd, prefix, facility, opt_strip_priority);
  return 0;
}


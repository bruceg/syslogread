/* sockaddr_in.c - Set up a sockaddr_in structure.
 * Copyright (C) 2005  Bruce Guenter <bruceg@em.ca>
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
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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

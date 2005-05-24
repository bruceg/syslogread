/* syslogwrite.c - Generic system log writing main loop.
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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "syslogwrite.h"

extern void write_message(int fd, const char* message, size_t msg_len);

void main_loop(int fd, const char* prefix,
	       unsigned facility, int strip_priority, int preserve_syslog)
{
  char inbuf[2048];
  char outbuf[2048];
  while(fgets(inbuf, sizeof inbuf, stdin)) {
    unsigned priority = LOG_INFO;
    const char* msgptr = inbuf;
    int in_preform = 0;
    unsigned int in_val = 0;
    if(*msgptr == '<' && preserve_syslog) {
      const char* endptr = msgptr + 1;
      if(isdigit(*endptr)) {
        do {
          in_val *= 10;
          in_val += *endptr - '0';
          ++endptr;
	} while(isdigit(*endptr));
	if(*endptr == '>') {
	  msgptr = endptr + 1;
	  in_preform = 1;
	}
      }
    }
    if(in_preform)
      ;
    else if(!strncasecmp(inbuf, "emergency:", 10)) {
      priority = LOG_ALERT;
      msgptr = inbuf + 10;
    }
    else if(!strncasecmp(inbuf, "alert:", 6)) {
      priority = LOG_ALERT;
      msgptr = inbuf + 6;
    }
    else if(!strncasecmp(inbuf, "critical:", 9)) {
      priority = LOG_CRIT;
      msgptr = inbuf + 9;
    }
    else if(!strncasecmp(inbuf, "error:", 6)) {
      priority = LOG_ERR;
      msgptr = inbuf + 6;
    }
    else if(!strncasecmp(inbuf, "warning:", 8)) {
      priority = LOG_WARNING;
      msgptr = inbuf + 8;
    }
    else if(!strncasecmp(inbuf, "notice:", 7)) {
      priority = LOG_NOTICE;
      msgptr = inbuf + 7;
    }
    else if(!strncasecmp(inbuf, "info:", 5)) {
      priority = LOG_INFO;
      msgptr = inbuf + 5;
    }
    else if(!strncasecmp(inbuf, "debug:", 6)) {
      priority = LOG_DEBUG;
      msgptr = inbuf + 6;
    }
    while(isspace(*msgptr))
      ++msgptr;
    if(in_preform)
      sprintf(outbuf, "<%u>%s", in_val, msgptr);
    else
      sprintf(outbuf, "<%u>%s%s", priority + (facility << 3),
	      prefix, strip_priority ? msgptr : inbuf);
    write_message(fd, outbuf, strlen(outbuf));
  }
}

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "syslogwrite.h"

extern void write_message(int fd, const char* message, size_t msg_len);

void main_loop(int fd, const char* prefix,
	       unsigned facility, int strip_priority)
{
  char inbuf[2048];
  char outbuf[2048];
  while(fgets(inbuf, sizeof inbuf, stdin)) {
    unsigned priority = LOG_INFO;
    const char* msgptr = inbuf;
    if(!strncasecmp(inbuf, "alert:", 6)) {
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
    sprintf(outbuf, "<%u>%s%s", priority + (facility << 3),
	    prefix, strip_priority ? msgptr : inbuf);
    write_message(fd, outbuf, strlen(outbuf));
  }
}

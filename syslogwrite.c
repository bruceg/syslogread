#include <stdio.h>
#include "syslogwrite.h"

extern void write_message(int fd, const char* prefix, const char* str);

void main_loop(int fd)
{
  char buf[1024];
  while(fgets(buf, sizeof buf, stdin)) {
    write_message(fd, "<>", buf);
  }
}

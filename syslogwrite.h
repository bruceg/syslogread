#ifndef SYSLOGREAD__SYSLOG_WRITE__H__
#define SYSLOGREAD__SYSLOG_WRITE__H__

void main_loop(int fd, const char* prefix,
	       unsigned facility, int strip_priority);

#endif

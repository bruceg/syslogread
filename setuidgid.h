#ifndef SYSLOGREAD__SET_UID_GID__H__
#define SYSLOGREAD__SET_UID_GID__H__

#include <sys/types.h>
gid_t parse_gid(const char* str);
uid_t parse_uid(const char* str);
void setuidgid(uid_t, gid_t);

#endif

#include <stdlib.h>
#include <unistd.h>
#include "setuidgid.h"

extern void usage(const char*);
extern void die(const char*);

gid_t parse_gid(const char* str)
{
  gid_t gid;
  if(!str)
    usage("GID is not set");
  gid = atoi(str);
  if(gid <= 0)
    usage("GID is not a positive number");
  return gid;
}

uid_t parse_uid(const char* str)
{
  uid_t uid;
  if(!str)
    usage("UID is not set");
  uid = atoi(str);
  if(uid <= 0)
    usage("UID is not a positive number");
  return uid;
}

void setuidgid(uid_t uid, gid_t gid)
{
  if(gid)
    if(setgid(gid))
      die("Could not set GID");
  if(uid)
    if(setuid(uid))
      die("Could not set UID");
}

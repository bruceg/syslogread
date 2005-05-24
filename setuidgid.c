/* setuidgid.c - Parse and set UID/GID values.
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

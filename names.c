/* names.c - Standard symbolic names for syslog code values.
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
#include <string.h>
#include "names.h"

static const char* tmp_priority_names[] = {
  "emerg",
  "alert",
  "crit",
  "err",
  "warning",
  "notice",
  "info",
  "debug"
};
const char** priority_names = tmp_priority_names;

int priority_number(const char* name)
{
  int i;
  for(i = 0; i < priority_count; i++) {
    if(!strcasecmp(name, priority_names[i]))
      return i;
  }
  return -1;
}

static const char* tmp_facility_names[] = {
  "kern",
  "user",
  "mail",
  "daemon",
  "auth",
  "syslog",
  "lpr",
  "news",
  "uucp",
  "cron",
  "authpriv",
  "ftp",
  "RESERVED",
  "RESERVED",
  "RESERVED",
  "RESERVED",
  "local0",
  "local1",
  "local2",
  "local3",
  "local4",
  "local5",
  "local6",
  "local7"
};
const char** facility_names = tmp_facility_names;

int facility_number(const char* name)
{
  int i;
  for(i = 0; i < facility_count; i++) {
    if(!strcasecmp(name, facility_names[i]))
      return i;
  }
  return -1;
}

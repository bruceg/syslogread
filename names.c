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

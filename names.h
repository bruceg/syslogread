#ifndef SYSLOGREAD__NAMES__H__
#define SYSLOGREAD__NAMES__H__

const char** priority_names;
#define priority_count 8
int priority_number(const char* name);

const char** facility_names;
#define facility_count 24
int facility_number(const char* name);

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "names.h"

static int selected[facility_count][priority_count] = {{0,}};

const char* usage_str =
"Usage: syslogxlate [-FPx] PRIORITY.FACILITY [PRIORITY.FACILITY...]
  Selects and translates syslog formatted messages.

  -F  Show the facility name or number
  -P  Show the priority name or number
  -x  Do not translate priority or facility numbers into names

  PRIORITY is one of emerg, alert, crit, err, warning, notice, info, or debug.
  FACILITY is one of kern, user, mail, daemon, auth, syslog, lpr, news, uucp,
                     cron, authpriv, ftp, or local0-local7.
  See the documentation for a detailed explanation of PRIORITY and FACILITY.
";

static int opt_show_facility = 0;
static int opt_show_priority = 0;
static int opt_xlate_names = 1;

void usage(const char* msg)
{
  if(msg)
    fprintf(stderr, "syslogxlate: Error: %s\n", msg);
  fputs(usage_str, stderr);
  exit(1);
}

void select_facility(unsigned facility, unsigned priority,
		     unsigned value, unsigned equal)
{
  if(equal)
    selected[facility][priority] = value;
  else {
    unsigned i;
    for(i = 0; i <= priority; i++)
      selected[facility][i] = value;
  }
}

void parse_facility(const char* str, unsigned* facility, unsigned* all_fac)
{
  if(str[0] == '*' && !str[1])
    *all_fac = 1;
  else {
    int i = facility_number(str);
    if(i < 0)
      usage("Unknown facility name");
    *facility = i;
    *all_fac = 0;
  }
}

void parse_priority(const char* str, unsigned* priority,
		    unsigned* negate, unsigned* equal)
{
  *negate = 0;
  while(*str == '!') {
    ++str;
    *negate = !*negate;
  }
  *equal = 0;
  while(*str == '=') {
    ++str;
    *equal = 1;
  }
  if(str[0] == '*' && !str[1])
    *priority = priority_count-1;
  else {
    int i = priority_number(str);
    if(i < 0)
      usage("Unknown priority name");
    *priority = i;
  }
}

void parse_selector(char* str)
{
  char* ptr;
  unsigned facility;
  unsigned priority;
  unsigned neg_pri;
  unsigned all_fac;
  unsigned eq_pri;
  unsigned i;
  
  ptr = strchr(str, '.');
  if(!ptr)
    usage("Invalid selector format");
  *ptr++ = 0;
  
  parse_facility(str, &facility, &all_fac);
  parse_priority(ptr, &priority, &neg_pri, &eq_pri);
  
  if(all_fac)
    for(i = 0; i < facility_count; i++)
      select_facility(i, priority, !neg_pri, eq_pri);
  else
    select_facility(facility, priority, !neg_pri, eq_pri);
}
      
void parse_args(int argc, char* argv[])
{
  int ch;
  while((ch = getopt(argc, argv, "FPtx")) != EOF) {
    switch(ch) {
    case 'F': opt_show_facility = 1;   break;
    case 'P': opt_show_priority = 1;   break;
    case 'x': opt_xlate_names = 0;     break;
    default:
      usage(0);
    }
  }
  if(optind >= argc)
    usage("You need to select at least one PRIORITY.FACILITY");
  for(; optind < argc; optind++)
    parse_selector(argv[optind]);
}

const char* parse_syslog(const char* line,
			 unsigned* facility, unsigned* priority)
{
  unsigned num;
  if(*line++ != '<')
    return 0;
  num = strtoul(line, (char**)&line, 10);
  if(num >= priority_count*facility_count || *line++ != '>')
    return 0;
  *facility = num >> 3;
  *priority = num & 7;
  return line;
}

void print_prefix(unsigned facility, unsigned priority)
{
  if(!opt_show_priority && !opt_show_facility)
    return;
  fputc('<', stdout);
  if(opt_show_facility) {
    if(opt_xlate_names)
      fputs(facility_names[facility], stdout);
    else
      printf("%u", facility);
  }
  if(opt_show_priority) {
    if(opt_show_facility)
      fputc(',', stdout);
    if(opt_xlate_names)
      fputs(priority_names[priority], stdout);
    else
      printf("%u", priority);
  }
  fputc('>', stdout);
  fputc(' ', stdout);
}

void mainloop(void)
{
  char buf[4096];
  while(fgets(buf, 4096, stdin)) {
    unsigned facility;
    unsigned priority;
    const char* ptr = parse_syslog(buf, &facility, &priority);
    if(ptr) {
      if(selected[facility][priority]) {
	print_prefix(facility, priority);
	fputs(ptr, stdout);
      }
    }
    else
      fputs(buf, stdout);
    fflush(stdout);
  }
}

int main(int argc, char* argv[])
{
  parse_args(argc, argv);
  mainloop();
  return 0;
}

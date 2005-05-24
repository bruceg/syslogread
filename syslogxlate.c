/* syslogxlate.c - Translate syslog messages
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "names.h"

static int selected[facility_count][priority_count] = {{0,}};

const char* usage_str =
"Usage: syslogxlate [-fpx] PRIORITY.FACILITY [PRIORITY.FACILITY...]\n"
"  Selects and translates syslog formatted messages.\n"
"\n"
"  -f  Do not show the facility name or number\n"
"  -p  Do not show the priority name or number\n"
"  -x  Do not translate priority or facility numbers into names\n"
"\n"
"  PRIORITY is one of emerg, alert, crit, err, warning, notice, info, or debug.\n"
"  FACILITY is one of kern, user, mail, daemon, auth, syslog, lpr, news, uucp,\n"
"                     cron, authpriv, ftp, or local0-local7.\n"
"  See the documentation for a detailed explanation of PRIORITY and FACILITY.\n";

static int opt_show_facility = 1;
static int opt_show_priority = 1;
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
  while((ch = getopt(argc, argv, "fptx")) != EOF) {
    switch(ch) {
    case 'f': opt_show_facility = 0;   break;
    case 'p': opt_show_priority = 0;   break;
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

/* Defined by syslogread.c */
gid_t parse_gid(const char* str);
uid_t parse_uid(const char* str);
void main_loop(unsigned num_sockets, int* sockets);
void setuidgid(uid_t, gid_t);

/* Used by syslogread.c */
extern void usage(const char*);
extern void die(const char*);
extern void error(const char*);

#include <installer.h>
#include "conf_bin.c"
#include "conf_man.c"

void insthier(void)
{
  int bin = opendir(conf_bin);
  int man = opendir(conf_man);
  int man1;

  c(bin, "sysloglread",  -1, -1, 0755);
  c(bin, "sysloglwrite", -1, -1, 0755);
  c(bin, "sysloguread",  -1, -1, 0755);
  c(bin, "sysloguwrite", -1, -1, 0755);
  c(bin, "syslogxlate",  -1, -1, 0755);

  man1 = d(man, "man1", -1, -1, 0755);
  c(man1, "sysloglread.1",  -1, -1, 0644);
  c(man1, "sysloglwrite.1", -1, -1, 0644);
  c(man1, "sysloguread.1",  -1, -1, 0644);
  c(man1, "sysloguwrite.1", -1, -1, 0644);
  c(man1, "syslogxlate.1",  -1, -1, 0644);
}

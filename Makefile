PACKAGE = syslogread
VERSION = 0.90

CC = gcc
CFLAGS = -O1 -W -Wall -g

LD = gcc
LDFLAGS = -g

RM = rm -f

INSTALL = install

PROGRAMS = sysloglread sysloglwrite sysloguread syslogxlate
SCRIPTS = syslogconf2svc
MAN1S = sysloglread.1 sysloguread.1 syslogxlate.1

all: $(PROGRAMS)

sysloglread: sysloglread.o syslogread.o setuidgid.o
	$(LD) $(LDFLAGS) -o $@ sysloglread.o syslogread.o setuidgid.o $(LIBS)

sysloglwrite: sysloglwrite.o syslogwrite.o setuidgid.o
	$(LD) $(LDFLAGS) -o $@ sysloglwrite.o syslogwrite.o setuidgid.o $(LIBS)

sysloguread: sysloguread.o syslogread.o setuidgid.o
	$(LD) $(LDFLAGS) -o $@ sysloguread.o syslogread.o setuidgid.o $(LIBS)

sysloguwrite: sysloguwrite.o syslogwrite.o setuidgid.o
	$(LD) $(LDFLAGS) -o $@ sysloguwrite.o syslogwrite.o setuidgid.o $(LIBS)

syslogxlate: syslogxlate.o names.o
	$(LD) $(LDFLAGS) -o $@ syslogxlate.o names.o $(LIBS)

names.o: names.c names.h
setuidgid.o: setuidgid.c setuidgid.h
syslogread.o: syslogread.c syslogread.h
syslogwrite.o: syslogwrite.c syslogwrite.h
sysloglread.o: sysloglread.c syslogread.h setuidgid.h
sysloglwrite.o: sysloglwrite.c syslogwrite.h setuidgid.h
sysloguread.o: sysloguread.c syslogread.h setuidgid.h
sysloguwrite.o: sysloguwrite.c syslogwrite.h setuidgid.h
syslogxlate.o: syslogxlate.c names.h

install: all
	$(INSTALL) -d $(DESTDIR)/usr/bin
	$(INSTALL) $(PROGRAMS) $(SCRIPTS) $(DESTDIR)/usr/bin

	$(INSTALL) -d $(DESTDIR)/usr/man/man1
	$(INSTALL) -m 644 $(MAN1S) $(DESTDIR)/usr/man/man1

clean:
	$(RM) core multilog qfilelog spipe *.o

PACKAGE = syslogread
VERSION = 0.90

CC = gcc
CFLAGS = -O1 -W -Wall -g

LD = gcc
LDFLAGS = -g

RM = rm -f

INSTALL = install
prefix = /usr

PROGRAMS = sysloglread sysloglwrite sysloguread sysloguwrite syslogxlate
SCRIPTS = syslogconf2svc
MAN1S = sysloglread.1 sysloglwrite.1 sysloguread.1 sysloguwrite.1 syslogxlate.1

all: $(PROGRAMS)

sysloglread: sysloglread.o syslogread.o setuidgid.o
	$(LD) $(LDFLAGS) -o $@ sysloglread.o syslogread.o setuidgid.o $(LIBS)

sysloglwrite: sysloglwrite.o syslogwrite.o setuidgid.o names.o
	$(LD) $(LDFLAGS) -o $@ sysloglwrite.o syslogwrite.o setuidgid.o names.o $(LIBS)

sysloguread: sysloguread.o syslogread.o setuidgid.o sockaddr_in.o
	$(LD) $(LDFLAGS) -o $@ sysloguread.o syslogread.o \
		setuidgid.o sockaddr_in.o $(LIBS)

sysloguwrite: sysloguwrite.o syslogwrite.o setuidgid.o names.o sockaddr_in.o
	$(LD) $(LDFLAGS) -o $@ sysloguwrite.o syslogwrite.o \
		setuidgid.o names.o sockaddr_in.o $(LIBS)

syslogxlate: syslogxlate.o names.o
	$(LD) $(LDFLAGS) -o $@ syslogxlate.o names.o $(LIBS)

names.o: names.c names.h
setuidgid.o: setuidgid.c setuidgid.h
sockaddr_in.o: sockaddr_in.c sockaddr_in.h
syslogread.o: syslogread.c syslogread.h
syslogwrite.o: syslogwrite.c syslogwrite.h names.h
sysloglread.o: sysloglread.c syslogread.h setuidgid.h
sysloglwrite.o: sysloglwrite.c syslogwrite.h setuidgid.h names.h
sysloguread.o: sysloguread.c syslogread.h setuidgid.h sockaddr_in.h
sysloguwrite.o: sysloguwrite.c syslogwrite.h setuidgid.h names.h sockaddr_in.h
syslogxlate.o: syslogxlate.c names.h

install: all
	$(INSTALL) -d $(DESTDIR)$(prefix)/bin
	$(INSTALL) $(PROGRAMS) $(SCRIPTS) $(DESTDIR)$(prefix)/bin

	$(INSTALL) -d $(DESTDIR)$(prefix)/man/man1
	$(INSTALL) -m 644 $(MAN1S) $(DESTDIR)$(prefix)/man/man1

clean:
	$(RM) core multilog qfilelog spipe *.o

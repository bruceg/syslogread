PACKAGE = syslogread
VERSION = 0.90

CC = gcc
CFLAGS = -O1 -W -Wall -g

LD = gcc
LDFLAGS = -g

RM = rm -f

INSTALL = install
prefix = /usr
bindir = $(prefix)/bin
mandir = $(prefix)/man
svcdir = /var/service

PROGRAMS = sysloglread sysloglwrite sysloguread sysloguwrite syslogxlate
SCRIPTS = syslogconf2svc
MAN1S = sysloglread.1 sysloglwrite.1 sysloguread.1 sysloguwrite.1 syslogxlate.1
SERVICES = sysloglread

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
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PROGRAMS) $(DESTDIR)$(bindir)

	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL) -m 644 $(MAN1S) $(DESTDIR)$(mandir)/man1

	@set -ex; for service in $(SERVICES); do \
	  $(INSTALL) -d $(DESTDIR)/$(svcdir)/$$service; \
	  $(INSTALL) -m 755 $$service.run $(DESTDIR)/$(svcdir)/$$service/run; \
	  $(INSTALL) -m 755 $$service-log.run $(DESTDIR)/$(svcdir)/$$service/log/run; \
	done

clean:
	$(RM) core $(PROGRAMS) *.o

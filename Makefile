PACKAGE = syslogread
VERSION = 0.90

CC = gcc
CFLAGS = -O1 -W -Wall -g

LD = gcc
LDFLAGS = -g

RM = rm -f

INSTALL = install

PROGRAMS = sysloglread sysloguread syslogxlate
SCRIPTS = syslogconf2svc
MAN1S = sysloglread.1 sysloguread.1 syslogxlate.1

all: $(PROGRAMS)

sysloglread: sysloglread.o syslogread.o
	$(LD) $(LDFLAGS) -o $@ sysloglread.o syslogread.o $(LIBS)

sysloguread: sysloguread.o syslogread.o
	$(LD) $(LDFLAGS) -o $@ sysloguread.o syslogread.o $(LIBS)

syslogxlate: syslogxlate.o
	$(LD) $(LDFLAGS) -o $@ $< $(LIBS)

sysloglread.o: sysloglread.c syslogread.h
sysloguread.o: sysloguread.c syslogread.h
syslogxlate.o: syslogxlate.c
syslogread.o: syslogread.c syslogread.h

install: all
	$(INSTALL) -d $(DESTDIR)/usr/bin
	$(INSTALL) $(PROGRAMS) $(SCRIPTS) $(DESTDIR)/usr/bin

	$(INSTALL) -d $(DESTDIR)/usr/man/man1
	$(INSTALL) -m 644 $(MAN1S) $(DESTDIR)/usr/man/man1

clean:
	$(RM) core multilog qfilelog spipe *.o

Name: @PACKAGE@
Summary: Syslog message handling tools
Version: @VERSION@
Release: 1
Copyright: GPL
Group: Utilities/System
Source: @PACKAGE@-@VERSION@.tar.gz
BuildRoot: /tmp/@PACKAGE@-root
Packager: Bruce Guenter <bruceg@em.ca>
Conflicts: sysklogd
Requires: supervise-scripts >= 3.2

%description
This package is a set of tools that are useful in receiving, analyzing,
or producing syslog messages.

%prep
%setup

%build
make CFLAGS="$RPM_OPT_FLAGS" LDFLAGS="-s"

%install
rm -fr $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT bindir=%{_bindir} mandir=%{_mandir} install

%clean
rm -rf $RPM_BUILD_ROOT

%pre
PATH="/sbin:/usr/sbin:$PATH" export PATH
add_user() { grep "^$1:" /etc/passwd >/dev/null || useradd -d "$3" -g "$2" -M -r -s /bin/true "$1"; }
add_group() { grep "^$1:" /etc/group >/dev/null || groupadd -r "$1"; }

add_group syslogrd
add_user  syslogrd syslogrd /var/log/sysloglread

%post
if ! [ -e /service/sysloglread ]
then
  echo "Type 'svc-add sysloglread' to start the local syslog server."
fi

%preun
case "$1" in
0)
	# pre-erase instructions
	svc-remove sysloglread
	;;
esac

%postun
PATH="/sbin:/usr/sbin:$PATH" export PATH
case "$1" in
0)
	# post-erase instructions
	userdel syslogrd
	groupdel syslogrd
	;;
esac

%files
%defattr(-,root,root)
%doc ANNOUNCEMENT NEWS README syslogconf2svc
%{_bindir}/*
%{_mandir}/man1/*

%dir %attr(-,syslogrd,syslogrd) /var/log/syslog
%dir /var/service/sysloglread
%dir /var/service/sysloglread/log
%config(noreplace) /var/service/sysloglread/run
%config(noreplace) /var/service/sysloglread/log/run

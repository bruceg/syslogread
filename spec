Name: @PACKAGE@
Summary: Syslog message handling tools
Version: @VERSION@
Release: 2
Copyright: GPL
Group: Utilities/System
Source: @PACKAGE@-@VERSION@.tar.gz
Packager: Bruce Guenter <bruceg@em.ca>
Conflicts: sysklogd
Requires: supervise-scripts >= 3.2
Provides: sysklogd-1.3.31
BuildRoot: %{_tmppath}/@PACKAGE@-root
BuildRequires: bglibs >= 1.007

%description
This package is a set of tools that are useful in receiving, analyzing,
or producing syslog messages.

%prep
%setup

%build
echo gcc $RPM_OPT_FLAGS >conf-cc
echo gcc -s >conf-ld
make

%install
rm -fr $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{_bindir}
mkdir -p $RPM_BUILD_ROOT%{_mandir}
mkdir -p $RPM_BUILD_ROOT/service
mkdir -p $RPM_BUILD_ROOT/var/service/sysloglread/log

echo $RPM_BUILD_ROOT%{_bindir} >conf-bin
echo $RPM_BUILD_ROOT%{_mandir} >conf-man
rm insthier.o installer instcheck
make installer instcheck
./installer
./instcheck
install -m 755 sysloglread.run $RPM_BUILD_ROOT/var/service/sysloglread/run
install -m 755 sysloglread-log.run $RPM_BUILD_ROOT/var/service/sysloglread/log/run
mkdir -p $RPM_BUILD_ROOT/var/log/syslog

%clean
rm -rf $RPM_BUILD_ROOT

%pre
PATH="%{_sbindir}:$PATH" export PATH
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
PATH="/sbin:%{_sbindir}:$PATH" export PATH
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
%dir %attr(1755,root,root) /var/service/sysloglread
%dir %attr(0755,root,root) /var/service/sysloglread/log
%config(noreplace) /var/service/sysloglread/run
%config(noreplace) /var/service/sysloglread/log/run

Name: @PACKAGE@
Summary: Syslog message handling tools
Version: @VERSION@
Release: 1
Copyright: GPL
Group: Utilities/System
Source: @PACKAGE@-@VERSION@.tar.gz
BuildRoot: /tmp/@PACKAGE@
Packager: Bruce Guenter <bruceg@em.ca>
Requires: daemontools
Requires: qlogtools

%description
This package is a set of tools that are useful in receiving, analyzing,
or producing syslog messages.

%prep
%setup

%build
make CFLAGS="$RPM_OPT_FLAGS" LDFLAGS="-s"

%install
rm -fr $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc ANNOUNCEMENT NEWS README
/usr/bin/*
/usr/man/man1/*

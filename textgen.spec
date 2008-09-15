%define LIBNAME textgen
Summary: textgen library
Name: libsmartmet-%{LIBNAME}
Version: 8.9.15
Release: 1.el5.fmi
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires: libsmartmet-newbase >= 8.9.11-1, mysql-devel
Provides: %{LIBNAME}

%description
FMI textgen library

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{LIBNAME}
 
%build
make %{_smp_mflags}

%install
%makeinstall includedir=%{buildroot}%{_includedir}/smartmet

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0775)
%{_includedir}/smartmet/%{LIBNAME}
%{_libdir}/libsmartmet_%{LIBNAME}.a


%changelog
* Mon Sep 15 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.15-1.el5.fmi
- Compiled with newbase 8.9.15-1
* Tue Jul 15 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.7.15-1.el5.fmi
- Compiled with newbase 8.7.15-1
* Wed Mar 12 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.3.12-1.el5.fmi
- Fixed a memory leak in forest fire warning reader
* Tue Mar 11 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.3.11-1.el5.fmi
- Linked with newbase 8.3.11-1 with wind interpolation fixes
* Wed Jan 30 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.1.30-1.el5.fmi
- Linked with newbase 8.1.25-3
* Thu Dec 27 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.7-1.el5.fmi
- Linked with bugfixed newbase 1.0.7-1
* Fri Nov 30 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.6-1.el5.fmi
- Linked with bugfixed newbase 1.0.5-1
* Fri Nov 23 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.5-1.el5.fmi
- IntegerRange behaviour changed based on YLE:s request
* Mon Nov 19 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.4-1.el5.fmi
- Linked with newbase 1.0.4-1
* Thu Nov 15 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.3-1.el5.fmi
- Linked with newbase 1.0.3-1
* Thu Oct 18 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.2-1.el5.fmi
- NFmiStaticTime::EpochTime from newbase into use
- Fixed regression test compilation system
- Newbase includes are now explicit
* Mon Sep 24 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-4.el5.fmi
- Fixed "make depend".
* Fri Sep 14 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-3.el5.fmi
- Added "make tag" feature.
* Thu Sep 13 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-2.el5.fmi
- Improved make system.
* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.

%define LIBNAME textgen
Summary: textgen library
Name: libsmartmet-%{LIBNAME}
Version: 12.3.2
Release: 1.el6.fmi
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires: libsmartmet-newbase >= 12.2.24
BuildRequires: boost-devel >= 1.47
BuildRequires: mysql-devel
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
* Fri Mar  2 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.2-1.el6.fmi
- DebugTextFormatter now separates things with newlines
* Thu Mar  1 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.1-3.el6.fmi
- Fixed a typo in the keyword for wind turning to the east
* Thu Mar  1 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.1-2.el6.fmi
- Fixed the degree sign to the correct UTF-8 value
* Thu Mar  1 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.1-1.el6.fmi
- Improvements to temperature stories
- Improvements to weather stories
- Major improvements to wind stories
* Thu Dec 15 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.12.15-3.el6.fmi
- Fixed temperature clamping to work in all situations
* Thu Dec 15 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.12.15-2.el6.fmi
- Fixed temperature interval logic
* Thu Dec 15 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.12.15-1.el6.fmi
- Latest UTF-8 version
* Thu Nov  3 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.11.3-1.el6.fmi
- Fixed 0 and 1 degree phrases
* Tue Nov  1 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.11.1-1.el6.fmi
- Fine tuning of algorithms. Do not require knowledge of growth season.
* Mon Oct 31 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.10.31-1.el6.fmi
- A large number of new improved algorithms
* Tue Jul 13 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.7.13-1.el5.fmi
- Removed excessive use of shared_ptr which caused segfaults, -O2 is back
* Mon Jul 12 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.7.12-1.el5.fmi
- Compiled without optimization to prevent segmentation faults
* Mon Jul  5 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.7.5-1.el5.fmi
- Upgraded to newbase 10.7.5
- New temperature and weather stories by Anssi Reponen
* Thu May 20 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.5.20-1.el5.fmi
- temperature_nightlymin no longer throws if there are 0 nights
* Wed Mar 17 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.3.17-1.el5.fmi
- Added ListedPeriodGenerator
* Mon Mar 15 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.3.15-1.el5.fmi
- g++ caused memory leaks when shared_ptr was used
* Thu Mar 11 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.3.11-1.el5.fmi
- Added PrecipitationRate
* Fri Jan 15 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.1.15-1.el5.fmi
- Upgrade to boost 1.41
* Tue May 12 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.5.12-1.el5.fmi
- Updated weather_overview story
* Tue Apr 21 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.4.21-1.el5.fmi
- Build single- and multithreaded versions
* Mon Apr  6 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.4.6-1.el5.fmi
- Recompile due to WIN32 and NFmiSaveBase removal
* Mon Mar 16 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.3.16-1.el5.fmi
- Added maximum wind parameter
* Mon Sep 29 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.29-1.el5.fmi
- Newbase header change forced rebuild
* Mon Sep 22 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.22-1.el5.fmi
- Compiled with static boost 1.36
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

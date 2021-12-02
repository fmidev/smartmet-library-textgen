%define DIRNAME textgen
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
%define DEVELNAME %{SPECNAME}-devel
Summary: textgen library
Name: %{SPECNAME}
Version: 21.12.2
Release: 1%{?dist}.fmi
License: FMI
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-textgen
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires: boost169-devel
BuildRequires: gcc-c++
BuildRequires: gdal33-devel
BuildRequires: make
BuildRequires: mariadb-devel
BuildRequires: mysql++-devel
BuildRequires: rpm-build
BuildRequires: smartmet-library-calculator-devel >= 21.12.2
BuildRequires: smartmet-library-macgyver-devel >= 21.12.1
BuildRequires: smartmet-library-newbase-devel >= 21.12.1
BuildRequires: smartmet-library-gis-devel >= 21.12.2
Requires: gdal33-libs
Requires: smartmet-library-calculator >= 21.12.2
Requires: smartmet-library-macgyver >= 21.12.1
Requires: smartmet-library-newbase >= 21.12.1

%if %{defined el7}
Requires: libpqxx < 1:7.0
BuildRequires: libpqxx-devel < 1:7.0
#TestRequires: libpqxx-devel < 1:7.0
%else
%if %{defined el8}
Requires: libpqxx >= 1:7.6.0, libpqxx < 1:7.7.0
BuildRequires: libpqxx-devel >= 1:7.6.0, libpqxx-devel < 1:7.7.0
#TestRequires: libpqxx-devel >= 1:7.6.0, libpqxx-devel < 1:7.7.0
%else
Requires: libpqxx
BuildRequires: libpqxx-devel
#TestRequires: libpqxx-devel
%endif
%endif

Provides: %{SPECNAME}
Obsoletes: libsmartmet-textgen < 17.1.4
Obsoletes: libsmartmet-textgen-debuginfo < 17.1.4
#TestRequires: boost169-devel
#TestRequires: gcc-c++
#TestRequires: mysql++-devel
#TestRequires: smartmet-library-calculator-devel >= 21.12.2
#TestRequires: smartmet-library-macgyver-devel >= 21.12.1
#TestRequires: smartmet-library-newbase-devel >= 21.12.1
#TestRequires: smartmet-library-gis-devel >= 21.12.1
#TestRequires: smartmet-library-regression
#TestRequires: smartmet-timezones
#TestRequires: php-cli

%description
FMI textgen library

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}
 
%build
make %{_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0775,root,root,0775)
%{_libdir}/lib%{LIBNAME}.so

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%package -n %{DEVELNAME}
Summary: FMI textgen development files
Provides: %{DEVELNAME}
Requires: %{SPECNAME}
Obsoletes: libsmartmet-textgen-devel < 17.1.4

%description -n %{DEVELNAME}
FMI textgen development files

%files -n %{DEVELNAME}
%defattr(0664,root,root,0775)
%{_includedir}/smartmet/%{DIRNAME}

%changelog
* Tue Oct 12 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.10.12-1.fmi
- More clang-tidy

* Wed Oct  6 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.10.6-1.fmi
- Used clang-tidy to modernize code

* Fri Sep 17 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.9.17-1.fmi
- Added libpqxx requirements

* Wed Sep 15 2021 Anssi Reponen <anssi.reponen@fmi.fi> - 21.9.15-2.fmi
- Added support for textgen::schema option in PostgreSQL dictionary ('textgen' is default)

* Wed Sep 15 2021 Anssi Reponen <anssi.reponen@fmi.fi> - 21.9.15-1.fmi
- Support for PostgreSQL dictionary database added (BRAINSTORM-1707)

* Thu May  6 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.5.6-1.fmi
- Repackaged due to NFmiAzimuthalArea ABI changes

* Wed Apr 21 2021 Anssi Reponen <anssi.reponen@fmi.fi> - 21.4.21-1.fmi
- Added two missing precipitation scenarios (SOL-12468)
- precipitation type changing from snow showers to sleet showers or vice versa

* Thu Feb 18 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.2.18-1.fmi
- Repackaged due to NFmiArea ABI changes

* Tue Feb 16 2021 Andris Pavēnis <andris.pavenis@fmi.fi> - 21.2.16-1.fmi
- Repackaged due to newbase ABI changes

* Mon Feb 15 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.2.15-1.fmi
- Repackaged due to newbase ABI changes

* Tue Feb  9 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.2.9-1.fmi
- Fixed potential nullptr dereference

* Thu Jan 14 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.14-1.fmi
- Repackaged smartmet to resolve debuginfo issues

* Tue Dec 15 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.12.15-1.fmi
- Upgrade to pgdg12

* Fri Dec  4 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.12.4-1.fmi
- Improved logging of area names and coordinates

* Wed Oct  7 2020 Andris Pavenis <andris.pavenis@fmi.fi> - 20.10.7-1.fmi
- Build update: use makefile.inc from smartmet-library-macgyver
- Link libsmartmet_textgen.so with libboost_locale and mysqlpp
- Fail if unresolved references found in libsmartmet_textgen.so

* Thu Aug 27 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.27-1.fmi
- NFmiGrid API changed

* Wed Aug 26 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.26-1.fmi
- Repackaged due to NFmiGrid API changes

* Fri Aug 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.21-1.fmi
- Upgrade to fmt 6.2

* Fri Jul 31 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.7.31-1.fmi
- Repackaged due to libpqxx upgrade

* Wed Jul  1 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.7.1-1.fmi
- Repackaged

* Sun Apr 26 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.26-1.fmi
- Repackaged

* Sat Apr 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.18-1.fmi
- Upgrade to Boost 1.69

* Thu Mar 26 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.3.26-1.fmi
- Repackaged due to NFmiArea API changes

* Thu Feb 27 2020 Anssi Reponen <anssi.reponen@fmi.fi> - 20.2.27-1.fmi
- If GustSpeed value is missing, use WindSpeed maximum value (BRAINSTORM-1776)

* Wed Feb 12 2020 Anssi Reponen <anssi.reponen@fmi.fi> - 20.2.12-1.fmi
- Freezing drizzle must be reported if it exists in querydata (BRAINSTORM-1754)

* Thu Feb  6 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.2.6-1.fmi
- NFmiPoint Z-coordinate was removed from the ABI

* Wed Dec  4 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.12.4-1.fmi
- Use -fno-omit-frame-pointer for a better profiling and debugging experience                                                                                           

* Tue Dec 3 2019 Anssi Reponen <anssi.reponen@fmi.fi> - 19.12.3-1.fmi
- Checking the parameters for missing value (BRAINSORM-1727)

* Wed Nov 20 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.20-1.fmi
- Repackaged due to newbase API changes

* Thu Oct 31 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.10.31-1.fmi
- Rebuilt due to newbase API/ABI changes

* Wed Oct 9 2019 Anssi Reponen <anssi.reponen@fmi.fi> - 19.10.9-1.fmi
- Corrections in swedish translation, both in database and in the code (BRAINSTORM-1692)

* Thu Sep 26 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.26-1.fmi
- Avoid 'using namespace boost' to avoid lookup issues
- Added support for GDAL 2
- Added ASAN and TSAN builds

* Mon Sep  2 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.2-1.fmi
- Fixed geofind API not to return references

* Mon Oct  1 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.10.1-1.fmi
- Added option -g to get a proper debuginfo package

* Sun Sep 16 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.9.16-1.fmi
- Repackaged since calculator library API changed

* Thu Sep 13 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.9.13-1.fmi
- Made MessageLogger thread specific
- Added MessageLogger ability to log to memory via stringstreams

* Wed Aug  1 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.8.1-1.fmi
- Use C++11 for-loops instead of BOOST_FOREACH

* Wed Jul 25 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.7.25-1.fmi
- Prefer nullptr over NULL

* Mon Jul 23 2018 Anssi Reponen <anssi.reponen@fmi.fi> - 18.7.23-2.fmi
- Removed obsolete and erroneous code (BRAINSTORM-1251)

* Mon Jul 23 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.7.23-1.fmi
- Removed superfluous destructors
- Added checks against nullptr
- Prefer C++11 lround over old style casting

* Wed May  2 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.5.2-1.fmi
- Recpackaged since newbase NFmiEnumConverter ABI changed

* Sat Apr  7 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.4.7-1.fmi
- Upgrade to boost 1.66

* Wed Jan 17 2018 Anssi Reponen <anssi.reponen@fmi.fi> - 18.1.17-1.fmi
- Fixed period handling for seasons: SOL-6065

* Tue Jan 9 2018 Anssi Reponen <anssi.reponen@fmi.fi> - 18.1.9-1.fmi
- Fixed time phrase bug: use 'huomenna' phrase when referring to second day (SOL-6128)

* Tue Nov 21 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.11.21-1.fmi
- Fixed wind direction handling bug: ignore temporal direction change

* Tue Oct 24 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.10.24-1.fmi
- Fixed reporting of wind direction change in the end of forecast period
- Fixed reporting of wind speed interval when wind speed is decreasing, but in the end increases a bit but not enough to be reported
- Improved reporting of wind speed interval when there is only small change

* Mon Oct 9 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.10.9-1.fmi
- Reporting wind direction changes when wind speed speed doesn't change during the whole forecast period

* Tue Sep 12 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.9.12-1.fmi
- Recompiled since replacing auto_ptr with unique_ptr changed the ABI

* Thu Aug 31 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.8.31-1.fmi
- fixed bugs in wind_forecast-story:
- fixed handling of wind speed change periods (there must be at least one period to report)
- fixed handling of wind speed intervals (dont report several intervals in the same part of the day)
- fixed handling wind direction change periods (deleted short period was reported)
- code cleaned

* Tue Aug 29 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.8.29-1.fmi
- fixed bug in reporting increasing/decreasing wind speed (wind speed change in the end of forecast period)

* Mon Aug 28 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.8.28-1.fmi
- Upgrade to boost 1.65

* Tue Aug 22 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.8.22-1.fmi
- fixed reporting of wind direction when wind speed doesn't change 
- fixed logic how right time phrase is decided: strict period borders are not used, but coverage is inspected instead (at least 80% must be inside the phrase period)
- dont report bad visibility when precipitation form is water
- fixed reporting of similar wind speed intervas in successive sentences
- fixed handling of varying wind (don't report strengthening wind when direction is varying)
- fixed missing rain in visibility forecast

* Wed Jun 14 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.6.14-1.fmi
- bad visibility ('huono näkyvyys') logic added
- new configuration parameter 'short_text_mode' added for forecast_at_sea-story
in order to enable shorter texts in press-producs: if short_text_mode is true, 
'huono näkyvyys', 'enimmäkseen hyvä näkyvyys' and 'muuten hyvä näkyvyys' phrases are not reported
- 'vähän'-word not used with weakening wind
- tautology bug fixed: report only one varying wind period even if there is a short 
period of directed wind in between

* Thu Jun 1 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.6.1-1.fmi
- fog forecast fixed and adapted to visibility forecast

* Wed May 31 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.5.31-1.fmi
- fixed handling of long continuous precipitation periods

* Wed May 24 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.5.24-1.fmi
- wind forecast fix: handling of events on the same part of the day corrected

* Tue May 23 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.5.23-1.fmi
- temperature anomaly story fixed: repeated sentence removed
- wind forecast fixed: report wind speed even it is weak during whole forecast period 
- precipitation forecast fixed: added missing check of type of precipitation when sentence is selected
- weather forecast at sea fixed: whole forecast period need not to be reported if there is no non-weak precipitation

* Thu May 18 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.5.18-2.fmi
- fixed wind speed reporting in first sentence

* Thu May 18 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.5.18-1.fmi
- time phrase handling corrected

* Tue May 16 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.5.16-1.fmi
- new story weather_forecast_at_sea added, fixed wind_overview, weather_forecast bugs:
- new weather forecast story 'weather_forecast_at_sea' reports precipitation, thunder, fog and visibility at sea
- sea breeze: If wind is not weak (> 5m/s) but it is varying (deviation > 45 degrees), we dont report wind direction, for example NOT 'Kohtalaista suunnaltaan vaihtelevaa tuulta', but 'Kohtalaista tuulta'
- Use wew configuration parameters for thuder forecast (default values below) instead of hard coded values:
  summertime::thunder::normal_extent_min = 5
  summertime::thunder::normal_extent_max = 30
  wintertime::thunder::normal_extent_min = 10
  wintertime::thunder::normal_extent_max = 30
  thunder::small_probability_min = 10
  thunder::small_probability_max = 20
  thunder::normal_probability_min = 30
  thunder::normal_probability_max = 50
- fixed wind_overview bugs for example:
  - tautology in time phrases in successive sentences
  - when wind weakens/strenghtens, report maximum two wind speed interval instead of three

* Mon Apr 24 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.4.24-1.fmi
- Fixed to use C++1 compiler options
- Added necessary override declarations

* Thu Apr 20 2017  Anssi Reponen <anssi.reponen@fmi.fi> - 17.4.20-1.fmi
- Temperature anomaly story corrected: basic data types not passed by reference

* Tue Apr 11 2017 Teemu Sirviö <teemu.sirvio@fmi.fi> - 17.4.11-1.fmi
- Fixed casting of strings in error messages

* Tue Mar 14 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.14-1.fmi
- Fixed to compile on Windows

* Tue Feb 21 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.2.21-1.fmi
- 'Vähän' word is not used with weakening wind
- Don't repeat 'vomistuvaa/heikkenevää'-phrase when reporting direction change during strenghtening/weakening wind
- Missing wind direction distribution data added
* Thu Feb 16 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.2.16-1.fmi
- Corrected wrong order (in time) of sentences in wind story
* Fri Feb 10 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.2.10-1.fmi
- Additional rule added in deduction of temperature interval (max36hours-story): If minimum temperature is less than -20 degrees and diffrece between minimum and maximum temperature is more than 10 degrees, then the whole interval is shown as such, e.g -6...-23
- Old wind story correction (daily_ranges-story) : Check if any of the sentence elements are empty before constructing the sentence (example of bug: Tänään, huomenna heikkoa luoteenpuoleista tuulta)
- Wind story corrections:
- Catching the case of slowly (but not continuously) strenghtening wind in the middle of the forecast period (ref: B5 20170201 15:00)
- If there is three successive periods with 'alkaen'-phrase, dont use 'alkaen phrase on the second period even if it is longer than 6h (e.g. Aamusta alkaen etelätuulta 5-10m/s, iltapäivällä lounaan puoleista tuulta, illasta alkaen länsituulta) 
- 'Vomistuvaa'-phrase is not used when wind is varying.
- 'ylimmillään'-phrase is not used when wind is weakening 
- When reporting weakening/strenghtening wind speed intervals, the gap between reporting times must be at least three hours (e.g. aluksi 3...5m/s, iltapäivällä 8..13m/s, illasta alkaen 15..19m/s)
- Code cleaned and refactored 

* Fri Jan 27 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.27-1.fmi
- Recompiled due to NFmiQueryData object size change

* Thu Jan 12 2017 Anssi Reponen <anssi.reponen@fmi.fi> - 17.1.12-1.fmi
- Splitted calculator code into a separate smartmet-library-calculator module
- Wind story corrections:
- Successive strengthening or weakening periods are reported in one sentence
- If there are two successive strengthening or weakening period, but a
long (> 6h) period in between, when wind speed doesn't change, then for
the latter strengthening/weakening period we start using edelleen-phrase ('wind continues to strenghten/weaken')

* Wed Jan  4 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.4-1.fmi
- Switched to FMI open source naming conventions

* Wed Sep 14 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.9.14-1.fmi
- cardinal direction and 'puoleinen'-expresson with same cardinal direction can not be successively (e.g. etelätuulta-etelänpuoleista tuulta), but we have to use 'välistä'-expression in the latter (eg. etelätuulta-etelän ja kaakon välistä tuulta)
- calculation of wind speed lower and upper limit corrected (rounding)
- calculated wind in the graph corrected
- log writing added
- version string added
- code cleaned
- regression tests corrected

* Thu Sep  8 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.9.8-1.fmi
- Changed not to use C++11 features to enable compiling on RHEL6

* Thu Aug 4 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.8.4-1.fmi
- Wind story correction:
- 'Aamuyöllä-ja aamulla'-phrase replaced with 'Aamuyöllä'
- Reporting wind direction and wind speed change together when they happen close to each other (on the same part of the day)

* Wed Aug 3 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.8.3-1.fmi
- Temperature anomaly bug fix for summertime: Afternoon maximum (before mean was used) temperature is compared to fractile maximum temperatue.
- More log writing added

* Tue Jun 14 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.14-1.fmi
- Wind story correction:
- Reporting wind speed change and wind direction change together when they happen at the same part of the day

* Fri Jun 10 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.10-1.fmi
- Wind story correction:
- Reporting together two weakening/stregthening periods, when there is relatively short period (< 6h) of unchanged wind between them

* Thu Jun 9 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.9-2.fmi
- Wind story correction:
- Parameter 'wind_speed_warning_threshold' value read from configuration file (hardcoded value used before)

* Thu Jun 9 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.9-1.fmi
- Wind story correction:
- Improved algorithm to detect wind speed changes when wind is weak

* Wed Jun 8 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.8-1.fmi
- Wind story corrections, improvements:
- If speed changes rapidly on the same part of the day, report the latest speed 
- Algorithm to detect wind speed changes improved: some periods that before were reported to have unchanged wind speed is now split to two and and latter is reported to have strengthening or weakening wind
- Algorithm to determine wind speed interval modified: new configuration parameter 'wind_speed_warning_threshold' added, if top wind value is smaller than 'wind_speed_warning_threshold', interval size is 4 and upper limit is taken from maximum value of mean mean wind on the period
- Code cleaned

* Mon Jun 6 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.6-1.fmi
- Frost is not reported, if there is any rain

* Fri Jun 3 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.3-1.fmi
- Wind Story correction:
- Short (< 4h) varying wind period in the beginning is now reported

* Thu Jun 2 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.6.2-1.fmi
- Spelling of wind direction phrase corrected
- If reporting time of wind speed change and direction change is near each other (max 2 hurs), report them at once
- Short varying wind period is not reported if it is not the last direction change
- Weekday name reported when day changes if forecast period is longer than 6 hours
- Code cleaned, more log writing added

* Mon May 30 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.30-1.fmi
- Wind story corrections:
- Wind speed calculaton when determining if wind speed has changed enough to be reported

* Fri May 27 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.27-1.fmi
- Wind story correction:
- If wind is weakening and direction changes to varying, report wind speed interval for the rest of the period, not at the end of weakening period
- Report wind direction in the beginning even if it changes to varying very soon

* Thu May 26 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.26-1.fmi
- Wind Story corrections:
- Corrected time phrase when wind speed and direction change at the same time
- Code cleaned, comments added

* Tue May 24 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.24-1.fmi
- WindStory corrections:
- Weekday error corrected
- Short wind direction period in the beginning of the story is reported

* Mon May 23 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.23-1.fmi
- Wind story corrections:
- Tautology removed in direction change phrases
- When wind direction changes many times during wind speed change period, report only last direction

* Fri May 20 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.20-2.fmi
- Wind story corrections:
- Dont use speed change phrases with varying wind

* Fri May 20 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.5.20-1.fmi
- Wind story corrections

* Fri May 20 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.20-1.fmi
- Wind story corrections:
- Wind speed interval is not reported if wind speed changes during short time 
in the end of forecast period
- Handling of steady period (no significant speed/direction changes) in the end corrected
- Code cleaned

* Thu May 19 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.19-1.fmi
- Wind story corrections:
- Handling situation where wind weakens/strenghtens during long time, but there are short steady periods between
- Corrected bug in timing of reporting the wind speed and direction changes when they occur at the same time or close to each other
- Missing time phrase before wind speed interval corrected
- More log writing added

* Thu May 12 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.12-1.fmi
- Wind story corrections: corrected error in weak wind period deduction, time phrase placement
- More log writing

* Wed May 11 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.11-1.fmi
- Correction in timing of wind direction changes

* Tue May 10 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.10-1.fmi
- Corrections in period handling, time phrases

* Wed May 04 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.4-1.fmi
- Handling of weak wind period corrected
- More log writing added
- Code cleaned

* Tue May 03 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.5.3-1.fmi
- Wind forecast bug corrections and improvements:
- Handling of 'wind_calc_top_share_weak' parameter corrected
- Handling of wind speed changes corrected
- Code cleaned

* Thu Apr 28 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.4.28-1.fmi
- Wind forecast code simplified and cleaned: speed and direction changes are now processed more separately.
- Wind direction is now determined by most common direction (moodi) in all cases (earlier just for varying wind) 
- New configuration parameter 'wind_calc_top_share_weak' added, in order to be able to give 
different weight to top wind based on top wind speed
- Time phrase handling modified, for example: Iltayö ja keskiyö / Keskiyö ja aamuyö -> Yö
- wind_speed_top_coverage (default value 98.0): This parameter tells how many percent the reported top wind speed 
must cover of the total area. For example if top wind on the area is 11 m/s, but it is predicted only on 1.9 % of 
total area, the reported top wind is 10 m/s. 
- wind_direction_min_speed (oletusarvo 6.5): This is limiting value of wind speed for wind direction when standard 
deviation of wind direction is big (> 45.0). If standard deviation of wind direction is big and top wind speed is 
smaller or equal than 'wind_direction_min_speed' wind direction is reportedf as varying.
- wind_speed_top_coverage (default value 98.0): This parameter tells how many percent the reported 
top wind speed must cover of the total area.
For example if top wind on the area is 11 m/s, but it is predicted only on 1.9 % of total area, the reported top wind is 10 m/s. 
- wind_direction_min_speed (oletusarvo 6.5): This is limiting value of wind speed for wind direction when 
standard deviation of wind direction is big (> 45.0). 
If standard deviation of wind direction is big and top wind speed is smaller or equal 
than 'wind_direction_min_speed' wind direction is reported as varying.

* Mon Mar 21 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.3.21-1.fmi
- No minus('-') sign with sub-zero temperatures when 'pakkanen'-phrase is used: 'Pakkasta vajaat -5 astetta' -> 'Pakkasta vajaat 5 astetta'
- 'Vajaat nolla astetta'-phrase corrected to form 'Vähän nollan alapuolella'
- new phrases to database ('vähän nollan alapuolella')
- Highcharts graphs (in wind_overview story) improved for better readibility
- new configuration parameters:
- code cleaned and refactored

* Wed Mar 02 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.3.2-1.fmi
- Optional data tables of wind direction distribution and wind speed distribution improved.
- Data smoothening algorithm modified and default values for related configuration parameters changed
- New confoguration parameter 'wind_calc_top_share' added, this parameter is used when calculating generic wind speed values

* Tue Feb 16 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.2.16-1.fmi
- One new phrase added to database
- Word 'voimakas' not used any more in wind-phrases
- Last time-pharse in 'Iltaan asti' -forecast can not be 'illalla' but 'iltapäivällä'
- Last time-pharse in 'Aamuun asti' -forecast can not be 'aamulla' but 'aamuyöllä'
- Tautology of time-phrases removed
- New configuration parameters added: 
  If qdtext::append_windspeed_distribution is true a table representing windspeed distribution 
  on area during forecast period is appended to html-document.
  If qdtext::append_winddirection_distribution is true a table representing winddirection distribution 
  on area during forecast period is appended to html-document

* Sat Jan 23 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.1.23-1.fmi
- Fmi::TimeZoneFactory API change

* Wed Jan 20 2016 Anssi Reponen <anssi.reponen@fmi.fi> - 16.1.20-1.fmi
- Handling of last hour of forecast period corrected. 
- Calculaton of wind speed corrected: same functions and precision used everywhere.
- If 'qdtext::append_graph' parameter is true a graph (highcharts) is appended to the HTML wind story
- If 'qdtext::append_rawdata' parameter is true a table is appended to the HTML wind story

* Sun Jan 17 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.1.17-1.fmi
- newbase API changed

* Thu Nov 12 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.11.12-1.fmi
- Recompiled to handle the new faster newbase time constructors

* Fri Sep 18 2015 Anssi Reponen <anssi reponen@fmi.fi> - 15.9.18-1.fmi
- New phrases (keskipaivasta/keskipaivaan) added. If 'switchhour' is 0 it is ignored.

* Wed Aug 26 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.26-1.fmi
- Recompiled with latest newbase with faster parameter changing

* Wed Apr 15 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.15-1.fmi
- newbase API changed

* Thu Apr  9 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.9-1.fmi
- newbase API changed

* Mon Mar 30 2015  <mheiskan@centos7.fmi.fi> - 15.3.30-1.fmi
- Use dynamic linking for smartmet libraries

* Fri Oct  3 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.10.3-1.fmi
- textgen::coordinates setting now has a default value

* Fri May 23 2014 Tuomo Lauri <tuomo.lauri@fmi.fi> - 14.5.23-1.fmi
- Fixed raw text inclusions
- Text-stories can now begin with any 'text*' - identifier

* Fri Apr 25 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.4.25-1.fmi
- Wind-related phrases modified
- Redundant phrases removed.
- 'Voimakas tuuli heikkenee'-phrase used when maximum wind is first greater than 17.5 m/s and then weakens.

* Tue Dec 17 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.12.17-2.fmi
- Fixed to calculate aggregates correctly for specific coordinates

* Tue Dec 17 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.12.17-1.fmi
- Updated to latest newbase

* Mon Oct  7 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.10.7-1.fmi
- New UTF-8 beta release

* Mon Jun  3 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.6.3-2.fmi
- Compiled with -fPIC

* Mon Jun  3 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.6.3-1.fmi
- First UTF-8 release

* Wed Nov 14 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.11.14-1.fmi
- Improved weather stories
- Updates required for making a BrainStorm plugin

* Tue Aug 28 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.8.28-1.el6.fmi
- Querydata input files are now memory mapped for speed

* Mon Aug 27 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.8.27-1.el6.fmi
- Added 'date' story
- Added PostGIS support
- Added CSS text formatter
- Output for several products can now be directed to the same file
- Output now goes by default to standard output
- Output can be directed to standard output using - as the file name

* Fri Jul  6 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.7.6-2.el6.fmi
- Fixed cppcheck issues

* Fri Jul  6 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.7.6-1.el6.fmi
- Fixed namespace pollution issues

* Thu Jul  5 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.7.5-1.el6.fmi
- Migration to boost 1.50
- Latest updates from Anssi Reponen

* Wed Apr  4 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.4.4-1.el6.fmi
- Removed debugging logging from wind_overview

* Thu Mar 29 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.29-1.el6.fmi
- Improved wave story now handles separately cases where range low value is zero
- Added text-story

* Wed Mar 28 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.28-1.el6.fmi
- Removed debugging logging from wind_overview
- Fixed UTF-8 capitalization of sentences
- Fixed UTF-8 capitalization of location names
- Added story wave_range

* Tue Mar 27 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.27-1.el6.fmi
- Improvements to wind_overview
- Improvements to temperature_max36hours
- Improvements to weather_forecast

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

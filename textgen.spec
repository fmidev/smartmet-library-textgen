Summary: textgen library
Name: textgen
Version: 1.0
Release: 1
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}
Requires: newbase >= 1.0-1, MySQL-devel-standard >= 5.0.27
Provides: textgen

%description
FMI textgen library

%prep
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

%setup -q -n %{name}
 
%build
make %{_smp_mflags} 

%install
make install prefix="${RPM_BUILD_ROOT}"
#mkdir -p ${RPM_BUILD_ROOT}/smartmet/src/c++/lib/textgen # nämä kohdat tuottavat virheen: corrupted header file size
#cp -r . ${RPM_BUILD_ROOT}/smartmet/src/c++/lib/textgen # nämä kohdat tuottavat virheen: corrupted header file size

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,www,www,0775)
/usr/include/textgen
/usr/lib/libtextgen.a
#/smartmet/src/c++/lib/textgen # nämä kohdat tuottavat virheen: corrupted header file size


%changelog
* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.


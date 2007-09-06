%define LIBNAME textgen
Summary: textgen library
Name: smartmet-%{LIBNAME}
Version: 1.0.1
Release: 1.el5.fmi
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}
Requires: smartmet-newbase >= 1.0.1-1, MySQL-devel-standard >= 5.0.27
Provides: %{LIBNAME}

%description
FMI textgen library

%prep
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

%setup -q -n %{LIBNAME}
 
%build
make clean
make depend
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
* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.


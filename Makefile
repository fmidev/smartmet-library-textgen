LIB = textgen

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

#
# To build serially (helps get the error messages right): make debug SCONS_FLAGS=""
#
SCONS_FLAGS=-j 4

# Installation directories

processor := $(shell uname -p)

ifeq ($(processor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

bindir = $(PREFIX)/bin
includedir = $(PREFIX)/include/smartmet
objdir = obj

# rpm variables

rpmsourcedir = /smartmet/src/redhat/SOURCES
rpmerr = "There's no spec file ($(LIB).spec). RPM wasn't created. Please make a spec file or copy and rename it into $(LIB).spec"

rpmversion := $(shell grep "^Version:" $(LIB).spec  | cut -d\  -f 2 | tr . _)
rpmrelease := $(shell grep "^Release:" $(LIB).spec  | cut -d\  -f 2 | tr . _)

# What to install

LIBFILE = libsmartmet_$(LIB).a
LIBFILE_MT = libsmartmet_$(LIB)-mt.a

# How to install

INSTALL_PROG = install -m 775
INSTALL_DATA = install -m 664

.PHONY: test rpm

#
# The rules
#
SCONS_FLAGS += objdir=$(objdir) prefix=$(PREFIX)

all release:
	scons $(SCONS_FLAGS) $(LIBFILE) $(LIBFILE_MT)

$(LIBFILE) $(LIBFILE_MT):
	scons $(SCONS_FLAGS) $@

debug:
	scons $(SCONS_FLAGS) debug=1 $(LIBFILE) $(LIBFILE_MT)

profile:
	scons $(SCONS_FLAGS) profile=1 $(LIBFILE) $(LIBFILE_MT)

clean:
	@#scons -c objdir=$(objdir)
	-rm -f $(LIBFILE) $(LIBFILE_MT) *~ source/*~ include/*~
	-rm -rf $(objdir)

install:
	@mkdir -p $(includedir)/$(LIB)
	 @list=`cd include && ls -1 *.h`; \
	for hdr in $$list; do \
	  echo $(INSTALL_DATA) include/$$hdr $(includedir)/$(LIB)/$$hdr; \
	  $(INSTALL_DATA) include/$$hdr $(includedir)/$(LIB)/$$hdr; \
	done
	@mkdir -p $(libdir)
	$(INSTALL_DATA) $(LIBFILE) $(libdir)/$(LIBFILE)
	$(INSTALL_DATA) $(LIBFILE_MT) $(libdir)/$(LIBFILE_MT)

test:
	cd test && make test

EXTRAS = $(wildcard docs/*.php)

html:
	@mkdir -p /data/local/html/lib/$(LIB)/docs
	@list='$(EXTRAS)'; \
	for extra in $$list; do \
	  echo $(INSTALL_DATA) $$extra /data/local/html/lib/textgen/$$extra; \
	  $(INSTALL_DATA) $$extra /data/local/html/lib/textgen/$$extra; \
	done
	doxygen $(LIB).dox

rpm: clean
	if [ -e $(LIB).spec ]; \
	then \
	  tar -C ../ -cf $(rpmsourcedir)/libsmartmet-$(LIB).tar $(LIB) ; \
	  gzip -f $(rpmsourcedir)/libsmartmet-$(LIB).tar ; \
	  TAR_OPTIONS=--wildcards rpmbuild -ta $(rpmsourcedir)/libsmartmet-$(LIB).tar.gz ; \
	else \
	  echo $(rpmerr); \
	fi;

tag:
	cvs -f tag 'libsmartmet_$(LIB)_$(rpmversion)-$(rpmrelease)' .

headertest:
	@echo "Checking self-sufficiency of each header:"
	@echo
	@for hdr in `cd include && ls -1 *.h`; do \
	echo $$hdr; \
	echo "#include \"$$hdr\"" > /tmp/$(LIB).cpp; \
	echo "int main() { return 0; }" >> /tmp/$(LIB).cpp; \
	$(CC) $(CFLAGS) $(INCLUDES) -o /dev/null /tmp/$(LIB).cpp $(LIBS); \
	done

analysis:
	@for f in source/*.cpp; do cmd="clang++ --analyze -I include -I /usr/include/smartmet -I /usr/include/mysql -DUNIX -DFMI_COMPRESSION -DBOOST -DBOOST_IOSTREAMS_NO_LIB $$f"; echo $$cmd; $$cmd; done; rm *.plist


mysqldump:
	mysqldump -h base -u textgen --password=w1w2w3 textgen > sql/textgen.sql

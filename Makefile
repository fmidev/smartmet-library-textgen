LIB = textgen

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wredundant-decls -Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Wunreachable-code -Weffc++
CC = g++
ARFLAGS = -r

# Default compiler flags

CFLAGS =  -DUNIX -O2 -DNDEBUG $(MAINFLAGS)

# Special modes

CFLAGS_DEBUG = -DUNIX -O0 -g $(MAINFLAGS) $(EXTRAFLAGS) -Werror
CFLAGS_PROFILE = -DUNIX -O2 -g -pg -DNDEBUG $(MAINFLAGS)

INCLUDES = -I $(includedir) -I $(includedir)/smartmet/newbase -I $(includedir)/mysql
LIBS = -L $(libdir) -lsmartmet-newbase -Lmysql -lmysqlclient

# Common library compiling template

# Installation directories

prosessor := $(shell uname -p)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(prosessor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

bindir = $(PREFIX)/bin
includedir = $(PREFIX)/include
objdir = obj

# rpm variables

rpmsourcedir = /smartmet/src/redhat/SOURCES
rpmerr = "There's no spec file ($(LIB).spec). RPM wasn't created. Please make a spec file or copy and rename it into $(LIB).spec"

rpmversion := $(shell grep "^Version:" $(LIB).spec  | cut -d\  -f 2 | tr . _)
rpmrelease := $(shell grep "^Release:" $(LIB).spec  | cut -d\  -f 2 | tr . _)

# What to install

LIBFILE = libsmartmet_$(LIB).a

# How to install

INSTALL_PROG = install -m 775
INSTALL_DATA = install -m 664

# Compiler flag overrides

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
  CFLAGS = $(CFLAGS_DEBUG)
endif

ifneq (,$(findstring profile,$(MAKECMDGOALS)))
  CFLAGS = $(CFLAGS_PROFILE)
endif

# Compilation directories

vpath %.cpp source
vpath %.h include
vpath %.o $(objdir)

# The files to be compiled

SRCS = $(patsubst source/%,%,$(wildcard source/*.cpp))
HDRS = $(patsubst include/%,%,$(wildcard include/*.h))
OBJS = $(SRCS:%.cpp=%.o)

OBJFILES = $(OBJS:%.o=obj/%.o)

INCLUDES := -I include $(INCLUDES)

.PHONY: test rpm

# The rules

all: objdir $(LIBFILE)
debug: all
release: all
profile: all

$(LIBFILE): $(OBJS)
	$(AR) $(ARFLAGS) $(LIBFILE) $(OBJFILES)

clean:
	rm -f $(LIBFILE) $(OBJFILES) *~ source/*~ include/*~

install:
	@mkdir -p $(includedir)/$(LIB)
	@list='$(HDRS)'; \
	for hdr in $$list; do \
	  if [ include/$$hdr -nt $(includedir)/$(LIB)/$$hdr ]; \
	  then \
	    echo $(INSTALL_DATA) include/$$hdr $(includedir)/$(LIB)/$$hdr; \
	  fi; \
	  $(INSTALL_DATA) include/$$hdr $(includedir)/$(LIB)/$$hdr; \
	done
	@mkdir -p $(libdir)
	$(INSTALL_DATA) $(LIBFILE) $(libdir)/$(LIBFILE)

depend:
	makedepend $(INCLUDES)

test:
	cd test && make test

html::
	mkdir -p /data/local/html/lib/$(LIB)
	doxygen $(LIB).dox

objdir:
	@mkdir -p $(objdir)

rpm: clean depend
	if [ -e $(LIB).spec ]; \
	then \
	  tar -C ../ -cf $(rpmsourcedir)/libsmartmet-$(LIB).tar $(LIB) ; \
	  gzip -f $(rpmsourcedir)/libsmartmet-$(LIB).tar ; \
	  rpmbuild -ta $(rpmsourcedir)/libsmartmet-$(LIB).tar.gz ; \
	else \
	  echo $(rpmerr); \
	fi;

tag:
	cvs -f tag 'libsmartmet_$(LIB)_$(rpmversion)-$(rpmrelease)' .

headertest:
	@echo "Checking self-sufficiency of each header:"
	@echo
	@for hdr in $(HDRS); do \
	echo $$hdr; \
	echo "#include \"$$hdr\"" > /tmp/$(LIB).cpp; \
	echo "int main() { return 0; }" >> /tmp/$(LIB).cpp; \
	$(CC) $(CFLAGS) $(INCLUDES) -o /dev/null /tmp/$(LIB).cpp $(LIBS); \
	done

.SUFFIXES: $(SUFFIXES) .cpp

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $(objdir)/$@ $<

# Extra html installation

EXTRAS = $(wildcard docs/*.php)
html::
	@mkdir -p $(prefix)/html/lib/textgen/docs
	@list='$(EXTRAS)'; \
	for extra in $$list; do \
	  echo $(INSTALL_DATA) $$extra $(prefix)/html/lib/textgen/$$extra; \
	  $(INSTALL_DATA) $$extra $(prefix)/html/lib/textgen/$$extra; \
	done

# The MySQL headers cause a lot of warnings

MySQLDictionary.o: MySQLDictionary.cpp
	$(CC) -Wno-deprecated $(INCLUDES) -c -o obj/$@ $<

mysqldump:
	mysqldump -h base -u textgen --password=w1w2w3 textgen > sql/textgen.sql

# -include Dependencies
# DO NOT DELETE THIS LINE -- make depend depends on it.

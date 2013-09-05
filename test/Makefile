PROG = $(patsubst %.cpp,%,$(wildcard *.cpp))
PHPPROG = $(wildcard *.php)
# OBS = $(wildcard ../obj/*.o)

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Weffc++ -Wunreachable-code -Wredundant-decls

processor := $(shell uname -p)
UNAME_S := $(shell uname -s)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(processor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS)

# Getting 'regression/tframe.h' on OS X
#
ifeq ($(UNAME_S), Darwin)
  INCLUDES = \
    -I ../../newbase/test \
    -I /sw/include \
    -I /sw/include/mysql \
    -I ..
  LIBS = \
    -L.. -lsmartmet_textgen \
	-L../../newbase -lsmartmet_newbase \
    -L/sw/lib -lmysqlclient -lmysqlpp
else
  INCLUDES = \
    -I /usr/include/smartmet \
	-I /usr/include/mysql
  LIBS = \
    -L .. -lsmartmet_textgen \
	-lsmartmet_newbase \
    -L $(libdir)/mysql -lmysqlclient -lmysqlpp
endif

INCLUDES += -I ../include

LIBS += \
	-lsmartmet_macgyver \
	-lboost_date_time-mt \
        -lboost_regex-mt\
        -lboost_filesystem-mt\
        -lboost_iostreams-mt\
	-lboost_locale-mt \
	-lboost_thread-mt \
        -lboost_system-mt\
	`pkg-config --cflags --libs icu-i18n` \
        -lz -lbz2 -lpthread

all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@for prog in $(PROG); do \
	 ./$$prog; \
	done
	@echo
	@for prog in $(PHPPROG); do \
	 php -q $$prog; \
	done

$(PROG) : % : %.cpp ../libsmartmet_textgen.a
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)


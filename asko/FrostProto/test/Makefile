PROG = $(patsubst %.cpp,%,$(wildcard *.cpp))
PHPPROG = $(wildcard *.php)
# OBS = $(wildcard ../obj/*.o)

# Path up to 'textgen' main level
#
TEXTGEN = ../../..

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
    -I $(TEXTGEN)/../newbase/test \
    -I /sw/include \
    -I /sw/include/mysql \
    -I ..
  LIBS = \
    -L$(TEXTGEN) -lsmartmet_textgen \
	-L$(TEXTGEN)/../newbase -lsmartmet_newbase \
    -L/sw/lib -lmysqlclient
else
  INCLUDES = \
    -I /usr/include/smartmet \
	-I /usr/include/mysql
  LIBS = \
    -L $(TEXTGEN) -lsmartmet_textgen \
	-lsmartmet_newbase \
    -L $(libdir)/mysql -lmysqlclient
endif

INCLUDES += -I $(TEXTGEN)/include -I ..

LIBS += \
        -lboost_regex-mt \
        -lboost_filesystem-mt \
        -lboost_iostreams-mt \
        -lboost_system-mt \
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

$(PROG) : % : %.cpp $(TEXTGEN)/libsmartmet_textgen.a
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)


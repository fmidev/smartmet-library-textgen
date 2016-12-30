#
# Each cpp compiles to a test exe
#
PROG = $(patsubst %.cpp,%,$(wildcard *.cpp))

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

UNAME_S := $(shell uname -s)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(shell uname -p), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

INCLUDES = \
	-I /usr/include/mysql \
	-I .. \
    -I $(TEXTGEN)/include

LIBS = \
    ../obj/*.o \
    -L$(TEXTGEN) -lsmartmet_textgen \
    -lsmartmet_newbase \
    -L $(libdir)/mysql -lmysqlclient \
    -lboost_regex-mt \
    -lboost_filesystem-mt \
    -lboost_iostreams-mt \
    -lboost_system-mt \
    -lz -lbz2 -lpthread

#---
all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@for prog in $(PROG); do \
	 ./$$prog; \
	done
	@echo

$(PROG): % : %.cpp ../obj/*.o ../*.h ../Makefile
	g++ -DUNIX -O0 -g $(MAINFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)

.PHONY: test echo all clean

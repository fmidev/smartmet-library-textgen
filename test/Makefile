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

INCLUDES = -I ../include \
	-I /usr/include/smartmet \
	-I /usr/include/mysql

LIBS = -L .. -lsmartmet_textgen \
	-lsmartmet_newbase \
        -lboost_regex-mt \
        -lboost_filesystem-mt \
        -lboost_iostreams-mt \
	-L $(libdir)/mysql -lmysqlclient -lz

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


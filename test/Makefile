PROG = $(patsubst %Test.cpp,%,$(wildcard *.cpp))
# OBS = $(wildcard ../obj/*.o)

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Weffc++ -Wunreachable-code -Wredundant-decls

CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS)
INCLUDES = -I ../include \
	-I ../../../../../include/regression \
	-I /usr/include/mysql
LIBS = -L .. -ltextgen -lmysqlclient

all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@for prog in $(PROG); do \
	./$$prog; \
	done

$(PROG) : % : %Test.cpp ../libtextgen.a
	$(CC) $(CFLAGS) -o $@ $@Test.cpp $(INCLUDES) $(LIBS)


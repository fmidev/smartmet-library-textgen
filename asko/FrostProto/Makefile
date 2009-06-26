
OBJ = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

TEXTGEN = ../..

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Weffc++ -Wunreachable-code -Wredundant-decls

#CC = g++
#CFLAGS = -DUNIX -O0 -g $(MAINFLAGS)

#PREFIX=/usr
#ifeq ($(shell uname -p), x86_64)
#  libdir = $(PREFIX)/lib64
#else
#  libdir = $(PREFIX)/lib
#endif

INCLUDES = \
    -I /usr/include/smartmet \
	-I /usr/include/mysql \
	-I $(TEXTGEN)/include

#LIBS = \
#	-lsmartmet_newbase \
#    -L $(libdir)/mysql -lmysqlclient

#CPPFLAGS += $(INCLUDES) $(LIBS)

all: $(OBJ)

test: $(OBJ)
	cd test && make test

obj/%.o: %.cpp
	g++ -c -DUNIX -O0 -g $(INCLUDES) -o $@ $<

clean:
	-rm obj/*.o

echo:
	echo $(OBJ)


.PROXY: test echo all clean

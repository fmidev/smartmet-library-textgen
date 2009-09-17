
OBJ = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

TEXTGEN = ../..

MAINFLAGS = -Wall -W -Wno-unused-parameter

#EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
#	-Wcast-align -Wwrite-strings -Wconversion -Winline \
#	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
#	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
#	-Wshadow -Woverloaded-virtual

#DIFFICULTFLAGS = -Weffc++ -Wunreachable-code -Wredundant-decls

CFLAGS = -DUNIX -O0 -g $(MAINFLAGS)

INCLUDES = \
    -I /usr/include/smartmet \
	-I /usr/include/mysql \
	-I $(TEXTGEN)/include

#---
all: $(OBJ)

test: $(OBJ)
	cd test && make test

obj/%.o: %.cpp *.h Makefile
	g++ -c $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	-rm obj/*.o

echo:
	echo $(OBJ)


.PHONY: test echo all clean

LIB = textgen

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast

DIFFICULTFLAGS = -Weffc++ -Wredundant-decls -Wshadow -Woverloaded-virtual -Wunreachable-code

CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS) $(EXTRAFLAGS) -Werror
CFLAGS_RELEASE =  -DUNIX -O2 -DNDEBUG $(MAINFLAGS)
LDFLAGS = -s
ARFLAGS = -r
INCLUDES = -I $(includedir)/newbase
LIBS = -L ../../../../lib -lnewbase

# Common library compiling template

include ../../makefiles/makefile.lib


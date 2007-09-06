LIB = textgen

MAINFLAGS = -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wredundant-decls -Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Wunreachable-code -Weffc++
CC = g++
CFLAGS = -DUNIX -O0 -g $(MAINFLAGS) $(EXTRAFLAGS) -Werror
CFLAGS_RELEASE =  -DUNIX -O2 -DNDEBUG $(MAINFLAGS)
LDFLAGS = 
ARFLAGS = -r
INCLUDES = -I $(includedir) -I $(includedir)/smartmet/newbase -I $(includedir)/mysql
LIBS = -L $(libdir) -lsmartmet-newbase -Lmysql -lmysqlclient

# Common library compiling template

include ../../makefiles/makefile.lib

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

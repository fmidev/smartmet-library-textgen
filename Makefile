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
INCLUDES = -I $(includedir) -I $(includedir)/newbase -I /usr/include/mysql
LIBS = -L ../../../../lib -lnewbase -L/usr/lib/mysql -lmysqlclient

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
	mysqldump -h mimir -u textgen --password=w1w2w3 textgen > sql/textgen.sql

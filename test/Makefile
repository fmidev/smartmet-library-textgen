PROG = $(patsubst %.cpp,%,$(wildcard *.cpp))
PHPPROG = $(wildcard *.php)
# OBS = $(wildcard ../obj/*.o)

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

CFLAGS = -DUNIX -O0 -g -Wno-inline $(FLAGS)

INCLUDES += \
	-I../textgen \
	-isystem /usr/include/mysql

LIBS += \
	../libsmartmet-textgen.so \
	-lsmartmet-calculator \
	-lsmartmet-newbase \
	-lboost_locale \
	-L$(libdir)/mysql -lmysqlclient -lmysqlpp

all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@for prog in $(PROG); do \
	 ./$$prog; \
	done

$(PROG) : % : %.cpp ../libsmartmet-textgen.so
	$(CXX) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)


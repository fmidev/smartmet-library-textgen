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
	-lsmartmet-macgyver \
	-lboost_locale \
	-L$(libdir)/mysql -lmysqlclient -lmysqlpp

all: $(PROG)
clean:
	rm -f $(PROG) *~
	rm -rf .obj

test: $(PROG)
	echo Running tests:
	total_errs=0; for prog in $(PROG); do errs=0; ./$$prog || errs=$$?; total_errs=$$(($$total_errs + $$errs)); done; echo Total error count: $$total_errs; exit $$total_errs

$(PROG) : % : .obj/%.o
	$(CXX) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)

.obj/%.o: %.cpp
	@mkdir -p .obj
	$(CXX) $(CFLAGS) $(INCLUDES) -c -MD -MF $(patsubst .obj/%.o, .obj/%.d, $@) -MT $@ -o $@ $<

ifneq ($(wildcard .obj/*.d),)
-include $(wildcard .obj/*.d)
endif

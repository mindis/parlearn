-include config.mk

DEBUG ?= 0

# 0 = libc malloc
# 1 = jemalloc
# 2 = tcmalloc
USE_MALLOC_MODE ?= 1

PWD := $(shell pwd)

ifeq ($(strip $(DEBUG)),1)
        CXXFLAGS := -MD -I$(PWD) -Wall -g -fno-omit-frame-pointer --std=c++0x
else
        CXXFLAGS := -MD -DNDEBUG -I$(PWD) -Wall -Werror -O3 -funroll-loops -ffast-math -fno-omit-frame-pointer --std=c++0x
        #CXXFLAGS := -MD -I$(PWD) -Wall -Werror -O2 -fno-omit-frame-pointer --std=c++0x
endif

LDFLAGS := -lpthread -ltbb
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS+=-lrt -lnuma
endif

ifeq ($(strip $(USE_MALLOC_MODE)),1)
        CXXFLAGS+=-DUSE_JEMALLOC
        LDFLAGS+=-ljemalloc
else
ifeq ($(strip $(USE_MALLOC_MODE)),2)
        CXXFLAGS+=-DUSE_TCMALLOC
        LDFLAGS+=-ltcmalloc
endif
endif

SRCFILES := dataset.cc
OBJFILES = $(SRCFILES:.cc=.o)

PROGS := tlearn converters/convert tools/featurehist

all: $(PROGS)

%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PROGS): %: %.o $(OBJFILES)
	$(CXX) $< -o $@ $(OBJFILES) $(LDFLAGS)

DEPFILES := $(wildcard *.d converters/*.d tools/*.d)
ifneq ($(DEPFILES),)
-include $(DEPFILES)
endif

.PHONY: clean
clean:
	rm -f *.o *.d {converters,tools}/*.o {converters,tools}/*.d $(PROGS)

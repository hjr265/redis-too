uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S),Linux)
	SHOBJ_CFLAGS ?=  -fno-common -g -ggdb
	SHOBJ_LDFLAGS ?= -shared -Bsymbolic
else
	SHOBJ_CFLAGS ?= -dynamic -fno-common -g -ggdb
	SHOBJ_LDFLAGS ?= -bundle -undefined dynamic_lookup
endif
CFLAGS = -Wall -g -fPIC -lc -lm -std=gnu99  
CC=gcc

all: too.so

too.so: too.o
	$(LD) -o $@ too.o $(SHOBJ_LDFLAGS) $(LIBS) -lc 

clean:
	rm -rf *.so *.o

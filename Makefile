
.SILENT:

#### uncomment if compiling with local copy of the adflib
#
## directory contain the adflib.a file
#ADFLIBOBJS=../ADFlib-0.10.7/src/.libs
##directory containing the adflib.h file
#ADFLIBINCS=../ADFlib-0.10.7/src/
#
#LIBOBJS=$(ADFLIBOBJS)/libadf.a
#CFLAGS+=-I$(ADFLIBINCS)
#
####

LIBS=-ladf

SOURCES=error.c misc.c version.c zfile.c
OBJS=$(SOURCES:.c=.o)
PROGS= adfcopy adfcreate adfdelete adfdump adfextract adfinfo adfinstall adflist adfmakedir

CC= gcc
CFLAGS+=-O2 -W -Wall -Wextra
#CFLAGS+=-g -ggdb

all: $(PROGS)

%.o:%.c
	echo $@
	$(CC) -c $(CFLAGS) -o $@ $<

adfcopy: $(OBJS) adfcopy.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJS) $(LIBOBJS) $(LIBS)

adfcreate: $(OBJS) adfcreate.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJS) $(LIBOBJS) $(LIBS)

adfdelete: $(OBJS) adfdelete.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJS) $(LIBOBJS) $(LIBS)

adfdump: $(OBJS) adfdump.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJS) $(LIBOBJS) $(LIBS)

adfextract: $(OBJS) adfextract.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJS) $(LIBOBJS) $(LIBS)

adfinfo: $(OBJS) adfinfo.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJS) $(LIBOBJS) $(LIBS)

bootblocks: bootblocks.c
	echo $@
	$(CC) $(CFLAGS) -c -o $@.o $@.c

adfinstall: $(OBJS) adfinstall.c bootblocks.o
	echo $@
	$(CC) $(CFLAGS) -o $@ $(OBJS) bootblocks.o $@.c $(LIBOBJS) $(LIBS)

adflist: $(OBJS) adflist.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $(OBJS) $@.c $(LIBOBJS) $(LIBS)

adfmakedir: $(OBJS) adfmakedir.c
	echo $@
	$(CC) $(CFLAGS) -o $@ $(OBJS) $@.c $(LIBOBJS) $(LIBS)

clean:
	$(RM) -f $(PROGS) *.o *~ core *.bb

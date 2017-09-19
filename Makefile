# IGL makefile
#
# usage: IRIX:      make MIPSABI={32|n32} LIBDIR={lib|lib32} install
#        x86 linux: make ABI=-m32 install

INCDIR=include
SRCDIR=src
EXAMPLEDIR=examples

LIBDIR?=lib
MIPSABI=32
ABI?=-mabi=$(MIPSABI)

INSTALLDIR?=/usr/local

# gcc
GCCPREFIX?=$(BASE)/igcc/usr/tgcware/gcc45/bin/
CC=$(GCCPREFIX)gcc $(ABI)
LD=$(GCCPREFIX)gcc $(ABI)
CFLAGS+=-DDEBUG		# debug output
CFLAGS+=-g3 -Wall	# debug version
CFLAGS+=-O3		# release version
LDFLAGS=-shared

# MIPSpro 7.4 on IRIX
#IDOPREFIX?=$(BASE)/iido/usr/bin/
#CC=$(IDOPREFIX)cc -$(MIPSABI)
#LD=$(IDOPREFIX)ld -$(MIPSABI)
#CFLAGS+=-Dinline=	# unsupported
#CFLAGS+=-g3		# debug version
#CFLAGS+=-O2		# release version
#LDFLAGS=-shared

LN=ln
AR=ar
RANLIB=ranlib

INCLUDES=-I$(INCDIR)
LIBS=-lGL -lGLU -lX11 -lXext -lm -lc

LIBGL_INIT=-Wl,-init,_igl_init

LIBGL_OBJS=$(LIBDIR)/buffer.o     \
     $(LIBDIR)/color.o            \
     $(LIBDIR)/device.o           \
     $(LIBDIR)/draw.o             \
     $(LIBDIR)/grpos.o            \
     $(LIBDIR)/igl.o              \
     $(LIBDIR)/light.o            \
     $(LIBDIR)/matrix.o           \
     $(LIBDIR)/menu.o             \
     $(LIBDIR)/object.o           \
     $(LIBDIR)/objapi.o           \
     $(LIBDIR)/pixel.o            \
     $(LIBDIR)/pixmode.o          \
     $(LIBDIR)/texture.o          \
     $(LIBDIR)/vertex.o           \
     $(LIBDIR)/window.o           \
     $(LIBDIR)/fortran.o
LIBFM_OBJS = $(LIBDIR)/fontmgr.o
LIBFGL_OBJS = $(LIBDIR)/fgl.o

LIB_OBJS = $(LIBGL_OBJS) $(LIBFM_OBJS) $(LIBFGL_OBJS)

EXAMPLE_OBJS=$(EXAMPLEDIR)/demo1.o    \
             $(EXAMPLEDIR)/demo2.o    \
             $(EXAMPLEDIR)/demo3.o    \
             $(EXAMPLEDIR)/demo4.o    \
             $(EXAMPLEDIR)/demo5.o

#
# targets
#
LIBGL_SHARED=$(LIBDIR)/libgl.so
LIBGL_STATIC=$(LIBDIR)/libgl.a
LIBFM_SHARED=$(LIBDIR)/libfm.so
LIBFM_STATIC=$(LIBDIR)/libfm.a
LIBFGL_STATIC=$(LIBDIR)/libfgl.a
EXAMPLES=$(EXAMPLEDIR)/demo1 $(EXAMPLEDIR)/demo2 $(EXAMPLEDIR)/demo3 $(EXAMPLEDIR)/demo4 $(EXAMPLEDIR)/demo5

STATIC=$(LIBGL_STATIC) $(LIBFM_STATIC) $(LIBFGL_STATIC)
SHARED=$(LIBGL_SHARED) $(LIBFM_SHARED)

igl: $(LIBDIR) $(SHARED) $(STATIC)
shared: $(LIBDIR) $(SHARED)
static: $(LIBDIR) $(STATIC)
examples: $(EXAMPLES)
all: igl examples

install: igl
	mkdir -p $(INSTALLDIR)/$(LIBDIR)
	cp $(SHARED) $(STATIC) $(INSTALLDIR)/$(LIBDIR)
	mkdir -p $(INSTALLDIR)/$(INCDIR)/igl
	cp $(INCDIR)/igl.h $(INCDIR)/ifm.h $(INSTALLDIR)/$(INCDIR)/igl
	cp $(INCDIR)/gl.h $(INCDIR)/device.h $(INCDIR)/fmclient.h $(INSTALLDIR)/$(INCDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(LIBGL_OBJS) $(LIBFM_OBJS) $(LIBFGL_OBJS): $(LIBDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/*.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(LIBGL_SHARED): $(LIBGL_OBJS)
	$(LD) $(LDFLAGS) $(LIBS) $(LIBGL_OBJS) -o $@ $(LIBGL_INIT)

$(LIBGL_STATIC): $(LIBGL_OBJS)
	$(AR) cru $@ $(LIBGL_OBJS)
	$(RANLIB) $@

$(LIBFGL_STATIC): $(LIBFGL_OBJS)
	$(AR) cru $@ $(LIBFGL_OBJS)
	$(RANLIB) $@

$(LIBFM_SHARED): $(LIBFM_OBJS)
	$(LD) $(LDFLAGS) $(LIBS) $(LIBFM_OBJS) -o $@

$(LIBFM_STATIC): $(LIBFM_OBJS)
	$(AR) cru $@ $(LIBFM_OBJS)
	$(RANLIB) $@

$(EXAMPLE_OBJS): $(EXAMPLEDIR)/%.o: $(EXAMPLEDIR)/%.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(EXAMPLES): %: %.o $(SHARED)
	@$(LD) $@.o -o $@ -L$(LIBDIR) -lgl $(LIBS)

clean:
	@rm -f $(LIB_OBJS) $(SHARED) $(STATIC)
	@rm -f $(EXAMPLE_OBJS) $(EXAMPLES)
	@echo DONE


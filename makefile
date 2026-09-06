CC = gcc
CFLAGS = -Wall -g -fPIC -Iinclude
LDFLAGS = -lm
OBJDIR = obj
SRCDIR = src

OBJS = $(OBJDIR)/bitreader.o \
       $(OBJDIR)/codage.o \
       $(OBJDIR)/decodage.o \
       $(OBJDIR)/filtrage.o \
       $(OBJDIR)/pgm_reader.o \
       $(OBJDIR)/quadtree.o

LIBRARY = lib/libqtc.so

all : library codec

library: $(LIBRARY)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJS)
	@mkdir -p lib
	$(CC) -shared -o $@ $(OBJS)
	@echo ">>> Built $@"

CODEC = codec

$(OBJDIR)/codec.o: main/codec.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(CODEC): $(OBJDIR)/codec.o $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $(OBJDIR)/codec.o -L./lib -lqtc $(LDFLAGS) -Wl,-rpath,./lib
	@echo ">>> Built codec"

help :
	@echo "make: make library and codec" 
	@echo "make library : make library" 
	@echo "make codec : make codec" 
	@echo "make help : show this list"
	@echo "make doxygen : make doxyfile"
	@echo "make clean : clean obj library codec doxyfile"

doxygen :
	@doxygen Doxyfile

clean:
	rm -rf obj lib codec html latex

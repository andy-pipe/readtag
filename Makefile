prefix ?= /usr/local
INSTALL_BIN ?= $(prefix)/bin/
INSTALL ?= install

###############################################################
# Begin: Configuration
###############################################################

# compiler and linker flags
CFLAGS += -std=c11 -Wall -pedantic
DEBUGFLAGS = -g -O0 -DDEBUG
LDFLAGS += -lm

# add header files here (or use rule for all *.h files below)
INCLUDES := 
#INCLUDES := $(wildcard *.h)

# add source files here (or use rule for all *.c files below)
SOURCES := $(wildcard *.c)

# add flags to link libs here
LIBS := 

###############################################################
# End: Configuration
###############################################################

OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES))
OBJDIR = release

ifdef DEBUG
	CFLAGS += $(DEBUGFLAGS)
	OBJDIR = debug
endif

all: readtag

readtag: $(OBJS)
	$(CC) $(LDFLAGS) -o readtag $(OBJS) $(LIBS)

$(OBJDIR)/%.o: %.c $(INCLUDES) Makefile | create_obj_dir
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	@rm -rf release debug
	@rm -f readtag

distclean: clean

install: all
	$(INSTALL) --directory $(INSTALL_BIN)
	$(INSTALL) readtag -m755 $(INSTALL_BIN)

install_target: install

uninstall:
	rm -f $(INSTALL_BIN)/readtag

uninstall_target: uninstall

check: FORCE
	@cppcheck --quiet --enable=all .
	@echo "DONE"

create_obj_dir:
	@mkdir -p $(OBJDIR)

FORCE:

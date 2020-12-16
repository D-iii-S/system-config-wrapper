.PHONY:	all lib clean

# Settings

MAKEFLAGS += --no-builtin-rules --no-builtin-variables

CC = g++
CC_OPTS_SO = -fpic -O2 -DNDEBUG -Wall -Wextra -Werror
LD = g++
LD_OPTS_SO = -fpic -shared -ldl

BIN = ../

# Targets

all: lib

lib: $(BIN)/system-config-wrapper.so

clean:
	rm -f *.o
	rm -f *.dep

# Files

MM_SO = system-config-wrapper

OO_SO = $(addsuffix .o, $(MM_SO))

DD_SO = $(addsuffix .dep, $(MM_SO))

DD = $(DD_SO)

# Modules

$(OO_SO): %.o: %.c
	$(CC) $(CC_OPTS_SO) -c -o $@ $<

# Executables

$(BIN)/system-config-wrapper.so: $(OO_SO)
	$(LD) $(LD_OPTS_SO) -o $@ $^

# Dependencies

include $(DD)

%.dep: %.c
	$(CC) -MM -MT "$@ $(basename $@).o" -o $@ $<

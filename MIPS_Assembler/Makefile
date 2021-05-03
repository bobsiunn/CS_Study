CC=gcc
CCFLAGS=

SRCS=mips-sim.c

TARGET=mips-sim

OBJS:=$(patsubst %.c,%.o,$(SRCS))

all:$(TARGET)

%.o:%.c
	$(CC) $(CCFLAGS)$< -c -o $@
$(TARGET):  $(OBJS)
	$(CC) $(CCFLAGS)$^ -o $@
			
.PHONY=clean

clean:
	rm -f $(OBJS) $(TARGET)
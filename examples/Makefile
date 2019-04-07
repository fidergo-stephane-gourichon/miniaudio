# PulseAudio fails on X/Ubuntu 18.04
CFLAGS=-W -Wall -Wextra -g -DMA_NO_PULSEAUDIO
LDFLAGS=-lpthread -lm -ldl

SRCS=$(wildcard *.c)
EXES=$(SRCS:.c=)

all: $(EXES)

debug:
	echo EXES=$(EXES)
	echo SRCS=$(SRCS)

# Default in GNU make 4.1 puts LDFLAGS before $< and fails to compile.
%: %.c
	gcc $(CFLAGS) $< $(LDFLAGS) -o $@

clean:
	rm -f $(EXES)

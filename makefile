CC            = gcc
CFLAGS        = -I.
CFLAGS_DEBUG  = -g -O0 -D DEBUG
CFLAGS_TEST   = -D TEST
LDFLAGS       = -L. -lcurl -liconv
LDFLAGS_DEBUG = -fsanitize=address
OBJ           = auth.o curl_callback.o filter.o io.o jsmn.o main.o pack.o pull.o push.o test.o util.o

all: pocket-tool

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LDFLAGS += $(LDFLAGS_DEBUG)
debug: pocket-tool

test: CFLAGS += $(CFLAGS_TEST)
test: pocket-tool

# $@ = left side of : (target)
# $^ = entire right side of : (dependencies)
# $< = first dependency

pocket-tool: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c main.h
	$(CC) -c $< $(CFLAGS)

main.o: main.c main.h usage.h
	$(CC) -c $< $(CFLAGS)

usage.h: usage.txt
	xxd -i usage.txt usage.h

clean:
	rm -f *.o *.dSYM *.gch pocket-tool

install: pocket-tool
	mv pocket-tool /usr/local/bin/pocket-tool

.PHONY: clean install debug test

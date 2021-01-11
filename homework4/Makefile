CC=gcc
CFLAGS=-g -std=c99 -pedantic -O1
SUBMIT=sim
SUBMITC=$(SUBMIT:=.c)
SUBMITH=$(SUBMIT:=.h)

all: $(SUBMIT) 

%: %.c
	$(CC) $(CFLAGS) -o $@ $@.c

.PHONY: clean package

package: $(SUBMITC) $(SUBMITH)
	tar -cvzf hw4.tgz $(SUBMITC) $(SUBMITH) readme.txt Makefile

clean:
	rm -f $(SUBMIT) hw4.tgz

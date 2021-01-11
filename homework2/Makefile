CC=gcc
CFLAGS=-g -std=c99 -pedantic -O1
SUBMIT=assem
SUBMITC=$(SUBMIT:=.c)
SUBMITH=$(SUBMIT:=.h)

all: $(SUBMIT) 

%: %.c
	$(CC) $(CFLAGS) -o $@ $@.c

.PHONY: clean package

package: $(SUBMITC) $(SUBMITH)
	tar -cvzf hw2.tgz $(SUBMITC) $(SUBMITH) readme.txt Makefile

clean:
	rm -f $(SUBMIT) hw2.tgz

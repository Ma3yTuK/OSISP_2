TAGRET = lab4
PREFIX = /usr/local/bin

.PHONY: all clean install uninstall

$(TAGRET): lab4.o
	gcc -o $(TAGRET) lab4.o

lab4.o : lab4.c
	gcc -c -o lab4.o lab4.c

all: $(TAGRET)

clean:
	rm -f $(TAGRET) *.o

install:
	install $(TAGRET) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(TAGRET)

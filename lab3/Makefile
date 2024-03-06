TAGRET = encryptor
PREFIX = /usr/local/bin

.PHONY: all clean install uninstall

$(TAGRET): des.o main.o
	gcc -o $(TAGRET) main.o des.o

des.o : des.c
	gcc -c -o des.o des.c
main.o : main.c
	gcc -c -o main.o main.c

all: $(TAGRET)

clean:
	rm -f $(TAGRET) *.o

install:
	install $(TAGRET) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(TAGRET)

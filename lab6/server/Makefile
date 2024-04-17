TAGRET = server
PREFIX = /usr/local/bin

.PHONY: all clean install uninstall

$(TAGRET): server.o
	gcc -o $(TAGRET) server.o

server.o : server.c
	gcc -c -o server.o server.c

all: $(TAGRET)

clean:
	rm -f $(TAGRET) *.o

install:
	install $(TAGRET) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(TAGRET)

run : $(TAGRET) 
	./$(TAGRET)
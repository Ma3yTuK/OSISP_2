TAGRET = client
PREFIX = /usr/local/bin

.PHONY: all clean install uninstall

$(TAGRET): client.o
	gcc -o $(TAGRET) client.o

client.o : client.c
	gcc -c -o client.o client.c

all: $(TAGRET)

clean:
	rm -f $(TAGRET) *.o

install:
	install $(TAGRET) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(TAGRET)

run : $(TAGRET) 
	./$(TAGRET)
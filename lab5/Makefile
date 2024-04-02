TAGRET = lab5
PREFIX = /usr/local/bin

.PHONY: all clean install uninstall

$(TAGRET): lab5.o
	gcc -o $(TAGRET) lab5.o

lab5.o : lab5.c
	gcc -c -o lab5.o lab5.c

all: $(TAGRET)

clean:
	rm -f $(TAGRET) *.o

install:
	install $(TAGRET) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(TAGRET)

run : $(TAGRET) 
	./$(TAGRET)
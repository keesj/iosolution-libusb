default:main
LIBS=libusb-1.0
CFLAGS= $(shell /usr/bin/pkg-config --libs --cflags ${LIBS})

iosolution.o:iosolution.c iosolution.h
	$(CC) -Wall -g ${CFLAGS} -c -o $@ iosolution.c 

main:main.c iosolution.o 
	$(CC) -Wall -g ${CFLAGS}  -o $@ main.c   iosolution.o -lusb-1.0
clean:
	rm -rf main iosolution.o *~ main

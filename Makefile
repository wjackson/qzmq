CC=gcc
CC=i686-w64-mingw32-gcc

all: qzmq.so

qzmq.so: qzmq.c
	${CC} -m32 -Wall -DDEBUG -DKXVER=3 -I. -lzmq -shared -o qzmq.so qzmq.c

qzmq.dll:
	${CC} -std=c99 -m32 -Wall -DDEBUG -DKXVER=3 -Wl,--no-undefined -I. -I/usr/local/win32/include -L/usr/local/win32/bin -lzmq -shared -o qzmq.dll qzmq.c

clean:
	rm -f qzmq.so

all: qzmq.so

qzmq.so: qzmq.c
	gcc -DDEBUG -DKXVER=2 -m64 -I. -lzmq -fPIC -shared -o qzmq.so qzmq.c

clean:
	rm -f qzmq.so

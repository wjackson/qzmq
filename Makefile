all: qzmq.so

qzmq.so: qzmq.c
	gcc -m32 -I. -lzmq -fPIC -shared -o qzmq.so qzmq.c

clean:
	rm -f qzmq.so

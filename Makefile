all: qzmq.so pub_test

qzmq.so: qzmq.c
	gcc -m32 -I. -lzmq -fPIC -shared -o qzmq.so qzmq.c

pub_test: pub_test.c
	gcc -lzmq -o pub_test pub_test.c

clean:
	rm -f qzmq.so
	rm -f pub_test

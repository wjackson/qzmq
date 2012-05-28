
all: qzmq.so pub_test hello_client hello_server

qzmq.so: qzmq.c
	gcc -m32 -I. -lzmq -fPIC -shared -o qzmq.so qzmq.c

pub_test: pub_test.c
	gcc -lzmq -o pub_test pub_test.c

hello_client: hello_client.c
	gcc -lzmq -o hello_client hello_client.c

hello_server: hello_server.c
	gcc -lzmq -o hello_server hello_server.c

clean:
	rm -f qzmq.so
	rm -f hello_client
	rm -f hello_server

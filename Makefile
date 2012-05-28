
all: qzmq.so hello_client hello_server

qzmq.so: qzmq.c
	gcc -m32 -I. -lzmq -fPIC -shared -o qzmq.so qzmq.c

hello_client: hello_client.c
	gcc -lzmq -o hello_client hello_client.c

hello_server: hello_server.c
	gcc -lzmq -o hello_server hello_server.c

clean:
	rm -f qzmq.so
	rm -f hello_client
	rm -f hello_server

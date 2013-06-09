CC=gcc

all: qzmq.so

qzmq.so: qzmq.c
	${CC} -Wall -DDEBUG -DKXVER=3 -I. -lzmq -shared -o qzmq.so qzmq.c

#
# windows build stuff
#

# win32
q-w32.a: q.def
	/usr/i686-w64-mingw32/bin/dlltool -d q.def --dllname q.exe --output-lib q-w32.a

qzmq-w32/w32:
	mkdir -p qzmq-w32/w32

qzmq-w32/w32/qzmq.dll: q-w32.a qzmq.c qzmq-w32/w32
	i686-w64-mingw32-gcc -Wall -DKXVER=3 -I. -I/usr/local/win32/include -L/usr/local/win32/lib -shared -o qzmq-w32/w32/qzmq.dll qzmq.c q-w32.a -lzmq

qzmq-w32.zip: qzmq-w32/w32/qzmq.dll zmq.q
	cp zmq.q qzmq-w32/
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll qzmq-w32/w32/
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libstdc++-6.dll qzmq-w32/w32/
	cp /usr/local/win32/bin/libzmq.dll qzmq-w32/w32/
	zip -r qzmq-w32.zip qzmq-w32

# win64
q-w64.a: q.def
	/usr/x86_64-w64-mingw32/bin/dlltool -d q.def --dllname q.exe --output-lib q-w64.a

qzmq-w64/w64:
	mkdir -p qzmq-w64/w64

qzmq-w64/w64/qzmq.dll: q-w64.a qzmq.c qzmq-w64/w64
	x86_64-w64-mingw32-gcc -Wall -DKXVER=3 -I. -I/usr/local/win64/include -L/usr/local/win64/lib -shared -o qzmq-w64/w64/qzmq.dll qzmq.c q-w64.a -lzmq

qzmq-w64.zip: qzmq-w64/w64/qzmq.dll zmq.q
	cp zmq.q qzmq-w64/
	cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll qzmq-w64/w64/
	cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libstdc++-6.dll qzmq-w64/w64/
	cp /usr/local/win64/bin/libzmq.dll qzmq-w64/w64/
	zip -r qzmq-w64.zip qzmq-w64

windows: qzmq-w32.zip qzmq-w64.zip

clean:
	rm -f  qzmq.so
	rm -f  q-w32.a
	rm -rf qzmq-w32
	rm -f  qzmq-w32.zip
	rm -f  q-w64.a
	rm -rf qzmq-w64
	rm -f  qzmq-w64.zip

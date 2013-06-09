# qzmq

ZeroMQ bindings for Q

## Installing on Linux

```
$ git clone https://github.com/wjackson/qzmq.git
$ make
$ cp qzmq.so $QHOME/<arch folder>/
$ cp zmq.q $QHOME
```

Note: If you're using the demo version of q on a 64-bit platform then you'll
want to edit the Makefile and add -m32 as an argument to the compiler.

## Installing on Windows

Download the appropriate binary:

* [qzmq-w32.zip](http://qzmq.s3-website-us-east-1.amazonaws.com/qzmq-w32.zip)
* [qzmq-w64.zip](http://qzmq.s3-website-us-east-1.amazonaws.com/qzmq-w64.zip)

Put zmq.q in %QHOME% and the DLLs in %QHOME%/w[32|64].

## Usage

```
$ q
q).z.k >= 2013.04.04 / is q new enough?
1b

$ q
q)\l zmq.q
q)c: .zmq.ctx_new[]
q)s: .zmq.socket[c;.zmq.ZMQ_PUSH]
q).zmq.connect[s;"tcp://127.0.0.1:5555"]
q).z.ts: {[] .zmq.send[s;"x: .z.t"]}
q)\t 2000

$ q
q)\l zmq.q
q)c:.zmq.ctx_new[]
q)s:.zmq.socket[c;.zmq.ZMQ_PULL]
q).zmq.bind[s;"tcp://127.0.0.1:5555"]
q).zmq.ps: {[m]: show last m }
"x: .z.t"
"x: .z.t"
"x: .z.t"
q).zmq.ps: {[m]: value last m }
q)x
18:41:21.329
q)x
18:41:23.329
```

## Q Version Requirement

Qzmq requires kdb+ version 3.0 2013.04.04 or later. This is because it relies
on the c function sd0x (added on 2013.04.04) to implement .zmq.close. Qzmq may
compile and load with earlier versions but q will be probably crash when
sockets are closed.

## ZeroMQ Version Requirement

Qzmq requires ZeroMQ 3.x. ZeroMQ >= 3.2.2 is supposedly compatible on the wire
with 2.x so it should be possible to interoperate with non-q components that
use an earlier version of the library.

## Comparison with Q IPC

### Synchronous/Asynchronous

Q IPC supports both synchronous and asynchronous communication. Qzmq only
supports asynchronous communication.

### Message Formats and Serialization

Q IPC supports two message formats:

* k string containing a q expression to be executed on the server.
* k list of the form (function; arg1; arg2; ...) where the function is to be
  applied with the given arguments.

The k object holding the message is serialized and placed on the wire.

Qzmq currently only supports strings.  Unlike q IPC, qzmq
serializes/deserializes messages as simple byte strings. This has the
advantage that it's easy to connect to other non-q components. For instance, a
SUB socket can be trivially hooked up to a remote PUB socket publishing JSON
messages. The publisher doesn't have to know that it's talking to q and it
doesn't need to serialize messages as k objects.

The other advantage to this approach is that it's relatively easy to support
multipart messages. If and when we support serialized k objects it will
probably be necessary to make assumptions about how message parts are encoded
based on their position in the message.

The disadvantage to this approach is that sending data structures from one q
instance to another via qzmq is a pain. You have to serialize to a string
first (-3!). It's probably inefficient too. So it's likely that a mechanism
for sending and receiving k objects will be added fairly soon.

### Callbacks

In q IPC, if we define .z.ps then arriving messages are sent to this callback.
Similarly, qzmq provides .zmq.ps. When it's defined, any arriving messages are
passed in.

Qzmq's .zmq.ps callback receives one parameter: a mixed list containing one
element for every message part in the received message. Each elements in the
mixed list is a char vector. Typically, the last element will contain the
payload of the message.

Without callbacks defined, q IPC evaluates incoming messages as q commands.
Qzmq, on the other hand, just drops messages if a callback isn't defined. If
you want the q IPC behavior define your callback like so:

```
.zmq.ps: { [m] value last m }
```

### Special Variables

Q maintains the following variables for tracking information about IPC
handles:

* .z.w: handle
* .z.W: handles

Similarly, qzmq has:

* .zmq.w: socket
* .zmq.W: sockets  (not yet implemented)
* .zmq.c: context  (not yet implemented)
* .zmq.C: contexts (not yet implemented)

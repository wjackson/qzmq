# qzmq

ZeroMQ bindings for Q

## Installing qzmq

```
$ git clone https://github.com/wjackson/qzmq.git
$ make
$ cp qzmq.so $QHOME/<arch folder>/
$ cp zmq.q $QHOME
```

Note: Edit the Makefile to configure whether you want a 32 or 64 bit binary.

## usage

```
$ q
q)\l zmq.q
q)ctx: .zmq.ctx_new[]
q)s:.zmq.socket[c;.zmq.ZMQ_PUB]
q).zmq.connect[s;"tcp://localhost:5555"]
q).z.ts: {[] .zmq.send[s;"x: .z.ps"]}
q)\t 2000

$ q
q)\l zmq.q
q)c:.zmq.ctx_new[]
q)s:.zmq.socket[c;.zmq.ZMQ_SUB]
q).zmq.setsockopt[s;.zmq.ZMQ_SUBSCRIBE;""]
q).zmq.bind[s;"tcp://lo:5555"]
q)x
2013.04.07D07:48:29.990440000
q).zmq.ps: {[x]: show x}
"x: .z.p"
"x: .z.p"
"x: .z.p"
```

## Q Version Requirement

qzmq requires kdb+ version 3.0 2013.04.05 or later. This is because we rely on
the k function sd0x (added on 2013.04.05) to implement zmq.close. qzmq may
compile and load with earlier versions of q but there will be probably crash
when closing sockets.

## ZeroMQ Version Requirement

qzmq requires ZeroMQ 3.x.

\l zmq.q

c: .zmq.ctx_new[]

push: .zmq.socket[c;.zmq.ZMQ_PUSH]
pull: .zmq.socket[c;.zmq.ZMQ_PULL]

.zmq.bind[push;"ipc:///tmp/qzmq.test"]
.zmq.connect[pull;"ipc:///tmp/qzmq.test"]

stop: { []
    .zmq.close[push];
    .zmq.close[pull];
    .zmq.ctx_destroy[c];
    value "\\\\";
 }

.zmq.ps: {
    $[.zmq.w = pull; show `pass; show `fail];
 }

$[.zmq.w = 0j; show `pass; show `fail];

.zmq.send[push;"test"]

.z.ts: { []
    .z.ts: { []
        $[.zmq.w = 0j; show `pass; show `fail];
        stop[];
     }
 }
\t 100

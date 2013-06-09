\l zmq.q

c: .zmq.ctx_new[]

push: .zmq.socket[c;.zmq.ZMQ_PUSH]
pull: .zmq.socket[c;.zmq.ZMQ_PULL]

.zmq.bind[push;"tcp://127.0.0.1:5555"]
.zmq.connect[pull;"tcp://127.0.0.1:5555"]

stop: { []
    .zmq.close[push];
    .zmq.close[pull];
    .zmq.ctx_destroy[c];
    value "\\\\";
 }

.zmq.ps: { [envelope]
    msg: last envelope;
    $[msg ~ "test"; show `pass; show `fail];
    stop[];
 }

.zmq.send[push;"test"]

.z.ts: { []
    .z.ts: { []
        show `timeout;
        stop[];
     }
 }
\t 100

\l zmq.q

c: .zmq.ctx_new[]

pub: .zmq.socket[c;.zmq.ZMQ_PUB]
sub: .zmq.socket[c;.zmq.ZMQ_SUB]

.zmq.bind[pub;"ipc:///tmp/qzmq.test"]
.zmq.connect[sub;"ipc:///tmp/qzmq.test"]
.zmq.setsockopt[sub;.zmq.ZMQ_SUBSCRIBE;""]

stop: { []
    .zmq.close[pub];
    .zmq.close[sub];
    .zmq.ctx_destroy[c];
    value "\\\\";
 }

.zmq.ps: { [envelope]
    msg: last envelope;
    $[msg ~ "test"; show `pass; show `fail];
    stop[];
 }

.z.ts: { [] / time 0

    .z.ts: { [] / time 100
        .zmq.send[pub;"test"]

        .z.ts: { /time 200
            show `timeout;
            stop[];
         }
     }
 }
\t 100
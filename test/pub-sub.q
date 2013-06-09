\l zmq.q

c: .zmq.ctx_new[]

pub: .zmq.socket[c;.zmq.ZMQ_PUB]
sub: .zmq.socket[c;.zmq.ZMQ_SUB]

.zmq.bind[pub;"tcp://127.0.0.1:5555"]
.zmq.connect[sub;"tcp://127.0.0.1:5555"]
.zmq.setsockopt[sub;.zmq.ZMQ_SUBSCRIBE;""]

stop: { []
    .zmq.unbind[pub;"tcp://127.0.0.1:5555"]
    .zmq.disconnect[sub;"tcp://127.0.0.1:5555"];
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

\l zmq.q

c: .zmq.ctx_new[]

req: .zmq.socket[c;.zmq.ZMQ_REQ]
rep: .zmq.socket[c;.zmq.ZMQ_REP]

.zmq.bind[req;"ipc:///tmp/qzmq.test"]
.zmq.connect[rep;"ipc:///tmp/qzmq.test"]

stop: { []
    .zmq.close[req];
    .zmq.close[rep];
    .zmq.ctx_destroy[c];
    value "\\\\";
 }

.zmq.ps: { [envelope]
    msg: last envelope;

    / received ping request
    if [.zmq.w = rep;
        .zmq.send[.zmq.w; "pong"];
    ]

    / received ping reply
    if [.zmq.w = req;
        reply: msg;
        $[reply ~ "pong"; show `pass; show `fail];
        stop[];
    ]
 }

.zmq.send[req;"ping"]

.z.ts: { []
    .z.ts: { []
        show `timeout;
        stop[];
     }
 }
\t 100

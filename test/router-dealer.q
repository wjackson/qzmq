\l zmq.q

c: .zmq.ctx_new[]

dealer: .zmq.socket[c;.zmq.ZMQ_DEALER]
router: .zmq.socket[c;.zmq.ZMQ_ROUTER]

.zmq.setsockopt[router;.zmq.ZMQ_ROUTER_MANDATORY;1]

.zmq.connect[dealer;"ipc:///tmp/qzmq.test"]
.zmq.bind[router;"ipc:///tmp/qzmq.test"]

stop: { []
    .zmq.close[dealer];
    .zmq.close[router];
    .zmq.ctx_destroy[c];
    value "\\\\";
 }

.zmq.ps: { [envelope]

    / received ping request
    if [.zmq.w = router;
        addr: -1 _ envelope;
        msg: last envelope;
        .zmq.send_multipart[.zmq.w;addr,enlist("pong")];
    ]

    / received ping reply
    if [.zmq.w = dealer;
        reply: last envelope;
        $[reply ~ "pong"; show `pass; show `fail];
        stop[];
    ]
 }

.z.ts: { []
    .z.ts: { []
        .zmq.send[dealer;"ping"]

        .z.ts: { []
            show `timeout;
            stop[];
         }
     }
 }
\t 100

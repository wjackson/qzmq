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

.zmq.ps: { [envelope]
    $[ envelope ~ ("aaa";"bbb"); show `pass; show `fail];

    stop[];
 }

.zmq.send_multipart[push; ("aaa";"bbb")]

.z.ts: { []
    .z.ts: { []
        show `timeout;
        stop[];
     }
 }
\t 100

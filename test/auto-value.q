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

/evaluate incoming messages
.zmq.ps: { [m] value last m }

x: 0
.zmq.send[push;"x:7"]

/send is async so we gotta wait before asserting
.z.ts: { []
    .z.ts: { []
        $[x=7; show `pass; show `fail];
        value "\\t 0";
        value "\\\\"; / exit the test
     }
 }
\t 100

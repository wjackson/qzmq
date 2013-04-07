\l zmq.q

c:.zmq.init[1]
s:.zmq.socket[c;.zmq.ZMQ_PUB]
/.zmq.setsockopt[s;.zmq.ZMQ_LINGER; 3000]

.zmq.connect[s;"tcp://localhost:5555"]

.zmq.send[s; "x: .z.p"]

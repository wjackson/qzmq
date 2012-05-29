\l zmq.q

c:.zmq.init[1]
s:.zmq.socket[c;.zmq.ZMQ_SUB]

.zmq.setsockopt[s;.zmq.ZMQ_SUBSCRIBE;`]
/.zmq.setsockopt[s;.zmq.ZMQ_LINGER;1000]
/.zmq.setsockopt[s;.zmq.ZMQ_HWM;1000]
.zmq.bind[s;`$"tcp://*:5555"]

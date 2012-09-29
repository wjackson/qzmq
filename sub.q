\l zmq.q

c:.zmq.init[1i]
s:.zmq.socket[c;.zmq.ZMQ_SUB]

/ filter in everything
.zmq.setsockopt[s;.zmq.ZMQ_SUBSCRIBE;""]

.zmq.bind[s;"tcp://*:5555"]

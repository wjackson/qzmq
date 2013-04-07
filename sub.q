\l zmq.q

c:.zmq.ctx_new[]
s:.zmq.socket[c;.zmq.ZMQ_SUB]

/ filter in everything
.zmq.setsockopt[s;.zmq.ZMQ_SUBSCRIBE;""]

.zmq.bind[s;"tcp://lo:5555"]

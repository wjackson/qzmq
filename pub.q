\l zmq.q

c:.zmq.init[1i]
s:.zmq.socket[c;.zmq.ZMQ_PUB]

.zmq.connect[s;"tcp://localhost:5555"]

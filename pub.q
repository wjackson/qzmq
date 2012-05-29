\l zmq.q

c:.zmq.init[1]
s:.zmq.socket[c;.zmq.ZMQ_PUB]

.zmq.connect[s;`$"tcp://localhost:5555"]

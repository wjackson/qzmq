\l zmq.q

c: .zmq.ctx_new[]

.zmq.ctx_set[c;.zmq.ZMQ_IO_THREADS;3]
threads: .zmq.ctx_get[c;.zmq.ZMQ_IO_THREADS]

$[threads=3; show `pass; show `fail];

.zmq.ctx_destroy[c]
\\

\d .zmq

/
Constants
\

\ Socket Types
ZMQ_PAIR:              0i
ZMQ_PUB:               1i
ZMQ_SUB:               2i
ZMQ_REQ:               3i
ZMQ_REP:               4i
ZMQ_DEALER:            5i
ZMQ_ROUTER:            6i
ZMQ_PULL:              7i
ZMQ_PUSH:              8i
ZMQ_XPUB:              9i
ZMQ_XSUB:              10i
ZMQ_XREQ:              ZMQ_DEALER
ZMQ_XREP:              ZMQ_ROUTER
ZMQ_UPSTREAM:          ZMQ_PULL
ZMQ_DOWNSTREAM:        ZMQ_PUSH

\ Socket options.
ZMQ_HWM:               1i
ZMQ_SWAP:              3i
ZMQ_AFFINITY:          4i
ZMQ_IDENTITY:          5i
ZMQ_SUBSCRIBE:         6i
ZMQ_UNSUBSCRIBE:       7i
ZMQ_RATE:              8i
ZMQ_RECOVERY_IVL:      9i
ZMQ_MCAST_LOOP:        10i
ZMQ_SNDBUF:            11i
ZMQ_RCVBUF:            12i
ZMQ_RCVMORE:           13i
ZMQ_FD:                14i
ZMQ_EVENTS:            15i
ZMQ_TYPE:              16i
ZMQ_LINGER:            17i
ZMQ_RECONNECT_IVL:     18i
ZMQ_BACKLOG:           19i
ZMQ_RECOVERY_IVL_MSEC: 20i
ZMQ_RECONNECT_IVL_MAX: 21i

/
Functions
\

bind:        `qzmq 2:(`q_bind;2)
close:       `qzmq 2:(`q_close;1)
connect:     `qzmq 2:(`q_connect;2)
/device:     `qzmq 2:(`q_device;2)
/getsockopt: `qzmq 2:(`q_getsockopt;2)
init:        `qzmq 2:(`q_init;1)
send:        `qzmq 2:(`q_send;2)
setsockopt:  `qzmq 2:(`q_setsockopt;3)
socket:      `qzmq 2:(`q_socket;2)
term:        `qzmq 2:(`q_term;1)
version:     `qzmq 2:(`q_version;1)

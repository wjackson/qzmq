\d .zmq

/
Constants
\

\ Socket Types
ZMQ_PAIR:              0
ZMQ_PUB:               1
ZMQ_SUB:               2
ZMQ_REQ:               3
ZMQ_REP:               4
ZMQ_DEALER:            5
ZMQ_ROUTER:            6
ZMQ_PULL:              7
ZMQ_PUSH:              8
ZMQ_XPUB:              9
ZMQ_XSUB:              10
ZMQ_XREQ:              ZMQ_DEALER
ZMQ_XREP:              ZMQ_ROUTER
ZMQ_UPSTREAM:          ZMQ_PULL
ZMQ_DOWNSTREAM:        ZMQ_PUSH

\ Socket options.
ZMQ_HWM:               1
ZMQ_SWAP:              3
ZMQ_AFFINITY:          4
ZMQ_IDENTITY:          5
ZMQ_SUBSCRIBE:         6
ZMQ_UNSUBSCRIBE:       7
ZMQ_RATE:              8
ZMQ_RECOVERY_IVL:      9
ZMQ_MCAST_LOOP:        10
ZMQ_SNDBUF:            11
ZMQ_RCVBUF:            12
ZMQ_RCVMORE:           13
ZMQ_FD:                14
ZMQ_EVENTS:            15
ZMQ_TYPE:              16
ZMQ_LINGER:            17
ZMQ_RECONNECT_IVL:     18
ZMQ_BACKLOG:           19
ZMQ_RECOVERY_IVL_MSEC: 20
ZMQ_RECONNECT_IVL_MAX: 21

/
Functions
\

init:       `qzmq 2:(`q_init;1)
socket:     `qzmq 2:(`q_socket;2)
setsockopt: `qzmq 2:(`q_setsockopt;3)
bind:       `qzmq 2:(`q_bind;2)
connect:    `qzmq 2:(`q_connect;2)
send:       `qzmq 2:(`q_send;2)

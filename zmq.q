\d .zmq

/ Constants

/ Socket Types
ZMQ_PAIR:    0i
ZMQ_PUB:     1i
ZMQ_SUB:     2i
ZMQ_REQ:     3i
ZMQ_REP:     4i
ZMQ_DEALER:  5i
ZMQ_ROUTER:  6i
ZMQ_PULL:    7i
ZMQ_PUSH:    8i
ZMQ_XPUB:    9i
ZMQ_XSUB:   10i
ZMQ_XREQ:   ZMQ_DEALER
ZMQ_XREP:   ZMQ_ROUTER

/ Socket Options
ZMQ_AFFINITY:                 4i
ZMQ_IDENTITY:                 5i
ZMQ_SUBSCRIBE:                6i
ZMQ_UNSUBSCRIBE:              7i
ZMQ_RATE:                     8i
ZMQ_RECOVERY_IVL:             9i
ZMQ_SNDBUF:                  11i
ZMQ_RCVBUF:                  12i
ZMQ_RCVMORE:                 13i
ZMQ_FD:                      14i
ZMQ_EVENTS:                  15i
ZMQ_TYPE:                    16i
ZMQ_LINGER:                  17i
ZMQ_RECONNECT_IVL:           18i
ZMQ_BACKLOG:                 19i
ZMQ_RECONNECT_IVL_MAX:       21i
ZMQ_MAXMSGSIZE:              22i
ZMQ_SNDHWM:                  23i
ZMQ_RCVHWM:                  24i
ZMQ_MULTICAST_HOPS:          25i
ZMQ_RCVTIMEO:                27i
ZMQ_SNDTIMEO:                28i
ZMQ_IPV4ONLY:                31i
ZMQ_LAST_ENDPOINT:           32i
ZMQ_ROUTER_MANDATORY:        33i
ZMQ_TCP_KEEPALIVE:           34i
ZMQ_TCP_KEEPALIVE_CNT:       35i
ZMQ_TCP_KEEPALIVE_IDLE:      36i
ZMQ_TCP_KEEPALIVE_INTVL:     37i
ZMQ_TCP_ACCEPT_FILTER:       38i
ZMQ_DELAY_ATTACH_ON_CONNECT: 39i
ZMQ_XPUB_VERBOSE:            40i

/ Message Options
ZMQ_MORE: 1i

/ send/recv Options
ZMQ_DONTWAIT: 1i
ZMQ_SNDMORE:  2i

/ Context Options
ZMQ_IO_THREADS:  1i
ZMQ_MAX_SOCKETS: 2i

/ Functions

bind:           `qzmq 2:(`q_bind;2)
close:          `qzmq 2:(`q_close;1)
connect:        `qzmq 2:(`q_connect;2)
ctx_destroy:    `qzmq 2:(`q_ctx_destroy;1)
ctx_get:        `qzmq 2:(`q_ctx_get;2)
ctx_new:        `qzmq 2:(`q_ctx_new;1)
ctx_set:        `qzmq 2:(`q_ctx_set;3)
disconnect:     `qzmq 2:(`q_disconnect;2)
/getsockopt:    `qzmq 2:(`q_getsockopt;2)
send:           `qzmq 2:(`q_send;2)
send_multipart: `qzmq 2:(`q_send_multipart;2)
/proxy:         `qzmq 2:(`q_proxy;2)
setsockopt:     `qzmq 2:(`q_setsockopt;3)
socket:         `qzmq 2:(`q_socket;2)
unbind:         `qzmq 2:(`q_unbind;2)
version:        `qzmq 2:(`q_version;1)

// default special variables
.zmq.w: 0j
.zmq.W: `long$()
.zmq.c: 0j
.zmq.C: `long$()

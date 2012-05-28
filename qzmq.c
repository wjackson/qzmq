#include <k.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "qzmq.h"

void* bind_context = NULL;
void* bind_socket  = NULL;

K q_init (K thread_count_k) {
    assert(thread_count_k->t == -KI);

    bind_context = zmq_init(thread_count_k->i);
    return (K)0;
}

K q_socket (K socket_type_k) {
    assert(socket_type_k->t == -KI);

    bind_socket = zmq_socket(bind_context, socket_type_k->i);
    assert(bind_socket != NULL);

    return (K)0;
}

K q_setsockopt (K opt_k, K value_k) {

    assert(opt_k->t == -KI);
    assert(value_k->t == -KI || value_k->t == -KS);

    char    *ptr;
    int      len;
    uint64_t u64;
    int64_t  i64;
    int        i;
    int       rc;

    switch(opt_k->i){
        case ZMQ_LINGER:
        case ZMQ_RECONNECT_IVL:
        case ZMQ_BACKLOG:
            i = value_k->i;
            rc = zmq_setsockopt(bind_socket, opt_k->i, &i, sizeof(int));
            break;

        case ZMQ_IDENTITY:
        case ZMQ_SUBSCRIBE:
        case ZMQ_UNSUBSCRIBE:
            ptr = value_k->s;
            len = sizeof(char) * strlen(ptr);
            rc  = zmq_setsockopt(bind_socket, opt_k->i, ptr, len);
            break;

        case ZMQ_SWAP:
        case ZMQ_RATE:
        case ZMQ_RECOVERY_IVL:
        case ZMQ_RECOVERY_IVL_MSEC:
        case ZMQ_MCAST_LOOP:
            i64 = (int64_t) value_k->i;
            rc = zmq_setsockopt(bind_socket, opt_k->i, &i64, sizeof(int64_t));
            break;

        case ZMQ_HWM:
        case ZMQ_AFFINITY:
        case ZMQ_SNDBUF:
        case ZMQ_RCVBUF:
            u64 = (uint64_t) value_k->i;
            rc = zmq_setsockopt(bind_socket, opt_k->i, &u64, sizeof(uint64_t));
            break;

        default:
            warn("Unknown sockopt type %d, assuming string. Send patch", opt_k->i);
            ptr = value_k->s;
            len = sizeof(char) * strlen(ptr);
            rc  = zmq_setsockopt(bind_socket, opt_k->i, ptr, len);
    }

    assert(rc == 0);

    return (K)0;
}

K q_bind (K endpoint_k) {
    assert(endpoint_k->t == -KS);
    zmq_bind(bind_socket, endpoint_k->s);
    put_on_ev_loop(bind_socket);
    return (K)0;
}

K on_msg_cb (int fd) {

    int events = 0;
    size_t option_len;
    int rc = zmq_getsockopt(bind_socket, ZMQ_EVENTS, &events, &option_len);
    assert(rc == 0);

    // ignore everything but ZMQ_POLLIN events
    if (!(events & ZMQ_POLLIN))
        return (K)0;

    while (1) {
        zmq_msg_t message;
        zmq_msg_init(&message);

        int s = zmq_recv(bind_socket, &message, ZMQ_NOBLOCK);
        if (s == -1 && zmq_errno() == EAGAIN)
            break;

        assert(s == 0);

        void* msg = zmq_msg_data(&message);
        /* printf("Received Hello: %s\n", msg); */
        k(0, msg, (K)0);

        zmq_msg_close(&message);
    }

    return (K)0;
}

void put_on_ev_loop (void *socket) {
    int fd;
    size_t option_len;
    zmq_getsockopt(socket, ZMQ_FD, &fd, &option_len);
    sd1(-fd, on_msg_cb);

    return;
}

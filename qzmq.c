#include <k.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include "qzmq.h"

void* contexts[255];
int   context_count = 0;
void* sockets_by_fd[1024];

K q_init (K thread_count_k) {
    assert(thread_count_k->t == -KI);

    contexts[context_count] = zmq_init(thread_count_k->i);
    return ki(context_count++);
}

K q_socket (K context_k, K socket_type_k) {
    assert(context_k->t == -KI);
    assert(socket_type_k->t == -KI);

    void *context = contexts[context_k->i];
    void *socket  = zmq_socket(context, socket_type_k->i);
    assert(socket != NULL);

    int fd;
    size_t option_len;
    int rc = zmq_getsockopt(socket, ZMQ_FD, &fd, &option_len);
    assert(rc == 0);
    sockets_by_fd[fd] = socket;

    // tell q to call back when the socket is readable
    sd1(-fd, on_msg_cb);

    return ki(fd);
}

K q_setsockopt (K socket_fd_k, K opt_k, K value_k) {
    assert(socket_fd_k->t == -KI);
    assert(opt_k->t == -KI);
    assert(value_k->t == -KI || value_k->t == -KS);

    int fd = socket_fd_k->i;
    void *socket = sockets_by_fd[fd];

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
            rc = zmq_setsockopt(socket, opt_k->i, &i, sizeof(int));
            break;

        case ZMQ_IDENTITY:
        case ZMQ_SUBSCRIBE:
        case ZMQ_UNSUBSCRIBE:
            ptr = value_k->s;
            len = sizeof(char) * strlen(ptr);
            rc  = zmq_setsockopt(socket, opt_k->i, ptr, len);
            break;

        case ZMQ_SWAP:
        case ZMQ_RATE:
        case ZMQ_RECOVERY_IVL:
        case ZMQ_RECOVERY_IVL_MSEC:
        case ZMQ_MCAST_LOOP:
            i64 = (int64_t) value_k->i;
            rc = zmq_setsockopt(socket, opt_k->i, &i64, sizeof(int64_t));
            break;

        case ZMQ_HWM:
        case ZMQ_AFFINITY:
        case ZMQ_SNDBUF:
        case ZMQ_RCVBUF:
            u64 = (uint64_t) value_k->i;
            rc = zmq_setsockopt(socket, opt_k->i, &u64, sizeof(uint64_t));
            break;

        default:
            warn("Unknown sockopt type %d, assuming string. Send patch", opt_k->i);
            ptr = value_k->s;
            len = sizeof(char) * strlen(ptr);
            rc  = zmq_setsockopt(socket, opt_k->i, ptr, len);
    }

    assert(rc == 0);

    return (K)0;
}

K q_bind (K socket_fd_k, K endpoint_k) {
    assert(socket_fd_k->t == -KI);
    assert(endpoint_k->t == KC);

    int  fd = socket_fd_k->i;
    void *socket = sockets_by_fd[socket_fd_k->i];

    char endpoint[endpoint_k->n+1];
    endpoint[endpoint_k->n] = '\0';
    strncpy(endpoint, kC(endpoint_k), endpoint_k->n);

    int rc = zmq_bind(socket, endpoint);
    assert(rc == 0);

    return (K)0;
}

K q_connect (K socket_fd_k, K endpoint_k) {
    assert(socket_fd_k->t == -KI);
    assert(endpoint_k->t == KC);

    int fd = socket_fd_k->i;
    void *socket = sockets_by_fd[socket_fd_k->i];

    char endpoint[endpoint_k->n+1];
    endpoint[endpoint_k->n] = '\0';
    strncpy(endpoint, kC(endpoint_k), endpoint_k->n);

    int rc = zmq_connect(socket, endpoint);
    assert(rc == 0);

    return (K)0;
}

K q_send (K socket_fd_k, K msg_k) {
    assert(socket_fd_k->t == -KI);
    assert(msg_k->t == KC);

    int fd = socket_fd_k->i;
    void *socket = sockets_by_fd[socket_fd_k->i];

    zmq_msg_t msg;
    zmq_msg_init_size(&msg, msg_k->n);
    memcpy(zmq_msg_data (&msg), kC(msg_k), msg_k->n);
    zmq_send(socket, &msg, 0);
    zmq_msg_close(&msg);

    return (K)0;
}

K q_version (void) {
    int major, minor, patch;

    char *version;
    zmq_version(&major, &minor, &patch);
    asprintf(&version, "%d.%d.%d", major, minor, patch);
    K version_k =  ks(version);
    free(version);

    return version_k;
}

K on_msg_cb (int fd) {
    void *socket = sockets_by_fd[fd];
    int events = 0;
    size_t option_len;
    int rc = zmq_getsockopt(socket, ZMQ_EVENTS, &events, &option_len);
    assert(rc == 0);

    // ignore everything but ZMQ_POLLIN events
    if (!(events & ZMQ_POLLIN))
        return (K)0;

    while (1) {
        zmq_msg_t message;
        zmq_msg_init(&message);

        int s = zmq_recv(socket, &message, ZMQ_NOBLOCK);
        if (s == -1 && zmq_errno() == EAGAIN)
            break;

        assert(s == 0);

        char* msg = zmq_msg_data(&message);

        K msg_cb_k = k(0, ".zmq.ps", (K)0);
        if (msg_cb_k->t == -128) {  // msg_cb doesn't exist
            k(0, msg, (K)0);
        }
        else {
            k(0, ".zmq.ps", kp(msg), (K)0);
        }

        zmq_msg_close(&message);
    }

    return (K)0;
}

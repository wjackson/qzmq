#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <k.h>
#include <zmq.h>
#include "qzmq.h"

void      **CONTEXTS = NULL;
int    CONTEXT_COUNT = 0;
void **SOCKETS_BY_FD = NULL;
int           MAX_FD = 0;
char        *CB_NAME = ".zmq.ps";
int             KERR = -128;

static K zrr (char *err_label) {
    char *err_str;
    asprintf(&err_str, "%s: %s\n", err_label, zmq_strerror(errno));
    return krr(err_str);
}

K q_init (K thread_count_k) {
    if (thread_count_k->t != -KI) return krr("type");

    void *context = zmq_init(thread_count_k->i);
    if (context == NULL) return zrr("zmq_init");

    CONTEXTS = (void**) realloc(CONTEXTS, (CONTEXT_COUNT+1) * sizeof(void*));
    CONTEXTS[CONTEXT_COUNT] = context;
    return ki(CONTEXT_COUNT++);
}

K q_socket (K context_k, K socket_type_k) {
    if (context_k->t != -KI || socket_type_k->t != -KI) return krr("type");

    void *context = CONTEXTS[context_k->i];
    void  *socket = zmq_socket(context, socket_type_k->i);
    if (socket == NULL) return zrr("zmq_socket");

    int     fd;
    size_t len = sizeof(fd);
    int rc = zmq_getsockopt(socket, ZMQ_FD, &fd, &len);
    if (rc != 0) return zrr("zmq_sockopt");

    MAX_FD = MAX_FD > fd ? MAX_FD : fd;
    SOCKETS_BY_FD = (void **) realloc(SOCKETS_BY_FD, (MAX_FD+1) * sizeof(void*));
    SOCKETS_BY_FD[fd] = socket;

    // tell q to call back when the socket is readable
    sd1(-fd, on_msg_cb);

    return ki(fd);
}

K q_close (K socket_fd_k) {
    if (socket_fd_k->t != -KI) return krr("type");

    int        fd = socket_fd_k->i;
    void  *socket = SOCKETS_BY_FD[fd];

    sd0x(fd, 0); // remove the fd from the event loop

    int rc = zmq_close(socket);
    if (rc != 0) return zrr("zmq_close");

    SOCKETS_BY_FD[fd] = NULL;

    return (K)0;
}

K q_term (K context_k) {
    if (context_k->t != -KI) return krr("type");

    void *context = CONTEXTS[context_k->i];

    int rc = zmq_term(context);
    if (rc != 0) return zrr("zmq_term");

    return (K)0;
}

K q_setsockopt (K socket_fd_k, K opt_k, K value_k) {
    if (socket_fd_k->t != -KI || opt_k->t != -KI ||
       (value_k->t != -KI && value_k->t != KC)) {
        return krr("type");
    }

    int        fd = socket_fd_k->i;
    void  *socket = SOCKETS_BY_FD[fd];
    int       rc  = -1;
    uint64_t u64;
    int64_t  i64;
    int        i;

    switch(opt_k->i) {
        case ZMQ_LINGER:
        case ZMQ_RECONNECT_IVL:
        case ZMQ_BACKLOG:
            i  = value_k->i;
            rc = zmq_setsockopt(socket, opt_k->i, &i, sizeof(int));
            break;

        case ZMQ_IDENTITY:
        case ZMQ_SUBSCRIBE:
        case ZMQ_UNSUBSCRIBE:
            rc  = zmq_setsockopt(socket, opt_k->i, kC(value_k), value_k->n);
            break;

        case ZMQ_SWAP:
        case ZMQ_RATE:
        case ZMQ_RECOVERY_IVL:
        case ZMQ_RECOVERY_IVL_MSEC:
        case ZMQ_MCAST_LOOP:
            i64 = (int64_t) value_k->i;
            rc  = zmq_setsockopt(socket, opt_k->i, &i64, sizeof(int64_t));
            break;

        case ZMQ_HWM:
        case ZMQ_AFFINITY:
        case ZMQ_SNDBUF:
        case ZMQ_RCVBUF:
            u64 = (uint64_t) value_k->i;
            rc  = zmq_setsockopt(socket, opt_k->i, &u64, sizeof(uint64_t));
            break;

        default:
            warn("Unknown sockopt type %d, assuming string.", opt_k->i);
            rc  = zmq_setsockopt(socket, opt_k->i, kC(value_k), value_k->n);
    }

    if (rc != 0) return zrr("zmq_setsockopt");

    return (K)0;
}

K q_bind (K socket_fd_k, K endpoint_k) {
    if (socket_fd_k->t != -KI || endpoint_k->t != KC) return krr("type");

    int       fd = socket_fd_k->i;
    void *socket = SOCKETS_BY_FD[socket_fd_k->i];

    char endpoint[endpoint_k->n+1];
    endpoint[endpoint_k->n] = '\0';
    strncpy(endpoint, kC(endpoint_k), endpoint_k->n);

    int rc = zmq_bind(socket, endpoint);
    if (rc != 0) return zrr("zmq_bind");

    return (K)0;
}

K q_connect (K socket_fd_k, K endpoint_k) {
    if (socket_fd_k->t != -KI || endpoint_k->t != KC) return krr("type");

    int       fd = socket_fd_k->i;
    void *socket = SOCKETS_BY_FD[socket_fd_k->i];

    char endpoint[endpoint_k->n+1];
    endpoint[endpoint_k->n] = '\0';
    strncpy(endpoint, kC(endpoint_k), endpoint_k->n);

    int rc = zmq_connect(socket, endpoint);
    if (rc != 0) return zrr("zmq_connect");

    return (K)0;
}

K q_send (K socket_fd_k, K msg_k) {
    if (socket_fd_k->t != -KI || msg_k->t != KC) return krr("type");

    int       rc;
    int       fd = socket_fd_k->i;
    void *socket = SOCKETS_BY_FD[socket_fd_k->i];

    zmq_msg_t msg;
    rc = zmq_msg_init_size(&msg, msg_k->n);
    if (rc != 0) return zrr("zmq_msg_init_size");

    memcpy(zmq_msg_data(&msg), kC(msg_k), msg_k->n);

    rc = zmq_send(socket, &msg, 0);
    if (rc != 0) return zrr("zmq_send");

    rc = zmq_msg_close(&msg);
    if (rc != 0) return zrr("zmq_msg_close");

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
    int               rc;
    size_t len, msg_size;
    char        *msg_str;
    K           result_k;
    void         *socket;
    zmq_msg_t        msg;

    socket = SOCKETS_BY_FD[fd];

    uint32_t events;
    size_t events_size = sizeof(events);

    while (1) {
        // read until there's no more pollin event
        rc = zmq_getsockopt(socket, ZMQ_EVENTS, &events, &events_size);
        if (rc < 0) return zrr("zmq_getsockopt");
        if (!(events & ZMQ_POLLIN)) {
            break;
        }

        rc = zmq_msg_init(&msg);
        if (rc != 0) return zrr("zmq_msg_init");

        rc = zmq_recv(socket, &msg, ZMQ_NOBLOCK);
        if (rc != 0) return zrr("zmq_recv");

        msg_size = zmq_msg_size(&msg);
        msg_str  = (char*) malloc(msg_size+1);
        msg_str[msg_size] = 0;
        memcpy(msg_str, zmq_msg_data(&msg), msg_size);

        rc = zmq_msg_close(&msg);
        if (rc != 0) return zrr("zmq_msg_close");

        K msg_cb_k = k(0, CB_NAME, (K)0);
        if (msg_cb_k->t == KERR) {  // msg_cb doesn't exist
            result_k = k(0, msg_str, (K)0);
        }
        else {
            result_k = k(0, CB_NAME, kp(msg_str), (K)0);
        }

        if (result_k->t == KERR) krr(result_k->s);

        r0(msg_cb_k);
        r0(result_k);
    }

    return (K)0;
}

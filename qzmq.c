#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <k.h>
#include <zmq.h>

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

char* ktos (K k) {
    char *a = malloc(sizeof(char) * (k->n+1));
    a[k->n] = '\0';
    memcpy(a, kC(k), k->n);

    return a;
}

K q_ctx_new (K x) {
    void *context = zmq_ctx_new();
    if (context == NULL) return zrr("zmq_ctx_new");

    CONTEXTS = (void**) realloc(CONTEXTS, (CONTEXT_COUNT+1) * sizeof(void*));
    CONTEXTS[CONTEXT_COUNT] = context;
    return kj(CONTEXT_COUNT++);
}

K on_msg_cb (int);

K q_socket (K context_k, K socket_type_k) {
    if (context_k->t != -KJ || socket_type_k->t != -KI) return krr("type");

    void *context = CONTEXTS[context_k->j];
    void  *socket = zmq_socket(context, socket_type_k->i);
    if (socket == NULL) return zrr("zmq_socket");

    int     fd;
    size_t len = sizeof(fd);
    int rc = zmq_getsockopt(socket, ZMQ_FD, &fd, &len);
    if (rc == -1) return zrr("zmq_sockopt");

    MAX_FD = MAX_FD > fd ? MAX_FD : fd;
    SOCKETS_BY_FD = (void **) realloc(SOCKETS_BY_FD, (MAX_FD+1) * sizeof(void*));
    SOCKETS_BY_FD[fd] = socket;

    // tell q to call back when the socket is readable
    sd1(-fd, on_msg_cb);

    return kj(fd);
}

K q_close (K socket_fd_k) {
    if (socket_fd_k->t != -KJ) return krr("type");

    int        fd = socket_fd_k->j;
    void  *socket = SOCKETS_BY_FD[fd];

    sd0x(fd, 0); // remove the fd from the event loop

    int rc = zmq_close(socket);
    if (rc == -1) return zrr("zmq_close");

    SOCKETS_BY_FD[fd] = NULL;

    return (K)0;
}

K q_ctx_destroy (K context_k) {
    if (context_k->t != -KJ) return krr("type");

    void *context = CONTEXTS[context_k->j];

    int rc = zmq_ctx_destroy(context);
    if (rc == -1) return zrr("ctx_destroy");

    return (K)0;
}

K q_ctx_set (K context_k, K opt_k, K value_k) {
    if (context_k->t != -KJ || opt_k->t != -KI ||
       (value_k->t != -KJ && value_k->t != KC)) {
        return krr("type");
    }

    void *context = CONTEXTS[context_k->j];

    int rc = zmq_ctx_set(context, opt_k->i, value_k->j);
    if (rc == -1) return zrr("zmq_ctx_set");

    return (K)0;
}

K q_setsockopt (K socket_fd_k, K opt_k, K value_k) {
    if (socket_fd_k->t != -KJ || opt_k->t != -KI ||
       (value_k->t != -KJ && value_k->t != KC)) {
        return krr("type");
    }

    int        fd = socket_fd_k->j;
    void  *socket = SOCKETS_BY_FD[fd];
    int       rc  = -1;
    uint64_t u64;
    int64_t  i64;
    int        i;

    switch(opt_k->i) {
        case ZMQ_RCVHWM:
        case ZMQ_SNDHWM:
        case ZMQ_RATE:
        case ZMQ_RECOVERY_IVL:
        case ZMQ_SNDBUF:
        case ZMQ_RCVBUF:
        case ZMQ_LINGER:
        case ZMQ_RECONNECT_IVL:
        case ZMQ_RECONNECT_IVL_MAX:
        case ZMQ_BACKLOG:
        case ZMQ_MULTICAST_HOPS:
        case ZMQ_RCVTIMEO:
        case ZMQ_SNDTIMEO:
        case ZMQ_IPV4ONLY:
        case ZMQ_DELAY_ATTACH_ON_CONNECT:
        case ZMQ_XPUB_VERBOSE:
        case ZMQ_TCP_KEEPALIVE:
        case ZMQ_TCP_KEEPALIVE_IDLE:
        case ZMQ_TCP_KEEPALIVE_CNT:
        case ZMQ_TCP_KEEPALIVE_INTVL:
        case ZMQ_TCP_ACCEPT_FILTER:
            i  = value_k->i;
            rc = zmq_setsockopt(socket, opt_k->i, &i, sizeof(int));
            break;

        case ZMQ_IDENTITY:
        case ZMQ_SUBSCRIBE:
        case ZMQ_UNSUBSCRIBE:
            rc  = zmq_setsockopt(socket, opt_k->i, kC(value_k), value_k->n);
            break;

        case ZMQ_MAXMSGSIZE:
            i64 = value_k->j;
            rc  = zmq_setsockopt(socket, opt_k->i, &i64, sizeof(int64_t));
            break;

        case ZMQ_AFFINITY:
            u64 = value_k->j;
            rc  = zmq_setsockopt(socket, opt_k->i, &u64, sizeof(uint64_t));
            break;
    }

    if (rc == -1) return zrr("zmq_setsockopt");

    return (K)0;
}

K q_bind (K socket_fd_k, K endpoint_k) {
    if (socket_fd_k->t != -KJ || endpoint_k->t != KC) return krr("type");

    char *endpoint = ktos(endpoint_k);
    void *socket   = SOCKETS_BY_FD[socket_fd_k->j];

    int rc = zmq_bind(socket, endpoint);
    free(endpoint);
    if (rc == -1) return zrr("zmq_bind");

    return (K)0;
}

K q_connect (K socket_fd_k, K endpoint_k) {
    if (socket_fd_k->t != -KJ || endpoint_k->t != KC) return krr("type");

    char *endpoint = ktos(endpoint_k);
    void *socket   = SOCKETS_BY_FD[socket_fd_k->j];

    int rc = zmq_connect(socket, endpoint);
    free(endpoint);
    if (rc == -1) return zrr("zmq_connect");

    return (K)0;
}

K q_send (K socket_fd_k, K msg_k) {
    if (socket_fd_k->t != -KJ || msg_k->t != KC) return krr("type");

    int       rc;
    void *socket = SOCKETS_BY_FD[socket_fd_k->j];

    rc = zmq_send(socket, kC(msg_k), msg_k->n, 0);
    if (rc == -1) return zrr("zmq_send");

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
    int             rc;
    K         result_k;
    void       *socket;
    size_t    msg_size;
    char      *msg_str;
    zmq_msg_t      msg;

    socket = SOCKETS_BY_FD[fd];

    uint32_t events;
    size_t events_size = sizeof(events);

    while (1) {
        // read until there's no more pollin event
        rc = zmq_getsockopt(socket, ZMQ_EVENTS, &events, &events_size);
        if (rc == -1) return zrr("zmq_getsockopt");
        if (!(events & ZMQ_POLLIN)) {
            break;
        }

        rc = zmq_msg_init(&msg);
        if (rc == -1) return zrr("zmq_msg_init");

        rc = zmq_msg_recv(&msg, socket, ZMQ_DONTWAIT);
        if (rc == -1) return zrr("zmq_msg_recv");

        msg_size = zmq_msg_size(&msg);
        msg_str  = (char*) malloc(msg_size+1);
        msg_str[msg_size] = 0;
        memcpy(msg_str, zmq_msg_data(&msg), msg_size);

        rc = zmq_msg_close(&msg);
        if (rc == -1) return zrr("zmq_msg_close");

        K msg_cb_k = k(0, CB_NAME, (K)0);
        if (msg_cb_k->t == KERR) {  // msg_cb doesn't exist
            result_k = k(0, msg_str, (K)0);
        }
        else {
            result_k = k(0, CB_NAME, kp(msg_str), (K)0);
        }
        free(msg_str);

        if (result_k->t == KERR) krr(result_k->s);

        r0(msg_cb_k);
        r0(result_k);
    }

    return (K)0;
}

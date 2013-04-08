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
    const char *zmq_err = zmq_strerror(errno);
    size_t     err_size = strlen(err_label) + strlen(zmq_err) + 4;
    char       *err_buf = malloc(sizeof(char) * err_size);
    if (err_buf == NULL) return krr("wsfull");

    snprintf(err_buf, err_size, "%s: %s\n", err_label, zmq_err);
    return krr(err_buf);
}

K q_ctx_new (K x) {
    void *context = zmq_ctx_new();
    if (context == NULL) return zrr("zmq_ctx_new");

    CONTEXTS = realloc(CONTEXTS, (CONTEXT_COUNT+1) * sizeof(void*));
    CONTEXTS[CONTEXT_COUNT] = context;
    return kj(CONTEXT_COUNT++);
}

K on_msg_cb (int);

K q_socket (K context_k, K socket_type_k) {
    if (context_k->t != -KJ || socket_type_k->t != -KI) return krr("type");

    void *context = CONTEXTS[context_k->j];
    void  *socket = zmq_socket(context, socket_type_k->i);
    if (socket == NULL) return zrr("zmq_socket");

    int fd;
    size_t len = sizeof(fd);
    int rc = zmq_getsockopt(socket, ZMQ_FD, &fd, &len);
    if (rc == -1) return zrr("zmq_sockopt");

    MAX_FD = MAX_FD > fd ? MAX_FD : fd;
    SOCKETS_BY_FD = realloc(SOCKETS_BY_FD, (MAX_FD+1) * sizeof(void*));
    SOCKETS_BY_FD[fd] = socket;

    // tell q to call back when the socket is readable
    sd1(-fd, on_msg_cb);

    return kj(fd);
}

K q_close (K socket_fd_k) {
    if (socket_fd_k->t != -KJ) return krr("type");

    int       rc;
    int       fd = socket_fd_k->j;
    void *socket = SOCKETS_BY_FD[fd];

    sd0x(fd, 0); // remove the fd from the event loop

    rc = zmq_close(socket);
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
    int        rc = -1;
    uint64_t  u64;
    int64_t   i64;
    int         i;

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

    char *endpoint = malloc(sizeof(char) * (endpoint_k->n+1));
    if (endpoint == NULL) return krr("wsfull");
    endpoint[endpoint_k->n] = '\0';
    memcpy(endpoint, kC(endpoint_k), endpoint_k->n);

    void *socket = SOCKETS_BY_FD[socket_fd_k->j];

    int rc = zmq_bind(socket, endpoint);
    free(endpoint);
    if (rc == -1) return zrr("zmq_bind");

    return (K)0;
}

K q_connect (K socket_fd_k, K endpoint_k) {
    if (socket_fd_k->t != -KJ || endpoint_k->t != KC) return krr("type");

    char *endpoint = malloc(sizeof(char) * (endpoint_k->n+1));
    if (endpoint == NULL) return krr("wsfull");
    endpoint[endpoint_k->n] = '\0';
    memcpy(endpoint, kC(endpoint_k), endpoint_k->n);

    void *socket = SOCKETS_BY_FD[socket_fd_k->j];

    int rc = zmq_connect(socket, endpoint);
    free(endpoint);
    if (rc == -1) return zrr("zmq_connect");

    return (K)0;
}

K q_send (K socket_fd_k, K msg_k) {
    if (socket_fd_k->t != -KJ) return krr("type");

    int rc;
    void *socket = SOCKETS_BY_FD[socket_fd_k->j];

    rc = zmq_send(socket, kC(msg_k), msg_k->n, 0);
    if (rc == -1) return zrr("zmq_send");

    return (K)0;
}

K q_send_multipart (K socket_fd_k, K envelope_k) {
    if (socket_fd_k->t != -KJ || envelope_k->t != 0) return krr("type");

    int i, rc;
    K part_k;
    void *socket = SOCKETS_BY_FD[socket_fd_k->j];

    for (i = 0; i < envelope_k->n; i++) {
        part_k = kK(envelope_k)[i];
        if (part_k->t != KC) {
            return krr("type");
        }
    }

    for (i = 0; i < envelope_k->n - 1; i++) {
        part_k = kK(envelope_k)[i];
        rc   = zmq_send(socket, kC(part_k), part_k->n, ZMQ_SNDMORE);
        if (rc == -1) return zrr("zmq_send");
    }

    // last part
    part_k = kK(envelope_k)[envelope_k->n-1];
    rc   = zmq_send(socket, kC(part_k), part_k->n, 0);
    if (rc == -1) return zrr("zmq_send");

    return (K)0;
}

K q_version (void) {
    int major, minor, patch;

    char version[16];
    zmq_version(&major, &minor, &patch);
    snprintf(version, 16, "%d.%d.%d", major, minor, patch);
    K version_k =  ks(version);

    return version_k;
}

K on_msg_cb (int fd) {
    int             rc, i;
    K            result_k;
    void          *socket;
    size_t      part_size;
    char        *part_buf;
    zmq_msg_t        part;
    size_t     part_count = 0;
    char       **envelope = NULL;

    socket = SOCKETS_BY_FD[fd];

    uint32_t events;
    size_t events_size = sizeof(events);

    while (1) {
        /* read until there's no more pollin event */
        rc = zmq_getsockopt(socket, ZMQ_EVENTS, &events, &events_size);
        if (rc == -1) return zrr("zmq_getsockopt");
        if (!(events & ZMQ_POLLIN)) {
            break;
        }

        // read all the message parts
        while (1) {
            part_count++;
            envelope = realloc(envelope, part_count * sizeof(char*));
            if (envelope == NULL) return krr("wsfull");

            rc = zmq_msg_init(&part);
            if (rc == -1) return zrr("zmq_msg_init");

            rc = zmq_msg_recv(&part, socket, ZMQ_DONTWAIT);
            if (rc == -1) return zrr("zmq_msg_recv");

            part_size = zmq_msg_size(&part);
            part_buf  = malloc(part_size+1);
            if (part_buf == NULL) return krr("wsfull");
            part_buf[part_size] = 0;
            memcpy(part_buf, zmq_msg_data(&part), part_size);

            rc = zmq_msg_close(&part);
            if (rc == -1) return zrr("zmq_msg_close");

            envelope[part_count-1] = part_buf;

            if (!zmq_msg_more(&part)) break;
        }

        K msg_cb_k = k(0, CB_NAME, (K)0);
        if (msg_cb_k->t == KERR) {  /* msg_cb doesn't exist */
            result_k = k(0, envelope[part_count-1], (K)0);
        }
        else {
            K envelope_k = ktn(0, part_count);
            for (i = 0; i < part_count; i++) {
                kK(envelope_k)[i] = kp(envelope[i]);
            }

            result_k = k(0, CB_NAME, envelope_k, (K)0);
        }

        // cleanup the part buffers
        for (i = 0; i < part_count; i++) {
            free(envelope[i]);
        }
        free(envelope);

        if (result_k->t == KERR) return krr(result_k->s);

        r0(msg_cb_k);
        r0(result_k);
    }

    return (K)0;
}

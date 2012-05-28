#include <k.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "qzmq.h"

K q_send_hi (void) {

    void *context = zmq_init(1);

    //  Socket to talk to server
    printf ("Connecting to hello world server…\n");
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        zmq_msg_t request;
        zmq_msg_init_size (&request, 5);
        memcpy (zmq_msg_data (&request), "Hello", 5);
        printf ("Sending Hello %d…\n", request_nbr);
        zmq_send (requester, &request, 0);
        zmq_msg_close (&request);

        zmq_msg_t reply;
        zmq_msg_init (&reply);
        zmq_recv (requester, &reply, 0);
        printf ("Received World %d\n", request_nbr);
        zmq_msg_close (&reply);
    }

    zmq_close (requester);
    zmq_term (context);

    return (K)0;
}

void *s_context;
void *s_responder;

K q_recv_hi(void)
{
    printf("q_recv_hi\n");
    // listen on the port
    s_context = zmq_init (1);

    //  Socket to talk to clients
    s_responder = zmq_socket (s_context, ZMQ_REP);
    zmq_bind (s_responder, "tcp://*:5555");

    int option_value;
    size_t option_len = sizeof(int);
    zmq_getsockopt(s_responder, ZMQ_FD, &option_value, &option_len);
    int fd = option_value;

    sd1(-fd, q_recv_cb);
    R (K)0;
}

int i = 0;
int j = 0;
K q_recv_cb (int d)
{
    i++;
    if (i > 100) {
        printf("too many calls\n");
        exit(1);
    }

    /* printf("q_recv_cb: %d\n", d); */

    int err;
    int events = 0;
    size_t option_len = sizeof(int);

    zmq_getsockopt(s_responder, ZMQ_EVENTS, &events, &option_len);

    if (events & ZMQ_POLLOUT) {
        /* printf("ZMQ_POLLOUT\n"); */
    }
    else if (events & ZMQ_POLLIN) {
        /* printf("ZMQ_POLLIN\n"); */

        int recv_more = 1;
        while (recv_more) {
            //  Wait for next request from client
            zmq_msg_t request;
            zmq_msg_init(&request);

            int s = zmq_recv(s_responder, &request, ZMQ_NOBLOCK);

            if (s == -1 && zmq_errno() == EAGAIN) {
                recv_more = 0;
            }
            else {
                char* msg = zmq_msg_data(&request);

                /* printf ("Received Hello: %s\n", msg); */
                zmq_msg_close(&request);

                /* //  Do some 'work' */
                /* sleep (1); */

                //  Send reply back to client
                zmq_msg_t reply;
                zmq_msg_init_size(&reply, 5);

                memcpy(zmq_msg_data (&reply), "World", 5);

                zmq_send(s_responder, &reply, 0);
                zmq_msg_close(&reply);

                /* printf ("Replied\n\n"); */
            }
        }
    }
    else {
        /* printf("unrecognized poll event\n"); */
    }


    R (K)0;
}

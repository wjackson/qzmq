//
//  Hello World server
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>

int main (void)
{
    void *context = zmq_init (1);

    //  Socket to talk to clients
    void *responder = zmq_socket (context, ZMQ_REP);
    zmq_bind (responder, "tcp://*:5555");

    zmq_pollitem_t items [] = {
        { responder, 0, ZMQ_POLLIN, 0 },
    };

    int events;
    int fd = 0;
    size_t option_len = sizeof(int);
    zmq_getsockopt(responder, ZMQ_FD, &fd, &option_len);

    printf("fd: %d\n", fd);

    fd_set rfds;
    fd_set wfds;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    FD_SET(fd, &rfds);

    /* select(16, [0 14 15], [], NULL, {0, 998279}) = 0 (Timeout) */

    int i;
    for (i = 0; i < 20; i++) {

        printf("i: %d\n", i);

        /* printf("start poll\n"); */
        /* zmq_poll(items, 1, -1); */
        /* printf("stop poll\n"); */

        // select(16, [0 14 15], [], NULL, {0, 998279}) = 0 (Timeout)
        /* select(fd+1, &fds, NULL, NULL, NULL); */

        printf("b4 select\n");
        select(fd+1, &rfds, &wfds, NULL, NULL);
        printf("after select\n");

        zmq_getsockopt(responder, ZMQ_EVENTS, &events, &option_len);

        if (events & ZMQ_POLLIN) {
            printf("POLLIN\n");

            printf("read time\n");

            int go = 1;

            while (go) {
                zmq_msg_t request;
                zmq_msg_init (&request);
                int s = zmq_recv(responder, &request, ZMQ_NOBLOCK);

                if (s == -1 && zmq_errno() == EAGAIN) {
                    go = 0;
                }
                else if (s == -1) {
                    // die
                }
                else {
                    char* msg = zmq_msg_data(&request);
                    printf("Received %s\n", msg);
                    zmq_msg_close (&request);

                    //  Do some 'work'
                    sleep (1);

                    //  Send reply back to client
                    zmq_msg_t reply;
                    zmq_msg_init_size (&reply, 5);
                    memcpy (zmq_msg_data (&reply), "World", 5);
                    zmq_send (responder, &reply, 0);
                    zmq_msg_close (&reply);
                }
            }

            /* FD_SET(16, &fds); */
            /* max = 17; */
        }

        else if (events & ZMQ_POLLOUT) {
            printf("POLLOUT\n");
        }

        else {
            printf("UNRECOGINIZED\n");
        }

    }
    //  We never get here but if we did, this would be how we end
    zmq_close (responder);
    zmq_term (context);
    return 0;
}


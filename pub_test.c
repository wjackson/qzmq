#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main (void)
{
    char buf[7];
    void *context = zmq_init(1);
    int rc;

    //  Socket to talk to server
    printf ("Connecting to hello world server…\n");
    void *publisher = zmq_socket(context, ZMQ_PUB);
    rc = zmq_connect (publisher, "tcp://localhost:5555");
    assert (rc == 0);

    int msg_nbr;
    for (msg_nbr = 0; msg_nbr < 10; msg_nbr++) {
        zmq_msg_t msg;
        zmq_msg_init_size (&msg, 9);
        sprintf(buf, "show `hi%d", msg_nbr);
        memcpy(zmq_msg_data (&msg), buf, 9);
        printf("Sending Hello %d…\n", msg_nbr);
        zmq_send(publisher, &msg, 0);
        zmq_msg_close(&msg);

        sleep(1);
    }

    printf("close\n");
    zmq_close (publisher);

    printf("term\n");
    zmq_term (context);

    printf("return\n");
    return 0;
}

#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#include "common.h"

int main()
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSG_COUNT;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(
        QUEUE_NAME,
        O_CREAT | O_RDONLY,
        0666,
        &attr
    );

    if (mq == (mqd_t)-1)
    {
        perror("mq_open");
        return 1;
    }

    std::cout << "Receiver is running...\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    while (true)
    {
        char buffer[MAX_MSG_SIZE];
        unsigned int priority = 0;

        ssize_t bytes_read = mq_receive(
            mq,
            buffer,
            MAX_MSG_SIZE,
            &priority
        );

        if (bytes_read == -1)
        {
            perror("mq_receive");
            break;
        }

        buffer[bytes_read] = '\0';

        std::cout << "Received: "
                  << buffer
                  << " | priority = "
                  << priority
                  << std::endl;

        usleep(700000); // delay 0.7 second
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    return 0;
}
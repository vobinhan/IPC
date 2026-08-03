#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

#include "common.h"

int main()
{
    mqd_t mq = mq_open(
        QUEUE_NAME,
        O_WRONLY
    );

    if (mq == (mqd_t)-1)
    {
        perror("mq_open");
        return 1;
    }

    std::cout << "Normal Sender started.\n";
    std::cout << "Enter message. Type exit to quit.\n\n";

    std::string input;

    while (true)
    {
        std::cout << "Normal > ";
        std::getline(std::cin, input);

        if (input == "exit")
        {
            break;
        }

        if (mq_send(
                mq,
                input.c_str(),
                input.size(),
                NORMAL_PRIORITY
            ) == -1)
        {
            perror("mq_send");
            break;
        }
    }

    mq_close(mq);

    return 0;
}
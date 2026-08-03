#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

#include "shared_data.h"

int main()
{
    int fd = shm_open(SHM_NAME,
                      O_RDWR,
                      0666);

    if (fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    SharedData *data =
        (SharedData *)mmap(nullptr,
                           sizeof(SharedData),
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED,
                           fd,
                           0);

    if (data == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    std::cout << "Reader reads:\n";

    std::cout << "Counter = "
              << data->counter
              << std::endl;

    std::cout << "Message = "
              << data->message
              << std::endl;

    munmap(data, sizeof(SharedData));

    close(fd);

    return 0;
}
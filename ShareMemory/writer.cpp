#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "shared_data.h"

int main()
{
    /* Kernel tạo một đối tượng Shared Memory.
        Writer
        ↓
        Kernel
        ↓
        Shared Memory Object
    */
    int fd = shm_open(SHM_NAME,
                      O_CREAT | O_RDWR,
                      0666);

    if (fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    // Kernel cap phat vung nho co kich thuoc sizeof(SharedData)
    ftruncate(fd, sizeof(SharedData));

    // Kernel ánh xạ vùng Shared Memory vào Virtual Address Space của Writer
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

    data->counter = 100;

    strcpy(data->message,
           "Hello from Writer Process");

    std::cout << "Writer writes:\n";
    std::cout << "Counter = "
              << data->counter
              << std::endl;

    std::cout << "Message = "
              << data->message
              << std::endl;

    std::cout << "\nPress ENTER to exit...";
    std::cin.get();

    munmap(data, sizeof(SharedData));

    close(fd);

    shm_unlink(SHM_NAME);

    return 0;
}
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

#include "common.h"

int main()
{
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (client_fd == -1)
    {
        perror("socket");
        return 1;
    }

    sockaddr_un server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sun_family = AF_UNIX;
    std::strncpy(server_addr.sun_path,
                 SOCKET_PATH,
                 sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd,
                reinterpret_cast<sockaddr*>(&server_addr),
                sizeof(server_addr)) == -1)
    {
        perror("connect");
        close(client_fd);
        return 1;
    }

    std::cout << "Connected to server.\n";
    std::cout << "Enter message. Type exit to quit.\n\n";

    std::string input;
    char buffer[BUFFER_SIZE];

    while (true)
    {
        std::cout << "Client > ";
        std::getline(std::cin, input);

        if (input == "exit")
        {
            break;
        }

        if (send(client_fd,
                 input.c_str(),
                 input.size(),
                 0) == -1)
        {
            perror("send");
            break;
        }

        std::memset(buffer, 0, sizeof(buffer));

        ssize_t bytes_read = recv(client_fd,
                                  buffer,
                                  sizeof(buffer) - 1,
                                  0);

        if (bytes_read == -1)
        {
            perror("recv");
            break;
        }

        if (bytes_read == 0)
        {
            std::cout << "Server disconnected.\n";
            break;
        }

        buffer[bytes_read] = '\0';

        std::cout << buffer << std::endl;
    }

    close(client_fd);

    return 0;
}
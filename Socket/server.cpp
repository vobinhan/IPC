#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "common.h"

int main()
{
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("socket");
        return 1;
    }

    // Xóa socket file cũ nếu tồn tại
    unlink(SOCKET_PATH);

    sockaddr_un server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sun_family = AF_UNIX;
    std::strncpy(server_addr.sun_path,
                 SOCKET_PATH,
                 sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd,
             reinterpret_cast<sockaddr*>(&server_addr),
             sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) == -1)
    {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    std::cout << "Server is listening on "
              << SOCKET_PATH
              << std::endl;

    int client_fd = accept(server_fd, nullptr, nullptr);

    if (client_fd == -1)
    {
        perror("accept");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    std::cout << "Client connected.\n";

    char buffer[BUFFER_SIZE];

    while (true)
    {
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
            std::cout << "Client disconnected.\n";
            break;
        }

        buffer[bytes_read] = '\0';

        std::cout << "Server received: "
                  << buffer
                  << std::endl;

        std::string reply = "Echo from server: ";
        reply += buffer;

        if (send(client_fd,
                 reply.c_str(),
                 reply.size(),
                 0) == -1)
        {
            perror("send");
            break;
        }
    }

    close(client_fd);
    close(server_fd);

    unlink(SOCKET_PATH);

    return 0;
}
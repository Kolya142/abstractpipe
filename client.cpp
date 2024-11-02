#include "PIPEClient.hpp"
#include <iostream>
#include <string.h>
#include <unistd.h>

int main() {
    std::cout << "./client.elf";
    PIPE::PIPEClient client = PIPE::PIPEClient("pipecommtest");
    std::cout.flush();
    while (true) {
        std::cout.flush();
        char* data = client.recv();
        std::cout << data << '\n';
        std::cout.flush();
        if (strcmp(data, "exit") == 0) {
            std::cout << "Received exit command. Closing connection." << std::endl;
            free(data);
            break;
        }

    }
    client.~PIPEClient();
}
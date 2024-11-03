#include "PIPEServer.hpp"
#include <iostream>
#include <string.h>
#include <unistd.h>

int main() {
    std::cout << "./server.elf\n";
    std::cout.flush();
    PIPE::PIPEServer server = PIPE::PIPEServer("pipecommtest");
    std::string data;
    int i = 0;
    while (true) {
        char* data = strdup(std::to_string(i).c_str());
        if (i >= 10000) {
            data = "exit";
        }
        std::cout << data << '\n';
        server.send(data);
        if (strcmp(data, "exit") == 0) {
            std::cout << "Received exit command. Closing connection." << std::endl;
            free(data);
            break;
        }
        i++;
        usleep(2);
    }
}
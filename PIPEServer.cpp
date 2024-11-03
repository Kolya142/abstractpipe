#include "PIPEServer.hpp"
#include <stdlib.h>
#include <unistd.h>
namespace PIPE {
    PIPEServer::PIPEServer(char* pipe_name)
    {
        char cmd[600];
        #ifdef __linux__
        std::sprintf(cmd, "./gope.elf a %s %d", pipe_name, getpid());
        #elif __APPLE__
        std::sprintf(cmd, "./gope.elf a %s %d", pipe_name, getpid());
        #else
        std::sprintf(cmd, "gope.exe a %s %d", pipe_name, getpid());
        #endif
        system(cmd);
        pipe = CreatePipe();
    }

    void PIPEServer::send(char* data)
    {
        SendBytes(pipe, data);
    }

    char* PIPEServer::recv()
    {
        return RecvBytes(pipe);
    }

    PIPEServer::~PIPEServer()
    {
        ClosePipe(pipe);
    }
}
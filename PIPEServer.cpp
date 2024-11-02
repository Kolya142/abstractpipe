#include "PIPEServer.hpp"
#include <stdlib.h>
#include <unistd.h>
namespace PIPE {
    PIPEServer::PIPEServer(char* pipe_name)
    {
        char cmd[600];
        std::sprintf(cmd, "./gope.elf a %s %d", pipe_name, getpid());
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
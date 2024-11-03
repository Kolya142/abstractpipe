#include "PIPEClient.hpp"
#include <stdlib.h>
#include <fstream>
namespace PIPE {
    PIPEClient::PIPEClient(char* pipe_name)
    {
        int pipe_id;
        char cmd[600];
        #ifdef __linux__
        std::sprintf(cmd, "./gope.elf g %s", pipe_name);
        #elif __APPLE__
        std::sprintf(cmd, "./gope.elf g %s", pipe_name);
        #else
        std::sprintf(cmd, "gope.exe g %s", pipe_name);
        #endif
        system(cmd);
        std::ifstream pco;
        #ifdef __linux__
        pco.open("/tmp/pco.txt");
        #elif __APPLE__
        pco.open("/tmp/pco.txt");
        #else
        pco.open("pco.txt");
        #endif
        std::string pcos;
        std::getline(pco, pcos);
        pipe_id = std::stoi(pcos);
        pipe = ConnectPipe(pipe_id);
    }

    void PIPEClient::send(char* data)
    {
        SendBytes(pipe, data);
    }

    char* PIPEClient::recv()
    {
        return RecvBytes(pipe);
    }

    PIPEClient::~PIPEClient()
    {
        ClosePipe(pipe);
    }
}
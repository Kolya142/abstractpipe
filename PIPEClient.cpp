#include "PIPEClient.hpp"
#include <stdlib.h>
#include <fstream>
namespace PIPE {
    PIPEClient::PIPEClient(char* pipe_name)
    {
        int pipe_id;
        char cmd[600];
        std::sprintf(cmd, "./gope.elf g %s", pipe_name);
        system(cmd);
        std::ifstream pco;
        pco.open("/tmp/pco.txt");
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
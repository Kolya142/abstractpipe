#include "PIPEHandler.hpp"
namespace PIPE {
    class PIPEServer {
        int* pipe;
    public:
        PIPEServer(char* pipe_name);
        void send(char* data);
        char* recv();
        ~PIPEServer();
    };
}
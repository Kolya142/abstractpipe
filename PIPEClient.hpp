#include "PIPEHandler.hpp"
namespace PIPE {
    class PIPEClient {
        int* pipe;
    public:
        PIPEClient(char* pipe_name);
        void send(char* data);
        char* recv();
        ~PIPEClient();
    };
}
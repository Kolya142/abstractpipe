#include "shash.hpp"

int* CreatePipe();
int* ConnectPipe(int pid);
char* RecvBytes(int* pipe);
void SendBytes(int* pipe, const char* data);
void ClosePipe(int* pipe);

/*
int* pipe stucture - [client fd, server fd]
*/

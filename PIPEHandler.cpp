#include "PIPEHandler.hpp"

#ifdef _WIN32
// TODO: implement for Win32 platform
#endif

#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
// #include <iostream>

int* CreatePipe() {
    pid_t pid = getpid();
    int pipeh = Shash::calculate_shash(pid);
    int* pipe_arr = (int*)malloc(2 * sizeof(int));
    
    if (pipe_arr == NULL) {
        perror("malloc failed");
        return NULL;
    }

    char pipe_read_name[100];
    char pipe_write_name[100];
    snprintf(pipe_read_name, sizeof(pipe_read_name), "/tmp/commpipe-%s-client", Shash::get_hex(pipeh));
    snprintf(pipe_write_name, sizeof(pipe_write_name), "/tmp/commpipe-%s-server", Shash::get_hex(pipeh));

    if (mkfifo(pipe_read_name, 0666) == -1) {
        perror("mkfifo read pipe failed");
        free(pipe_arr);
        return NULL;
    }
    
    if (mkfifo(pipe_write_name, 0666) == -1) {
        perror("mkfifo write pipe failed");
        unlink(pipe_read_name);
        free(pipe_arr);
        return NULL;
    }

    pipe_arr[0] = open(pipe_read_name, O_RDONLY | O_NONBLOCK);
    if (pipe_arr[0] == -1) {
        perror("open read pipe failed");
        unlink(pipe_read_name);
        unlink(pipe_write_name);
        free(pipe_arr);
        return NULL;
    }
    
    pipe_arr[1] = open(pipe_write_name, O_WRONLY);
    if (pipe_arr[1] == -1) {
        perror("open write pipe failed");
        close(pipe_arr[0]);
        unlink(pipe_read_name);
        unlink(pipe_write_name);
        free(pipe_arr);
        return NULL;
    }

    return pipe_arr;
}

int* ConnectPipe(int pid) {
    int pipeh = Shash::calculate_shash(pid);
    int* pipe_arr = (int*)malloc(2 * sizeof(int));
    if (!pipe_arr) {
        perror("Memory allocation failed");
        return NULL;
    }

    char pipe_read_name[100];
    char pipe_write_name[100];
    sprintf(pipe_read_name, "/tmp/commpipe-%s-server", Shash::get_hex(pipeh));
    sprintf(pipe_write_name, "/tmp/commpipe-%s-client", Shash::get_hex(pipeh));

    // std::cout << "Connecting to pipes:" << std::endl;
    // std::cout << "Read pipe: " << pipe_read_name << std::endl;
    // std::cout << "Write pipe: " << pipe_write_name << std::endl;
    // std::cout.flush();

    pipe_arr[0] = open(pipe_read_name, O_RDONLY);
    if (pipe_arr[0] == -1) {
        perror("open read pipe failed");
        free(pipe_arr);
        return NULL;
    }
    // std::cout << "Opened read pipe successfully" << std::endl;

    pipe_arr[1] = open(pipe_write_name, O_WRONLY);
    if (pipe_arr[1] == -1) {
        perror("open write pipe failed");
        close(pipe_arr[0]);
        free(pipe_arr);
        return NULL;
    }
    // std::cout << "Opened write pipe successfully" << std::endl;

    return pipe_arr;
}


char* RecvBytes(int* pipe) {
    char* data = (char*)malloc(4);
    if (data == NULL) {
        perror("malloc failed");
        return NULL;
    }

    ssize_t bytes_read = read(pipe[0], data, 4);
    if (bytes_read == -1) {
        perror("read failed");
        free(data);
        return NULL;
    }
    
    return data;
}

void SendBytes(int* pipe, const char* data) {
    if (write(pipe[1], data, 4) == -1) {
        perror("write failed");
    }
}

void ClosePipe(int* pipe) {
    if (pipe != NULL) {
        close(pipe[0]);
        close(pipe[1]);
        free(pipe);
    }
}

#endif

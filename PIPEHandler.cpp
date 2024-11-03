#include "PIPEHandler.hpp"
// win32 support not support
#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int* CreatePipe() {
    HANDLE* pipe_arr = (HANDLE*)malloc(2 * sizeof(HANDLE));
    if (pipe_arr == NULL) {
        perror("malloc failed");
        return NULL;
    }

    HANDLE readPipe, writePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
        perror("CreatePipe failed");
        free(pipe_arr);
        return NULL;
    }

    pipe_arr[0] = readPipe;
    pipe_arr[1] = writePipe;

    return (int*)pipe_arr;
}

int* ConnectPipe(int pid) {
    HANDLE* pipe_arr = (HANDLE*)malloc(2 * sizeof(HANDLE));
    if (pipe_arr == NULL) {
        perror("malloc failed");
        return NULL;
    }

    char readPipeName[256];
    char writePipeName[256];
    snprintf(readPipeName, sizeof(readPipeName), "\\\\.\\pipe\\commpipe-%d-client", pid);
    snprintf(writePipeName, sizeof(writePipeName), "\\\\.\\pipe\\commpipe-%d-server", pid);

    HANDLE readPipe = CreateFile(
        readPipeName,
        GENERIC_READ,
        0, // No sharing
        NULL, // Default security attributes
        OPEN_EXISTING,
        0, // Default attributes
        NULL  // No template
    );

    if (readPipe == INVALID_HANDLE_VALUE) {
        perror("Failed to open read pipe");
        free(pipe_arr);
        return NULL;
    }

    HANDLE writePipe = CreateFile(
        writePipeName,
        GENERIC_WRITE,
        0, // No sharing
        NULL, // Default security attributes
        OPEN_EXISTING,
        0, // Default attributes
        NULL  // No template
    );

    if (writePipe == INVALID_HANDLE_VALUE) {
        perror("Failed to open write pipe");
        CloseHandle(readPipe);
        free(pipe_arr);
        return NULL;
    }

    pipe_arr[0] = readPipe;
    pipe_arr[1] = writePipe;

    return (int*)pipe_arr;
}

char* RecvBytes(int* pipe) {
    char* data = (char*)malloc(4);
    if (data == NULL) {
        perror("malloc failed");
        return NULL;
    }

    DWORD bytesRead;
    if (!ReadFile((HANDLE)pipe[0], data, 4, &bytesRead, NULL)) {
        perror("read failed");
        free(data);
        return NULL;
    }
    
    return data;
}

void SendBytes(int* pipe, const char* data) {
    DWORD bytesWritten;
    if (!WriteFile((HANDLE)pipe[1], data, 4, &bytesWritten, NULL)) {
        perror("write failed");
    }
}

void ClosePipe(int* pipe) {
    if (pipe != NULL) {
        CloseHandle((HANDLE)pipe[0]);
        CloseHandle((HANDLE)pipe[1]);
        free(pipe);
    }
}

#endif

#ifdef __APPLE__
// macOS specific includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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

    pipe_arr[0] = open(pipe_read_name, O_RDONLY);
    if (pipe_arr[0] == -1) {
        perror("open read pipe failed");
        free(pipe_arr);
        return NULL;
    }

    pipe_arr[1] = open(pipe_write_name, O_WRONLY);
    if (pipe_arr[1] == -1) {
        perror("open write pipe failed");
        close(pipe_arr[0]);
        free(pipe_arr);
        return NULL;
    }

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

#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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

    pipe_arr[0] = open(pipe_read_name, O_RDONLY);
    if (pipe_arr[0] == -1) {
        perror("open read pipe failed");
        free(pipe_arr);
        return NULL;
    }

    pipe_arr[1] = open(pipe_write_name, O_WRONLY);
    if (pipe_arr[1] == -1) {
        perror("open write pipe failed");
        close(pipe_arr[0]);
        free(pipe_arr);
        return NULL;
    }

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

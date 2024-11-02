#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

void make_process(char* app) {
    execl("kitty", "kitty", app);
}

void main() {
    
}
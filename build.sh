#!/bin/bash
g++ client.cpp PIPEHandler.cpp shash.cpp PIPEClient.cpp -o client.elf
g++ server.cpp PIPEHandler.cpp shash.cpp PIPEServer.cpp -o server.elf
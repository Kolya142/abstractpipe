#!/bin/bash
g++ client.cpp PIPEHandler.cpp shash.cpp PIPEClient.cpp -o client-mac.elf
g++ server.cpp PIPEHandler.cpp shash.cpp PIPEServer.cpp -o server-mac.elf
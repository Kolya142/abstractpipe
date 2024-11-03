@echo off
g++ client.cpp PIPEHandler.cpp shash.cpp PIPEClient.cpp -o client.exe
g++ server.cpp PIPEHandler.cpp shash.cpp PIPEServer.cpp -o server.exe
pause
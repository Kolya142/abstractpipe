# c++ socket-like communication implementation with pipe
## supported systems
- [x] **linux** x86_64\
- [ ] **windows** x32\
- [ ] **macos** amd64\
- [ ] **macos** arm64
## usage
abstract classes
- ```c++
  #include "PIPEClient.hpp"
  #include <iostream>
  #include <string.h>
  #include <unistd.h>
  
  int main() {
      std::cout << "./client.elf";
      PIPE::PIPEClient client = PIPE::PIPEClient("pipecommtest"); // Create client
      std::cout.flush();
      while (true) {
          std::cout.flush();
          char* data = client.recv(); // Get data
          std::cout << data << '\n';
          std::cout.flush();
          if (strcmp(data, "exit") == 0) {
              std::cout << "Received exit command. Closing connection." << std::endl;
              free(data);
              break;
          }
  
      }
      client.~PIPEClient();
  }
  ```
- ```c++
  #include "PIPEServer.hpp"
  #include <iostream>
  #include <string.h>
  #include <unistd.h>
  
  int main() {
      std::cout << "./server.elf\n";
      std::cout.flush();
      PIPE::PIPEServer server = PIPE::PIPEServer("pipecommtest"); // Create server
      std::string data;
      int i = 0;
      while (true) {
          char* data = strdup(std::to_string(i).c_str());
          if (i >= 10000) {
              data = "exit";
          }
          std::cout << data << '\n';
          server.send(data); // Send data
          if (strcmp(data, "exit") == 0) {
              std::cout << "Received exit command. Closing connection." << std::endl;
              free(data);
              break;
          }
          i++;
          usleep(2);
      }
  }
  ```

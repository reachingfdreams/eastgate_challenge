//////////////////////////////////////////////////////////////////////////////////
//  @file  client.cpp
//  @brief Multithreaded client with POSIX sockets for Linux/Unix terminal
//         command line messenger
//
//  @brief main() function is responsible for creating socket, connecting to
//         server with specified ip address and port. And creating two
//         independent threads for sending and receiving messages from server
//         (in other words from other clients)
//
//  @brief SendThread() is responsible for picking message from stdin, encoding
//         the message, sending to server and decoding for printing in terminal
//         with appropriate formatting. Delivering message to appropriate client
//         is server responsibility.
//
//  @brief ReceiveThread() is responsible for receiving encoded message from
//         server, decoding the message, determining sender name of message and
//         private and public state of this message. After printing in terminal
//         with appropriate formatting. Also is responsible for receiving new
//         active clients notifications from server.
//
//  @brief message decoding and encoding is described in encode_decode.cpp and
//         .hpp files
//////////////////////////////////////////////////////////////////////////////////

#include <sys/socket.h> // Core socket functions and data structures
#include <arpa/inet.h>  // for manipulating IP addresses, for inet_addr()
#include <netdb.h>      // for gethostname()
#include <unistd.h>     // for close()
#include <strings.h>    // for bzero()
#include <sys/ioctl.h>  // for ioctl() - terminal sizes
#include <pthread.h>    // POSIX threcv_thread

#include <iostream>
#include <vector>
#include <string>
#include <cerrno>       // for errno
#include <cstring>      // for std::strerror()

#include "encode_decode.hpp"
#include "terminal_utils.hpp"

namespace {

const int kDefaultPort = 4375;

}  // namespace

// Declarations.
void* SendThread(void* arg);
void* ReceiveThread(void* arg);

int main (int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Syntax: ./client <server ip address>" << std::endl;
    return 0;
  }

  const char* ip_addres = argv[1];

  int socket_desc;
  struct sockaddr_in server_addr;

  // Create socket.
  socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_desc == -1) {
    std::cerr << "Cannot open socket with port#  " << kDefaultPort << std::endl;
    close(socket_desc);
    return 0;
  }

  bzero((char *)&server_addr, sizeof(server_addr));

  // Set the remote server information.
  server_addr.sin_addr.s_addr = inet_addr(ip_addres);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kDefaultPort);

  if (connect(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    std::cerr << "Cannot connect to the server with ip " << ip_addres << std::endl;
    close(socket_desc);
    return 0;
  }

  pthread_t send_thread_id;
  pthread_t recv_thread_id;

  pthread_create(&send_thread_id, NULL, SendThread, (void*)&socket_desc);
  pthread_detach(send_thread_id);

  pthread_create(&recv_thread_id, NULL, ReceiveThread, (void*)&socket_desc);
  pthread_join(recv_thread_id, NULL);

  pthread_exit(NULL);

  return 0;
}

void* SendThread(void* arg) {
  int socket_desc = *(int*)arg;

  char client_name[50];
  std::vector<std::string> dest_names;
  std::string message_info;
  std::string message;

  char input[1024];
  char input_encoded[1024];

  bzero(client_name, sizeof(client_name));

  std::cout << "Write your name: " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_CYAN;
  std::cin.getline(client_name, 50);
  std::cout << terminal::RESET_ALL;

  send(socket_desc, (char*)&client_name, sizeof(client_name), 0);

  std::cout << "Private message format is: @<destination name>: <message>" << std::endl;
  std::cout << "Public message format is: <message>" << std::endl;
  std::cout << "Start to write your first message !!!" << std::endl << std::endl;

  while (1) {
    // clear all data buffers
    bzero(input, sizeof(input));
    bzero(input_encoded, sizeof(input_encoded));
    dest_names.clear();
    message_info.clear();
    message.clear();

    std::cin.getline(input, sizeof(input));

    // Erase input from terminal.
    int input_size = std::cin.gcount();
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int erase_line_count = 0;
    if ((input_size % w.ws_col) != 0) {
      erase_line_count = (int)(input_size / w.ws_col) + 1;
    } else {
      erase_line_count = (int)(input_size / w.ws_col);
    }
    while (erase_line_count-- != 0) {
      std::cout << terminal::CursorPreviousLine() << terminal::EraseLine();
    }

    // If input is empty (Enter key is pressed) do nothing
    if (input_size == 1) {
      continue;
    }

    ed::EncodeMessage(input, input_encoded, sizeof(input_encoded));
    // ed::PrintEncodedMessage(input_encoded, sizeof(input_encoded));

    send(socket_desc,(char*)&input_encoded, sizeof(input_encoded), 0);

    // Decoding for input formatting in terminal.
    ed::DecodeMessage(input_encoded, sizeof(input_encoded), dest_names, message_info, message);

    // Formatting input (encoded -> decoded input message)
    // Private message is sent.
    if (!dest_names.empty()) {
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_BLUE;
      for (auto it = dest_names.begin(); it != dest_names.end(); ++it) {
        std::cout << *it;
        if (*it != dest_names.back()) {
          std::cout << ", ";
        }
      }
      std::cout << terminal::RESET_ALL << "<- " << terminal::TEXT_BOLD
                << terminal::TEXTCOLOR_YELLOW
                << message << terminal::RESET_ALL << std::endl;
    } else {  // Public message is send (if no dest_names)
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_YELLOW
                << message << terminal::RESET_ALL << std::endl;

    }
  }

  pthread_exit(NULL);
}

void* ReceiveThread(void* arg) {
  char input[1024];

  std::vector<std::string> names_from;
  std::string message_info;
  std::string message_from;

  int socket_desc = *(int*)arg;

  while (1) {
    names_from.clear();
    message_info.clear();
    message_from.clear();

    int rv = recv(socket_desc, input, sizeof(input), 0);
    if (rv == -1) {
      std::cerr << "Error reading from server: " << std::strerror(errno) << std::endl;
      break;
    } else if ( rv  == 0 ) {
      break;
    }

    // Decode the message.
    ed::DecodeMessage(input, sizeof(input), names_from, message_info, message_from);

    // Private incoming messages
    if (!message_info.compare("Private")) {
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_RED << names_from.front()
                << terminal::RESET_ALL <<  "-> " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_GREEN
                << message_from << terminal::RESET_ALL << std::endl;
      // Public incoming messages
    } else if (!message_info.compare("Public")) {
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_WHITE << names_from.front()
                << terminal::RESET_ALL <<  "-> " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_GREEN
                << message_from << terminal::RESET_ALL << std::endl;
      // New active clients notifications from server
    } else if (!message_info.compare("Server:online")) {
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_CYAN;
      for (auto it = names_from.begin(); it != names_from.end(); ++it) {
        std::cout << *it;
        if (*it != names_from.back()) {
          std:: cout << ", ";
        }
      }
      std::cout << terminal::RESET_ALL << terminal::TEXT_BOLD << terminal::TEXTCOLOR_WHITE
                << message_from << terminal::RESET_ALL << std::endl;
    }
  }

  std::cout << "Server is down! Closing ReceiveThread and socket..." << std::endl;

  close(socket_desc);
  pthread_exit(NULL);
}

# Multi-threaded TCP client-server
Linux/Unix terminal command line chat application with POSIX sockets and threads.

## Files
- README.md - the current readme file
- makefile - compiling and building automation with GNU make
- server.cpp - multi-threaded server with POSIX sockets for Linux/Unix terminal cmd
- client.cpp - multi-threaded client with POSIX sockets for Linux/Unix terminal cmd
- encode_decode.hpp/cpp	- functions declarations and definitions for encodeing and
  decoding messages for sending over TCP socket
- terminal_utils.hpp/cpp - designed for manipulating with linux terminal

## Requirements
- g++ compiler with C++11 standard
- Supporting POSIX standards
- GNU make

## Compilation:

- Compilation is done with GNU make, just type on terminal "make"

## Usage:

- on server side: ./server

- on client side: ./client  server_IPv4_address(ex: 127.0.0.1)


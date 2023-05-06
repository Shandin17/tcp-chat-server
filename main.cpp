#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Server.hpp"

/**
 * TCP server for simple gossip chat functionality
 * each message will be received by all connections
 */

 /**
 * Code structure:
 * 1.
 *  EventSelector - event sampling (select system call) (generalization of main cycle)
 *      [x] adding/deleting pointers to FdHandlers (add, remove)
  *     [x] keep objects of FdHandler in array
  *     [x] flag for stopping main cycle
  *     [x] main cycle
 *
 *  FdHandler - receiver of events (generalization of socket)
 *      keeps descriptor in constant field
  *      [x] descriptor getter
  *      [x] set descriptor in constructor
  *      [x] field that marks that FdHandler owns descriptor or not
  *      [x] descriptor indicators - wantRead (default true) wantWrite (default false)
  *      [x] handle - virtual method with params which indicate whether we ready to read or write
 *
 * 2.
 *  Session - socket bundled to client
  *     subclass of FdHandler. Will keep all "chat business logic" for user
  *     we can make all fields of this class private and make Server friendly to it
  *     [x] name field of the user
  *     [x] send messages
 *  Server - socket in listen mode here
  *     [x] will handle a listen socket and if new connection appears will create
  *     new object of Session
  *     [x] remove Session object if "end of file" situation appears
  *     [x] make constructor private and start server with static start method
  *     [x] send message to all parties in the chat that new user joined
  *     [x] send message that user left chat to all others
 */


static const char server_error[] = "Server init error";

int main(int argc, char** argv) {
     if (argc != 2) {
         fprintf(stderr, "Usage: serv <port>\n");
         return 1;
     }
     char *endptr;
     int port = strtol(argv[1], &endptr, 10);
     if (!*argv[1] || *endptr) {
         fprintf(stderr, "Invalid port number\n");
         return 2;
     }
     EventSelector *selector = new EventSelector();
     Server *serv = Server::start(selector, port);
     if (!serv) {
         perror(server_error);
     }
     selector->run();
    return 0;
}

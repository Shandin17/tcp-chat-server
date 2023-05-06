#ifndef SESSION_H
#define SESSION_H

#include "FdHandler.hpp"
#include "Server.hpp"
#include <netinet/in.h>

enum {
    max_buffer_size = 1023,
    max_connection_len = 16,
};

class Server;

class Session: FdHandler {
    friend class Server;
    Server *master;
    char buffer[max_buffer_size + 1];
    int bufUsed;
    bool ignoring;
    char *name;
    sockaddr_in userAddress;

    Session(Server *master, int fd, sockaddr_in addr);
    ~Session();
    void send(char *msg);
    virtual void handle(bool r, bool w);
    void readAndIgnore();
    void readAndCheck();
    void checkLines();
    void processLine(const char* str);
};

#endif

#ifndef SERVER_H
#define SERVER_H

#include "EventSelector.hpp"
#include "Session.hpp"

class Session;

class Server: public FdHandler {
    EventSelector* eventSelector;
    struct item {
        Session *s;
        item *next;
    };
    item *first;
    Server(EventSelector *sel, int fd);
public:
    static Server *start(EventSelector *sel, int port);
    void sendAll(char* msg, Session* excludedSession = 0);
    void remove(Session *s);
    ~Server();
private:
    virtual void handle(bool r, bool w);
};

#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Server.hpp"
#include "Session.hpp"

Server *Server::start(EventSelector *sel, int port) {
    struct sockaddr_in addr;
    int opt, ls, result;
    ls = socket(AF_INET, SOCK_STREAM, 0);
    if (ls == -1) {
        return 0;
    }
    opt = 1;
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    result = bind(ls, (struct sockaddr*) &addr, sizeof(addr));
    if (result == -1) {
        return 0;
    }
    result = listen(ls, max_connection_len);
    if (result == -1) {
        return 0;
    }
    return new Server(sel, ls);
}

Server::Server(EventSelector *sel, int fd): FdHandler(fd, true), eventSelector(sel), first(0) {
    eventSelector->addFd(this);
}

Server::~Server() {
    while (first) {
        item *tmp = first;
        first = first->next;
        eventSelector->removeFd(tmp->s);
        delete tmp->s;
        delete tmp;
    }
    eventSelector->removeFd(this);
}

void Server::handle(bool r, bool w) {
    if (!r) { // must not happen
        return;
    }
    int sd;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    sd = accept(getFd(), (struct sockaddr*) &addr, &len);
    if (sd == -1) {
        return;
    }
    item *p = new item;
    p->s = new Session(this, sd, addr);
    p->next = first;
    first = p;
    eventSelector->addFd(p->s);
}

void Server::remove(Session *s) {
    eventSelector->removeFd(s);
    item **p;
    for (p = &first; *p; p = &((*p)->next)) {
        if ((*p)->s == s) {
            item* tmp = *p;
            *p = (*p)->next;
            delete tmp->s;
            delete tmp;
            return;
        }
    }
}

void Server::sendAll(char *msg, Session *excludedSession) {
    item *p;
    for (p = first; p; p = p->next) {
        if (p->s != excludedSession) {
            p->s->send(msg);
        }
    }
}

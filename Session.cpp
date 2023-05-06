#include "Session.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

static const char welcome_msg[] = "Welcome to chat, your name is ";
static const char entered_msg[] = " has entered the chat";
static const char left_msg[] = " has left the chat";

void Session::send(char *msg) {
    write(getFd(), msg, strlen(msg));
}

Session::Session(Server *master, int fd, sockaddr_in addr)
    : FdHandler(fd, true), master(master), bufUsed(0), ignoring(false), name(0), userAddress(addr)  {
    send("Your name please: ");
}

Session::~Session() {
    if (name) {
        delete [] name;
    }
}

void Session::handle(bool r, bool w) {
    if (!r) {  // should never happen
        return;
    }
    if (bufUsed >=(int)sizeof(buffer)) {
        bufUsed = 0;
        ignoring = true;
    }
    if (ignoring) {
        readAndIgnore();
    } else {
        readAndCheck();
    }
}

/**
 * read and ignore till \n is received
 */
void Session::readAndIgnore() {
    int rc = read(getFd(), buffer, sizeof(buffer));
    if (rc < 1) {
        master->remove(this);
        return;
    }
    for (int i = 0; i < rc; i++) {
        if (buffer[i] == '\n') {
            int rest = rc - i - 1;
            if (rest > 0) {
                memmove(buffer, buffer + i + 1, rest);
            }
        }
    }
}

/**
* make another read and handle received lines if we got some
*/
void Session::readAndCheck() {
    int rc = read(getFd(), buffer, sizeof(buffer));
    if (rc < 1) {
        if (name) {
            int len = strlen(name);
            char *lmsg = new char[len + sizeof(left_msg) + 2];
            sprintf(lmsg, "%s%s\n", name, left_msg);
            master->sendAll(lmsg, this);
            delete [] lmsg;
        }
        master->remove(this);
        return;
    }
    bufUsed += rc;
    checkLines();
}

/**
* check if whole line has came and handle all such lines.
 * Leave in buffer only beginning of unfinished line
*/
void Session::checkLines() {
    if (bufUsed <= 0) {
        return;
    }
    for (int i = 0; i < bufUsed; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = 0;
            if (i > 0 && buffer[i-1] == '\r') {
                buffer[i - 1] = 0;
            }
            processLine(buffer);
            int rest = bufUsed - i - 1;
            memmove(buffer, buffer + i + 1, rest);
            bufUsed = rest;
            checkLines();
            return;
        }
    }
}

/**
* handle one single line
*/
void Session::processLine(const char *str) {
    int len = strlen(str);
    if (!name) {
        this->name = new char[len + 1];
        strcpy(name, str);
        char *wmsg = new char[len + sizeof(welcome_msg) + 2];
        sprintf(wmsg, "%s%s\n", welcome_msg, name);
        this->send(wmsg);
        char *emsg = new char[len + sizeof(entered_msg) + 2];
        sprintf(emsg, "%s%s\n", name, entered_msg);
        this->master->sendAll(emsg);
        delete [] wmsg;
        delete [] emsg;
        return;
    }
    int nl = strlen(name);
    char *msg = new char[nl + len + 5];
    sprintf(msg, "<%s> %s\n", name, str);
    master->sendAll(msg);
    delete [] msg;
}

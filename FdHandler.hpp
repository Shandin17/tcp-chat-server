#ifndef TCP_SERVER_FDHANDLER_HPP
#define TCP_SERVER_FDHANDLER_HPP

class FdHandler {
    int fd;
    bool ownFd;
public:
    FdHandler(int fd, bool ownFd = true): fd(fd), ownFd(ownFd) {};
    ~FdHandler();
    int getFd() { return fd; }
    bool isOwnFd() { return ownFd; }
    virtual bool wantRead() const { return true; }
    virtual bool wantWrite() const { return false; }
    virtual void handle(bool r, bool w) = 0;
};


#endif //TCP_SERVER_FDHANDLER_HPP

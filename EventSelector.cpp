#include "EventSelector.hpp"
#include "FdHandler.hpp"
#include <unistd.h>
#include <cerrno>

void EventSelector::addFd(FdHandler* fdHandler) {
    int fd = fdHandler->getFd();
    if (!fdArr) {
        fdArrSize = fd > 15 ? fd + 1 : 16;
        fdArr = new FdHandler*[fdArrSize];
        for (int i = 0; i < fdArrSize; i ++) {
            fdArr[i] = 0;
        }
        maxFd = -1;
    }
    if (fd >= fdArrSize) {
        FdHandler** tmpArr = new FdHandler*[fd + 1];
        for (int i = 0; i <= fd; i++) {
            tmpArr[i] = i < fdArrSize ? fdArr[i] : 0;
        }
        fdArrSize = fd + 1;
        delete [] fdArr;
        fdArr = tmpArr;
    }
    if (fd > maxFd)
        maxFd = fd;
    fdArr[fd] = fdHandler;
}

bool EventSelector::removeFd(FdHandler *fdHandler) {
    int fd = fdHandler->getFd();
    if (fd > fdArrSize || fdArr[fd] == fdHandler) {
        return false;
    }
    fdArr[fd] = 0;
    if (fd == maxFd) {
        while (!fdArr[maxFd] && maxFd >= 0) {
            maxFd--;
        }
    }
    return true;
}

void EventSelector::run() {
    quitCycle = false;
    do {
        fd_set rds, wds;
        FD_ZERO(&rds);
        FD_ZERO(&wds);
        for (int i = 0; i <= maxFd; i++) {
            FdHandler* fd = fdArr[i];
            if (fd) {
                if (fd->wantRead()) {
                    FD_SET(fd->getFd(), &rds);
                }
                if (fd->wantWrite()) {
                    FD_SET(fd->getFd(), &wds);
                }
            }
        }
        int result = select(maxFd + 1, &rds, &wds, 0, 0);
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        if (result > 0) {
            for (int i = 0; i <= maxFd; i++) {
                if (!fdArr[i]) {
                    continue;
                }
                bool r = FD_ISSET(i, &rds);
                bool w = FD_ISSET(i, &wds);
                if(r || w) {
                    fdArr[i]->handle(r, w);
                }
            }
        }
    } while (!quitCycle);
}

EventSelector::~EventSelector() {
    if (fdArrSize)
        delete [] fdArr;
}

#include "FdHandler.hpp"
#include "unistd.h"

FdHandler::~FdHandler() {
    if (ownFd)
        close(fd);
}

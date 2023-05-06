#ifndef TCP_SERVER_EVENTSELECTOR_HPP
#define TCP_SERVER_EVENTSELECTOR_HPP

#include "FdHandler.hpp"

class EventSelector {
    FdHandler **fdArr;
    int fdArrSize;
    bool quitCycle;
    int maxFd;
public:
    EventSelector(): fdArr(0), quitCycle(false) {};
    ~EventSelector();
    void addFd(FdHandler* fdHandler);
    bool removeFd(FdHandler* fdHandler);
    bool setQuitCycleFlag(bool newVal) { return quitCycle = newVal; }
    bool quitCycleFlag() { return quitCycle; }
    void run();
};


#endif //TCP_SERVER_EVENTSELECTOR_HPP

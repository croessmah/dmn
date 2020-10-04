#ifndef NETCOMMANDSQUEUE_H
#define NETCOMMANDSQUEUE_H

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

class NetCommandsQueue
{
public:
    NetCommandsQueue();
	void push(std::string source);
    bool waitAndPop(std::string & result);
    void sendEnd();
    void waitEmpty();
private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool endSignal_;
};


#endif // NETCOMMANDSQUEUE_H

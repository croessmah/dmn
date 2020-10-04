#include "NetCommandsQueue.h"


NetCommandsQueue::NetCommandsQueue() : endSignal_(false) {}


void NetCommandsQueue::push(std::string &&source)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.emplace(std::move(source));
    }
    cv_.notify_one();
}


bool NetCommandsQueue::waitAndPop(std::string &result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (endSignal_ && queue_.empty()) {
        return false;
    }

    if (queue_.empty()) {
        cv_.wait(lock, [this]{ return endSignal_ || !this->queue_.empty(); });
    }

    if (endSignal_ && queue_.empty()) {
        return false;
    }

    result = std::move(queue_.front());
    queue_.pop();
    lock.unlock();
    cv_.notify_one();
    return true;
}


void NetCommandsQueue::sendEnd()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        endSignal_ = true;
    }
    cv_.notify_all();
}


void NetCommandsQueue::waitEmpty()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!queue_.empty()) {
        cv_.wait(lock, [this]{ return this->queue_.empty(); });
    }
}

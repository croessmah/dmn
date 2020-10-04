#include "BackgroundFunctional.h"
#include "Executor.h"
#include "NetCommandsQueue.h"
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <iostream>

namespace
{

bool split(std::string & source, std::vector<char *> & result)
{
    result.clear();
    if (source.empty()) {
        return true;
    }
    char * chunk = strtok(&source.front(), "~");
    while (chunk) {
        result.push_back(chunk);
        chunk = strtok(nullptr, "~");
    }
    return true;
}


void backThreadFunction(NetCommandsQueue & queue, Executor & executor)
{
    std::string commandText;
    std::vector<char *> tokens;
    while (queue.waitAndPop(commandText)) {
		std::cout << "cmd text: " << commandText << std::endl;
        if (split(commandText, tokens)) {
            if (executor.execute(tokens)) {
                //todo: success
            } else {
                //todo: failure
            }
        } else {
            //todo: ошибка - нет данных?
        }
    }
}


}//internal linkage


unsigned runBackgroundWorkers(unsigned threadsCount, NetCommandsQueue & queue, Executor & executor)
{
    if (threadsCount == 0) {
        threadsCount = std::thread::hardware_concurrency();
        if (threadsCount == 0) {
            threadsCount = 1;
        }
        if (threadsCount > 1) {
            --threadsCount;
        }
    }
    for (unsigned i = 0; i < threadsCount; ++i) {
        std::thread(backThreadFunction, std::ref(queue), std::ref(executor)).detach();
    }
    return threadsCount;
}

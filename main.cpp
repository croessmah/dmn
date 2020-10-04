#include <iostream>
#include <sstream>
#include <chrono>
#include <vector>
#include <cstring>

#include "NetCommandsQueue.h"
#include "CommandsTable.h"
#include "Executor.h"
#include "BackgroundFunctional.h"

#include "CmdFuncs.h"
#include "Server.h"

#include "Socket.h"
#include <thread>

std::string generateOne(unsigned length)
{
    std::string result;
    result.reserve(length);
    while (length--) {
        result.push_back(rand() % 25 + 65);
    }
    return result;
}


std::string generateCommand(unsigned count)
{
    char const * funcs[] = {
        "cmd1", "cmd2", "cmd3", "cmd4"
    };
    std::string result;
    result += "authkey~";
    result += funcs[rand() % 4];
    for (unsigned i = 0; i < count; ++i) {
        result += "~" + generateOne(rand() % 10 + 1);
    }
    return result;
}


void connectTest()
{
	std::this_thread::sleep_for(std::chrono::seconds(5));
	for (int i = 0; i < 10000; ++i) {
		Socket sock;
		sock.create();
	}
}


int main()
{
    std::cout.sync_with_stdio(false);
    NetCommandsQueue ncc;
    CommandsTable tbl;
    tbl.reg("cmd1", std::make_unique<CmdFunc1>())
       .reg("cmd2", std::make_unique<CmdFunc2>())
       .reg("cmd3", std::make_unique<CmdFunc3>());
    Executor executor(tbl);

	std::thread(connectTest).detach();

    try {
		Server srv(ncc, 10000);
		runBackgroundWorkers(0, ncc, executor);
        srv.run();
		ncc.sendEnd();
		ncc.waitEmpty();
    } catch (std::exception & e) {
        std::cout << "error: " << e.what();
    }



    return 0;
}

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
#include <vector>

class CommandsTable;

class Executor
{
public:
    Executor(CommandsTable & table);
    bool execute(std::vector<char *> & args);
private:
    CommandsTable & table_;
};

#endif // EXECUTOR_H

#include "Executor.h"
#include "CommandsTable.h"
#include "ICommand.h"


Executor::Executor(CommandsTable & table):
    table_(table)
{
}


bool Executor::execute(std::vector<char *> & args)
{
    if (args.size() < 2) {
        //todo: ошибка - мало параметров
        return false;
    }
    if (ICommand * cmd = table_.get(args[1])) {
        return cmd->execute(args.data() + 2, args.size() - 2);
    }
    //todo: ошибка - неправильное имя команды
    return false;
}

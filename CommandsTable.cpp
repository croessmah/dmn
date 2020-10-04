#include <cassert>
#include "CommandsTable.h"
#include "ICommand.h"


CommandsTable & CommandsTable::reg(std::string const & name, CommandsTable::CommandPointer cmd)
{
    assert(cmd != nullptr && "null command");
    table_[name] = std::move(cmd);
    return *this;
}


ICommand * CommandsTable::get(std::string const & name) const
{
    auto it = table_.find(name);
    return it != table_.end() ? it->second.get() : nullptr;
}


CommandsTable::~CommandsTable()
{
}



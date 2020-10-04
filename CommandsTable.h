#ifndef COMMANDSTABLE_HPP
#define COMMANDSTABLE_HPP

#include <memory>
#include <string>
#include <map>

class ICommand;

class CommandsTable
{
public:
    using CommandPointer = std::unique_ptr<ICommand>;
    CommandsTable & reg(std::string const & name, CommandPointer cmd);
    ICommand * get(std::string const & name) const;
    ~CommandsTable();
private:
    std::map<std::string, CommandPointer> table_;
};



#endif // COMMANDSTABLE_HPP

#ifndef ICOMMAND_H
#define ICOMMAND_H
#include <vector>

class ICommand
{
public:
    virtual ~ICommand() {}
    virtual bool execute(char const * const * args, unsigned count) const = 0;
};

#endif // ICOMMAND_H

#ifndef CMDFUNC1_H
#define CMDFUNC1_H

#include "ICommand.h"

class CmdFunc1: public ICommand
{
public:
    bool execute(char const * const * args, unsigned count) const override;
};


class CmdFunc2: public ICommand
{
public:
    bool execute(char const * const * args, unsigned count) const override;
};


class CmdFunc3: public ICommand
{
public:
    bool execute(char const * const * args, unsigned count) const override;
};

#endif // CMDFUNC1_H

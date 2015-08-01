#include "function.h"
#include <iostream>

std::string Function::Determine(std::list<std::list<Atom>> argv)
{
    if (argv.size() != this->argn.size()) throw Exception("Function", "The number of function arguments do not match.");

    std::string result;
    for (auto it = body.begin(); it != body.end(); ++it)
    {
        auto value = argv.begin();
        auto key = this->argn.begin();
        while(value != argv.end())
        {
            if (*key == *it)
            {
                for (const auto& atom : *value) result = result + atom.Name() + " ";
                break;
            }
            ++key;
            ++value;
        }
        if (value == argv.end()) result = result + (*it).Name() + " ";
    }
    result.pop_back();
    return result;
}


size_t Function::Args() const
{
    return argn.size();
}

std::string Function::Name() const
{
    return name;
}

Function &Function::operator =(const Function &func)
{
    if (&func != this)
    {
        argn = func.argn;
        name = func.name;
        body = func.body;
        status = func.status;
    }
    return *this;
}

bool Function::operator ==(const Function &func) const
{
    return func.name == name;
}

bool Function::operator !=(const Function &func) const
{
    return func.name != name;
}


#include "Simplify/function.h"

Simplify::Function::Function(const Simplify::Function &func)
{
    this->argn = func.argn;
    this->body = func.body;
    this->name = func.name;
    this->status = func.status;
}

std::string Simplify::Function::Determine(std::list<std::list<Atom>> argv)
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

size_t Simplify::Function::Args() const
{
    return argn.size();
}

std::string Simplify::Function::Name() const
{
    return name;
}

Simplify::Function &Simplify::Function::operator =(const Simplify::Function &func)
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

bool Simplify::Function::operator ==(const Simplify::Function &func) const
{
    return func.name == name;
}

bool Simplify::Function::operator !=(const Simplify::Function &func) const
{
    return func.name != name;
}


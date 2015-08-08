#include "Simplify/simplify.h"

Simplify::Atom::Atom(const Simplify::Atom &atom)
{
    if (&atom != this)
    {
        name = atom.Name();
    }
}

const std::string &Simplify::Atom::Name() const
{
    return name;
}

Simplify::Atom &Simplify::Atom::operator =(const Simplify::Atom &atom)
{
    if (&atom != this)
    {
        name = atom.Name();
    }
    return *this;
}

bool Simplify::Atom::operator ==(const Simplify::Atom &atom) const
{
    return atom.Name() == name;
}

bool Simplify::Atom::operator !=(const Simplify::Atom &atom) const
{
    return !(atom == *this);
}

std::list<Simplify::Atom> Simplify::StringToList(std::string string)
{
    std::list<Simplify::Atom> result;
    std::string AtomName;
    for (const auto &ch : string)
        if (ch == ' ')
        {
            result.push_back(Simplify::Atom(AtomName));
            AtomName.clear();
        }
        else
        {
            AtomName.push_back(ch);
        }
    result.push_back(Simplify::Atom(AtomName));
    return result;
}

std::string Simplify::ListToString(std::list<Simplify::Atom> list)
{
    std::string result;
    for (const auto &atom : list) result = result + atom.Name() + " ";
    result.pop_back();
    return result;
}

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

Simplify::Segment Simplify::Segment::Clone()
{
    Segment clone;
    clone.args = args;
    clone.declaredFunc = declaredFunc;
    clone.declaringFunc = declaringFunc;
    clone.expectArgs = expectArgs;
    clone.out = out;
    clone.stage = stage;
    return clone;
}


#include "Simplify/atom.h"

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

#include "atom.h"

Atom::Atom(const Atom &atom)
{
    if (&atom != this)
    {
        name = atom.Name();
    }
}

const std::string &Atom::Name() const
{
    return name;
}

bool Atom::operator ==(const Atom &atom) const
{
    return atom.Name() == name;
}

bool Atom::operator !=(const Atom &atom) const
{
    return !(atom == *this);
}

std::list<Atom> StringToList(std::string string)
{
    std::list<Atom> result;
    std::string AtomName;
    for (const auto &ch : string)
        if (ch == ' ')
        {
            result.push_back(Atom(AtomName));
            AtomName.clear();
        }
        else
        {
            AtomName.push_back(ch);
        }
    result.push_back(Atom(AtomName));
    return result;
}


std::string ListToString(std::list<Atom> list)
{
    std::string result;
    for (const auto &atom : list) result = result + atom.Name() + " ";
    result.pop_back();
    return result;
}

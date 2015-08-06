#ifndef ATOM_H
#define ATOM_H

#include <string>
#include <list>

namespace Simplify
{
class Atom
{
private:
    std::string name;
public:
    Atom() = default;
    Atom(std::string name) : name(name) {}
    Atom(const Atom&);
    ~Atom() = default;
public:
    const std::string& Name() const;
public:
    Atom &operator = (const Atom &);
    bool operator == (const Atom &) const;
    bool operator != (const Atom &) const;
};

std::list<Atom> StringToList (std::string);
std::string ListToString (std::list<Atom>);
}

#endif // ATOM_H

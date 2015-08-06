#ifndef FUNCTION_H
#define FUNCTION_H

#include "Simplify/atom.h"
#include "exception.h"

namespace Simplify
{
enum class Status {let, letrec};
class Function
{
private:
    std::string name;
    std::list<Atom> argn;
    std::list<Atom> body;
    Status status;
public:
    Function() = default;
    Function(const Function &);
    Function(std::string name, std::list<Atom> argn, std::list<Atom> body, Status status) : name(name), argn(argn), body(body), status(status) {}
    ~Function() = default;
public:
    std::string Determine(std::list<std::list<Atom>>);
    size_t Args() const;
    std::string Name() const;
    Function& operator = (const Function&);
    bool operator == (const Function&) const;
    bool operator != (const Function&) const;
};
}

#endif // FUNCTION_H

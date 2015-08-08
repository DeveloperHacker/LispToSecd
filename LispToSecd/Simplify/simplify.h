#ifndef SIMPLIFY
#define SIMPLIFY

#include <string>
#include <list>
#include <stack>
#include "exception.h"

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

enum class Stage {Start, End, Declaring, ReadNameFunc, StartReadArgn, ReadArgn, ReadBodyFunc, ReadArgv};
struct DeclaringFunc
{
    std::string name;
    std::list<Atom> argn;
    std::list<Atom> body;
    Status status;
};
struct Segment
{
    Stage stage;
    std::list<Function> declaredFunc;
    DeclaringFunc declaringFunc;
    long long int expectArgs;
    long long int args;
    std::list<Atom> out;
    Segment Clone();
};
typedef std::stack<Segment> Buffer;
}

#endif // SIMPLIFY


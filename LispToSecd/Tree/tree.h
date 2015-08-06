#ifndef LEAF_H
#define LEAF_H

#include <list>
#include "Simplify/atom.h"

namespace Tree
{
using Simplify::Atom;
class Leaf;
typedef Leaf Root;

enum class Status {let, letrec};
class Function
{
private:
    std::string name;
    size_t args;
    Root body;
    Status status;
public:
    Function(const Function &);
    Function(std::string, size_t, Root, Status);
    ~Function();
};

class SExpression
{
private:
    Function *func;
    std::list<Leaf> leafs;
public:
    SExpression() = default;
    SExpression(Function *func, std::list<Leaf> leafs) : func(func), leafs(leafs) {}
    SExpression(const SExpression &);
    ~SExpression() = default;
};

class Leaf
{
private:
    std::list<Function> declaredFunc;
    SExpression sExpression;
public:
    Leaf() = default;
    Leaf(const Leaf &);
    Leaf(std::list<Function> funcs, SExpression exp) : declaredFunc(funcs), sExpression(exp) {}
    ~Leaf() = default;
public:
    Leaf &operator = (const Leaf &);
};
}

#endif // LEAF_H

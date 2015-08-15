#ifndef LEAF_H
#define LEAF_H

#include <list>
#include "Simplify/simplify.h"

namespace Tree
{
enum class Stage {StartDeclaring, Declaring, ReadHead, ReadTail};
using Simplify::Atom;
class Function;
struct SExpression;
struct Leaf;

struct SExpression
{
    Function *func;
    std::list<Leaf> leafs;

    SExpression() = default;
    SExpression(Function *func, std::list<Leaf> leafs) : func(func), leafs(leafs) {}
    SExpression(const SExpression &);
    ~SExpression() = default;

    SExpression &operator =(const SExpression&);
};

struct Leaf
{
    std::list<Function*> declaredFunc;
    SExpression sExpression;

    Leaf() = default;
    Leaf(const Leaf &);
    Leaf(std::list<Function*> funcs, SExpression exp) : declaredFunc(funcs), sExpression(exp) {}
    ~Leaf() = default;

    Leaf &operator = (const Leaf &);
};
struct Root
{
    Leaf leaf;
    std::list<Function*> BuildInfunctions;
};

enum class Status {let, letrec, argument};
class Function
{
private:
    std::string name;
    size_t args;
    Leaf body;
    Status status;
public:
    Function() = default;
    Function(const Function &);
    Function(std::string name, size_t args, Leaf body, Status status) :name(name), args(args), body(body), status(status) {}
    ~Function() = default;
public:
    Function &operator = (const Function &);
    bool operator == (const Function &) const;
    bool operator != (const Function &) const;
    Leaf *BodyPtr();
    size_t Args() const;
    std::string Name() const;
    Status GetStatus() const;

};

struct Segment
{
    Tree::Leaf *currentLeaf;
    Stage stage;
    Segment Clone();
};
typedef std::stack<Segment> Buffer;
}

#endif // LEAF_H

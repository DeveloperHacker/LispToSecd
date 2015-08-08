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
    std::list<Function> declaredFunc;
    SExpression sExpression;

    Leaf() = default;
    Leaf(const Leaf &);
    Leaf(std::list<Function> funcs, SExpression exp) : declaredFunc(funcs), sExpression(exp) {}
    ~Leaf() = default;

    Leaf &operator = (const Leaf &);
};
typedef Leaf Root;

enum class Status {let, letrec};
class Function
{
private:
    long long int id;
    std::string name;
    size_t args;
    Root body;
    Status status;
public:
    Function() = default;
    Function(const Function &);
    Function(long long int id, std::string name, size_t args, Root body, Status status) : id(id), name(name), args(args), body(body), status(status) {}
    ~Function() = default;
public:
    Function &operator = (const Function &);
    bool operator == (const Function &) const;
    bool operator != (const Function &) const;
    Root* BodyPtr();
    size_t ID() const;
    void SetID(size_t);
    std::string Name() const;
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

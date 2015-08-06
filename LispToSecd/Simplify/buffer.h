#ifndef BUFFER
#define BUFFER

#include <stack>
#include "Simplify/function.h"

namespace Simplify
{
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

#endif // BUFFER

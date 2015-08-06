#include "Simplify/buffer.h"

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

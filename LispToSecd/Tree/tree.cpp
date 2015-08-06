#include "Tree/tree.h"

Tree::SExpression::SExpression(const Tree::SExpression &exp)
{
    this->func = exp.func;
    this->leafs = exp.leafs;
}

Tree::Leaf::Leaf(const Leaf &leaf)
{
    this->sExpression = leaf.sExpression;
    this->declaredFunc = leaf.declaredFunc;
}

Tree::Leaf &Tree::Leaf::operator =(const Tree::Leaf &leaf)
{
    if (&leaf != this)
    {
        this->sExpression = leaf.sExpression;
        this->declaredFunc = leaf.declaredFunc;
    }

    return *this;
}

#include "Tree/tree.h"

Tree::SExpression::SExpression(const Tree::SExpression &exp)
{
    this->func = exp.func;
    this->leafs = exp.leafs;
}

Tree::SExpression &Tree::SExpression::operator =(const Tree::SExpression &exp)
{
    if (&exp != this)
    {
        this->func = exp.func;
        this->leafs = exp.leafs;
    }

    return *this;
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


Tree::Function::Function(const Tree::Function &func)
{
    this->name = func.name;
    this->body = func.body;
    this->args = func.args;
    this->status = func.status;
}

Tree::Function &Tree::Function::operator =(const Tree::Function &func)
{
    if (&func != this)
    {
        this->name = func.name;
        this->body = func.body;
        this->args = func.args;
        this->status = func.status;
    }

    return *this;
}

bool Tree::Function::operator ==(const Tree::Function &func) const
{
    return (func.name == this->name);
}

bool Tree::Function::operator !=(const Tree::Function &func) const
{
    return !(func == *this);
}

Tree::Leaf *Tree::Function::BodyPtr()
{
    return &body;
}

size_t Tree::Function::Args() const
{
    return args;
}

std::string Tree::Function::Name() const
{
    return name;
}

Tree::Status Tree::Function::GetStatus() const
{
    return status;
}

Tree::Segment Tree::Segment::Clone()
{
    Segment segment;
    segment.currentLeaf = currentLeaf;
    segment.stage = stage;
    return segment;
}

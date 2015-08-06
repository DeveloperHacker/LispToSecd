#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <iostream>

#include "exception.h"
#include "Simplify/buffer.h"
#include "Tree/tree.h"

class Parser
{
public:
    Parser() = default;
    ~Parser() = default;
public:
    static bool isNumber(const std::string &number);
    void execute(std::string);
private:
    std::list<Simplify::Atom> SimplifySourceFile(std::ifstream &) const;
    Tree::Root BuildTree(std::list<Simplify::Atom> &) const;
};

#endif // PARSER_H

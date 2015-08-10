#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include "Simplify/simplify.h"
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
    std::list<Simplify::Atom> Simplification(std::ifstream &) const;
    Tree::Root BuildTree(std::list<Simplify::Atom> &) const;
    void GenSecdCode(std::ofstream &, const Tree::Root &) const;
private:
    static std::string FomatOutSource(std::list<Simplify::Atom>);
    static void PrintFormatTree(Tree::Root &, std::ofstream &);
};

#endif // PARSER_H

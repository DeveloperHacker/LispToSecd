#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <iostream>

#include "exception.h"
#include "Simplify/buffer.h"

class Parser
{
public:
    Parser() = default;
    ~Parser() = default;
public:
    static bool isNumber(const std::string &number);
    void execute(std::string);
private:
    std::list<Atom> SimplifySourceFile(std::ifstream &);

};

#endif // PARSER_H

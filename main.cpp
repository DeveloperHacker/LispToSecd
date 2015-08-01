#include <iostream>

#include "parser.h"

int main(int args, char *argv[])
{
    Parser parser;
    try
    {
        if (args < 2) throw Exception("Initialization", "Source file is not found");
        parser.execute (argv[1]);
    }
    catch(Exception &exception)
    {
        std::cout << "Error: " << exception.name << ": " << exception.message << std::endl;
        return 1;
    }

    return 0;
}

#include <iostream>
#include "Parser/parser.h"

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
        system("pause");
        return 1;
    }
    system("pause");
    return 0;
}

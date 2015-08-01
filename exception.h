#ifndef EXCEPTION
#define EXCEPTION

#include <exception>
#include <string>

class Exception : public std::exception
{
public:
    std::string name;
    std::string message;
public:
    Exception() = default;
    Exception(const std::string &name, const std::string &message) : name(name), message(message) {}
    ~Exception() = default;
};

#endif // EXCEPTION


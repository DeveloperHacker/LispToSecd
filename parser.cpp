#include "parser.h"

bool Parser::isNumber(const std::string &number)
{
    size_t start_index;

    if (number.size() == 0) return false;
    start_index = (number[0] == '-') ? 1 : 0;
    for (auto i = start_index; i < number.size(); ++i)
        if (number[i] < '0' || number[i] > '9') return false;
    return true;
}

void Parser::execute(std::string fileNameIn)
{
    std::string fileNameOut;
    for (const auto& ch : fileNameIn)
    {
        fileNameOut.push_back (ch);
        if (ch == '.') break;
    }
    fileNameOut = fileNameOut + "secd";

    std::ifstream in(fileNameIn);
    std::ofstream out(fileNameOut);
    if (!in) throw Exception("Execute", "Source file is not found.");

    auto list = SimplifySourceFile (in);
    std::cout << ListToString(list) << std::endl;
    //Tree = BuildTree(list);
    //GenSecdCode(out, Tree);
}

std::list<Atom> Parser::SimplifySourceFile(std::ifstream &in)
{
    std::list<Function> declaredFunction;
    declaredFunction.push_back(Function("car", StringToList("A"), StringToList("car A"), Status::let));
    declaredFunction.push_back(Function("cdr", StringToList("A"), StringToList("cdr A"), Status::let));
    declaredFunction.push_back(Function("q", StringToList("A"),StringToList("q A"), Status::let));
    declaredFunction.push_back(Function("!", StringToList("A"), StringToList("! A"), Status::let));
    declaredFunction.push_back(Function("+", StringToList("A B"), StringToList("+ A B"), Status::let));
    declaredFunction.push_back(Function("-", StringToList("A B"), StringToList("- A B"), Status::let));
    declaredFunction.push_back(Function("*", StringToList("A B"), StringToList("* A B"), Status::let));
    declaredFunction.push_back(Function("/", StringToList("A B"), StringToList("/ A B"), Status::let));
    declaredFunction.push_back(Function("&", StringToList("A B"), StringToList("& A B"), Status::let));
    declaredFunction.push_back(Function("|", StringToList("A B"), StringToList("| A B"), Status::let));
    declaredFunction.push_back(Function(">", StringToList("A B"), StringToList("> A B"), Status::let));
    declaredFunction.push_back(Function("<", StringToList("A B"), StringToList("< A B"), Status::let));
    declaredFunction.push_back(Function("=", StringToList("A B"), StringToList("= A B"), Status::let));
    declaredFunction.push_back(Function("!=", StringToList("A B"), StringToList("!= A B"), Status::let));
    declaredFunction.push_back(Function(">=", StringToList("A B"), StringToList(">= A B"), Status::let));
    declaredFunction.push_back(Function("<=", StringToList("A B"), StringToList("<= A B"), Status::let));
    declaredFunction.push_back(Function("if",  StringToList("A B C"), StringToList("if A B C"), Status::let));    
    Stage stage = Stage::Start;
    DeclaringFunc declaringFunc;
    long long int expectArgs;
    long long int args;
    std::list<Atom> out;
    Buffer buffer;

    std::string token;
    bool comment = false;
    while (!in.eof())
    {
        token.clear();
        in >> token;
        if (token == "") continue;
        if (token == "//") { std::getline(in, token); continue; }
        if (token == "/*") { comment = true; continue; }
        if (token == "*/") { comment = false; continue; }
        if (comment == true) continue;

        if (stage == Stage::Start)
        {
            if (token == "(")
            {
                out.push_back(Atom(token));
                stage = Stage::Declaring;
            }
            else throw Exception("SimplifySourceFile", "At the beginning source file of expected token '('.");
        }
        else if (stage == Stage::End)
        {
            throw Exception("SimplifySourceFile", "At the end source file of expected token ')'.");
        }
        else if (stage == Stage::Declaring)
        {
            auto it = declaredFunction.begin();
            while (it != declaredFunction.end())
            {
                if (token == (*it).Name()) break;
                ++it;
            }
            if (it != declaredFunction.end())
            {
                expectArgs = (*it).Args();
                args = expectArgs;
                out.push_back(Atom((*it).Name()));
                stage = Stage::ReadArgv;
            }
            else if (token == "let")
            {
                declaringFunc.status = Status::let;
                stage = Stage::ReadNameFunc;
            }
            else if (token == "letrec")
            {
                declaringFunc.status = Status::letrec;
                stage = Stage::ReadNameFunc;
            }
            else if (isNumber(token))
            {
                expectArgs = 0;
                args = 0;
                out.push_back(token);
                stage = Stage::ReadArgv;
            }
            else throw Exception("SimplifySourceFile", "Token " + token + " was not declared in this scope.");
        }
        else if (stage == Stage::ReadNameFunc)
        {
            if (token == "(" || token == ")") throw Exception("SimplifySourceFile", "It is not possible to overload operator: '" + token + "'.");
            declaringFunc.name = token;
            stage = Stage::StartReadArgn;
        }
        else if (stage == Stage::StartReadArgn)
        {
            if (token != "(") throw Exception("SimplifySourceFile" ,"Expected format function declaration: let/letrec Name ( Args ) ( S-Expression )");
            declaringFunc.argn.clear();
            stage = Stage::ReadArgn;
        }
        else if (stage == Stage::ReadArgn)
        {
            if (token == ")")  stage = Stage::ReadBodyFunc;
            else declaringFunc.argn.push_back(Atom(token));
        }
        else if (stage == Stage::ReadBodyFunc)
        {
            if (token != "(") throw Exception("SimplifySourceFile", "Expected format function declaration: let/letrec Name ( Args ) ( S-Expression )");
            if (declaringFunc.status == Status::letrec)
            {
                auto body = declaringFunc.argn;
                body.push_front(Atom(declaringFunc.name));
                auto it = declaredFunction.begin();
                while (it != declaredFunction.end())
                {
                    if ((*it).Name() == declaringFunc.name)  break;
                    ++it;
                }
                if (it != declaredFunction.end()) *it = Function(declaringFunc.name, declaringFunc.argn, body, declaringFunc.status);
                else declaredFunction.push_back(Function(declaringFunc.name, declaringFunc.argn, body, declaringFunc.status));
            }
            Segment segment;
            segment.declaredFunc = declaredFunction;
            segment.declaringFunc = declaringFunc;
            segment.expectArgs = expectArgs;
            segment.args = args;
            segment.out = out;
            segment.stage = stage;
            buffer.push(segment.Clone());
            for (const auto &atom : declaringFunc.argn)
            {
                std::list<Atom> temp;
                temp.push_back(atom);
                declaredFunction.push_back(Function(atom.Name(), std::list<Atom>(), temp, Status::let));
            }
            out = std::list<Atom>();
            stage = Stage::Declaring;
        }
        else if (stage == Stage::ReadArgv)
        {
            auto it = declaredFunction.begin();
            while (it != declaredFunction.end())
            {
                if (token == (*it).Name()) break;
                ++it;
            }
            if (it != declaredFunction.end())
            {
                --expectArgs;
                if ((*it).Args() != 0) throw Exception("SimplifySourceFile", "Expected format of a function call: ( Name Args )");
                auto list = StringToList((*it).Determine(std::list<std::list<Atom>>()));
                if (list.size() != 1) out.push_back(Atom("("));
                for (const auto& atom : list) out.push_back(atom);
                if (list.size() != 1) out.push_back(Atom(")"));
            }
            else if (isNumber(token))
            {
                --expectArgs;
                out.push_back(Atom(token));
            }
            else if (token == ")")
            {
                if (expectArgs != 0) throw Exception("SimplifySourceFile", "Incorrect number of arguments.");
                std::list<std::list<Atom>> argv;
                std::list<Atom> temp;
                for (auto i = 0; i < args; ++i)
                {
                    if (out.back().Name() != ")")
                    {
                        temp.push_front(Atom(out.back()));
                        out.pop_back();
                        argv.push_front(std::list<Atom>(temp));
                    }
                    else
                    {
                        auto openBackets = 0ULL;
                        do
                        {
                            if (out.back().Name() == "(") --openBackets;
                            if (out.back().Name() == ")") ++openBackets;
                            temp.push_front(Atom(out.back()));
                            out.pop_back();
                        }
                        while (openBackets != 0);
                        argv.push_front(std::list<Atom>(temp));
                    }
                    temp.clear();
                }
                auto it = declaredFunction.begin();
                while (it != declaredFunction.end())
                {
                    if (out.back() == (*it).Name())
                    {
                        out.pop_back();
                        auto list = StringToList((*it).Determine(argv));
                        for (const auto& atom : list) out.push_back(atom);
                        break;
                    }
                    ++it;
                }
                if (buffer.size() == 0)
                {
                    out.push_back(Atom(token));
                    stage = Stage::End;
                }
                else
                {
                    auto segment = buffer.top();
                    declaredFunction = segment.declaredFunc;
                    declaringFunc = segment.declaringFunc;
                    expectArgs = segment.expectArgs;
                    args = segment.args;
                    auto oldOut = segment.out;
                    stage = segment.stage;
                    buffer.pop();
                    if (stage == Stage::ReadBodyFunc)
                    {
                        stage = Stage::Declaring;
                        declaringFunc.body = out;
                        if (declaringFunc.status == Status::let)
                        {
                            auto it = declaredFunction.begin();
                            while (it != declaredFunction.end())
                            {
                                if ((*it).Name() == declaringFunc.name)  break;
                                ++it;
                            }
                            if (it != declaredFunction.end()) *it = Function(declaringFunc.name, declaringFunc.argn, declaringFunc.body, declaringFunc.status);
                            else declaredFunction.push_back(Function(declaringFunc.name, declaringFunc.argn, declaringFunc.body, declaringFunc.status));
//                            declaredFunction.push_back(Function(declaringFunc.name, declaringFunc.argn, declaringFunc.body, declaringFunc.status));
                            out = std::list<Atom>(oldOut);
                        }
                        if (declaringFunc.status == Status::letrec)
                        {
                            out.clear();
                            out.push_back(Atom("("));
                            out.push_back(Atom("letrec"));
                            out.push_back(Atom(declaringFunc.name));
                            out.push_back(Atom("("));
                            for (const auto &atom : declaringFunc.argn) out.push_back(atom);
                            out.push_back(Atom(")"));
                            out.push_back(Atom("("));
                            for (const auto &atom : declaringFunc.body) out.push_back(atom);
                            out.push_back(Atom(")"));
                            oldOut.pop_front();
                            for (const auto &atom : oldOut) out.push_back(atom);
                        }
                    }
                    else
                    {
                        if (out.size() != 1) oldOut.push_back(Atom("("));
                        for (const auto &atom : out) oldOut.push_back(atom);
                        if (out.size() != 1) oldOut.push_back(Atom(")"));
                        out = std::list<Atom>(oldOut);
                    }
                }
            }
            else if (token == "(")
            {
                --expectArgs;
                Segment segment;
                segment.declaredFunc = declaredFunction;
                segment.declaringFunc = declaringFunc;
                segment.expectArgs = expectArgs;
                segment.args = args;
                segment.out = out;
                segment.stage = stage;
                buffer.push(segment.Clone());
                out = std::list<Atom>();
                stage = Stage::Declaring;
            }
            else throw Exception("SimplifySourceFile","Token " + token + " was not declared in this scope.");
        }
    }
    if (stage != Stage::End) throw Exception("SimplifySourceFile", "At the end source file of expected token ')'.");

    return out;
}

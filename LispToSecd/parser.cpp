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

    std::list<Simplify::Atom> list = SimplifySourceFile (in);
    std::cout << Simplify::ListToString(list) << std::endl;
    Tree::Root root = BuildTree(list);
    //GenSecdCode(out, Tree);
}

std::list< Simplify::Atom> Parser::SimplifySourceFile(std::ifstream &in) const
{
    std::list< Simplify::Function> declaredFunc;
    declaredFunc.push_back( Simplify::Function("car", Simplify::StringToList("A"), Simplify::StringToList("car A"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("cdr", Simplify::StringToList("A"), Simplify::StringToList("cdr A"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("q", Simplify::StringToList("A"), Simplify::StringToList("q A"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("!", Simplify::StringToList("A"), Simplify::StringToList("! A"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("+", Simplify::StringToList("A B"), Simplify::StringToList("+ A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("-", Simplify::StringToList("A B"), Simplify::StringToList("- A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("*", Simplify::StringToList("A B"), Simplify::StringToList("* A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("/", Simplify::StringToList("A B"), Simplify::StringToList("/ A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("&", Simplify::StringToList("A B"), Simplify::StringToList("& A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("|", Simplify::StringToList("A B"), Simplify::StringToList("| A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function(">", Simplify::StringToList("A B"), Simplify::StringToList("> A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("<", Simplify::StringToList("A B"), Simplify::StringToList("< A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("=", Simplify::StringToList("A B"), Simplify::StringToList("= A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("!=", Simplify::StringToList("A B"), Simplify::StringToList("!= A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function(">=", Simplify::StringToList("A B"), Simplify::StringToList(">= A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("<=", Simplify::StringToList("A B"), Simplify::StringToList("<= A B"), Simplify::Status::let));
    declaredFunc.push_back( Simplify::Function("if",  Simplify::StringToList("A B C"), Simplify::StringToList("if A B C"), Simplify::Status::let));
    Simplify::Stage stage = Simplify::Stage::Start;
    Simplify::DeclaringFunc declaringFunc;
    long long int expectArgs = 0;
    long long int args = 0;
    std::list< Simplify::Atom> out;
    Simplify::Buffer buffer;

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

        if (stage == Simplify::Stage::Start)
        {
            if (token == "(")
            {
                out.push_back( Simplify::Atom(token));
                stage = Simplify::Stage::Declaring;
            }
            else throw Exception("SimplifySourceFile", "At the beginning source file of expected token '('.");
        }
        else if (stage == Simplify::Stage::End)
        {
            throw Exception("SimplifySourceFile", "At the end source file of expected token ')'.");
        }
        else if (stage == Simplify::Stage::Declaring)
        {
            auto it = declaredFunc.begin();
            while (it != declaredFunc.end())
            {
                if (token == (*it).Name()) break;
                ++it;
            }
            if (it != declaredFunc.end())
            {
                expectArgs = (*it).Args();
                args = expectArgs;
                out.push_back( Simplify::Atom((*it).Name()));
                stage = Simplify::Stage::ReadArgv;
            }
            else if (token == "let")
            {
                declaringFunc.status = Simplify::Status::let;
                stage = Simplify::Stage::ReadNameFunc;
            }
            else if (token == "letrec")
            {
                declaringFunc.status = Simplify::Status::letrec;
                stage = Simplify::Stage::ReadNameFunc;
            }
            else if (isNumber(token))
            {
                expectArgs = 0;
                args = 0;
                out.push_back(token);
                stage = Simplify::Stage::ReadArgv;
            }
            else throw Exception("SimplifySourceFile", "Token " + token + " was not declared in this scope.");
        }
        else if (stage == Simplify::Stage::ReadNameFunc)
        {
            if (token == "(" || token == ")") throw Exception("SimplifySourceFile", "It is not possible to overload operator: '" + token + "'.");
            declaringFunc.name = token;
            stage = Simplify::Stage::StartReadArgn;
        }
        else if (stage == Simplify::Stage::StartReadArgn)
        {
            if (token != "(") throw Exception("SimplifySourceFile" ,"Expected format function declaration: let/letrec Name ( Args ) ( S-Expression )");
            declaringFunc.argn.clear();
            stage = Simplify::Stage::ReadArgn;
        }
        else if (stage == Simplify::Stage::ReadArgn)
        {
            if (token == ")")  stage = Simplify::Stage::ReadBodyFunc;
            else declaringFunc.argn.push_back( Simplify::Atom(token));
        }
        else if (stage == Simplify::Stage::ReadBodyFunc)
        {
            if (token != "(") throw Exception("SimplifySourceFile", "Expected format function declaration: let/letrec Name ( Args ) ( S-Expression )");
            if (declaringFunc.status == Simplify::Status::letrec)
            {
                auto body = declaringFunc.argn;
                body.push_front( Simplify::Atom(declaringFunc.name));
                auto it = declaredFunc.begin();
                while (it != declaredFunc.end())
                {
                    if ((*it).Name() == declaringFunc.name)  break;
                    ++it;
                }
                if (it != declaredFunc.end()) *it = Simplify::Function(declaringFunc.name, declaringFunc.argn, body, declaringFunc.status);
                else declaredFunc.push_back(Simplify::Function(declaringFunc.name, declaringFunc.argn, body, declaringFunc.status));
            }
            Simplify::Segment segment;
            segment.declaredFunc = declaredFunc;
            segment.declaringFunc = declaringFunc;
            segment.expectArgs = expectArgs;
            segment.args = args;
            segment.out = out;
            segment.stage = stage;
            buffer.push(segment.Clone());
            for (const auto &atom : declaringFunc.argn)
            {
                std::list< Simplify::Atom> temp;
                temp.push_back(atom);
                declaredFunc.push_back(Simplify::Function(atom.Name(), std::list< Simplify::Atom>(), temp, Simplify::Status::let));
            }
            out = std::list< Simplify::Atom>();
            stage = Simplify::Stage::Declaring;
        }
        else if (stage == Simplify::Stage::ReadArgv)
        {
            auto it = declaredFunc.begin();
            while (it != declaredFunc.end())
            {
                if (token == (*it).Name()) break;
                ++it;
            }
            if (it != declaredFunc.end())
            {
                --expectArgs;
                if ((*it).Args() != 0) throw Exception("SimplifySourceFile", "Expected format of a function call: ( Name Args )");
                auto list = Simplify::StringToList((*it).Determine(std::list<std::list< Simplify::Atom>>()));
                if (list.size() != 1) out.push_back( Simplify::Atom("("));
                for (const auto& atom : list) out.push_back(atom);
                if (list.size() != 1) out.push_back( Simplify::Atom(")"));
            }
            else if (isNumber(token))
            {
                --expectArgs;
                out.push_back( Simplify::Atom(token));
            }
            else if (token == ")")
            {
                if (expectArgs != 0) throw Exception("SimplifySourceFile", "Incorrect number of arguments.");
                std::list<std::list< Simplify::Atom>> argv;
                std::list< Simplify::Atom> temp;
                for (auto i = 0; i < args; ++i)
                {
                    if (out.back().Name() != ")")
                    {
                        temp.push_front( Simplify::Atom(out.back()));
                        out.pop_back();
                        argv.push_front(std::list< Simplify::Atom>(temp));
                    }
                    else
                    {
                        auto openBackets = 0ULL;
                        do
                        {
                            if (out.back().Name() == "(") --openBackets;
                            if (out.back().Name() == ")") ++openBackets;
                            temp.push_front( Simplify::Atom(out.back()));
                            out.pop_back();
                        }
                        while (openBackets != 0);
                        argv.push_front(std::list< Simplify::Atom>(temp));
                    }
                    temp.clear();
                }
                auto it = declaredFunc.begin();
                while (it != declaredFunc.end())
                {
                    if (out.back() == (*it).Name())
                    {
                        out.pop_back();
                        auto list = Simplify::StringToList((*it).Determine(argv));
                        for (const auto& atom : list) out.push_back(atom);
                        break;
                    }
                    ++it;
                }
                if (buffer.size() == 0)
                {
                    out.push_back( Simplify::Atom(token));
                    stage = Simplify::Stage::End;
                }
                else
                {
                    auto segment = buffer.top();
                    declaredFunc = segment.declaredFunc;
                    declaringFunc = segment.declaringFunc;
                    expectArgs = segment.expectArgs;
                    args = segment.args;
                    auto oldOut = segment.out;
                    stage = segment.stage;
                    buffer.pop();
                    if (stage == Simplify::Stage::ReadBodyFunc)
                    {
                        stage = Simplify::Stage::Declaring;
                        declaringFunc.body = out;
                        if (declaringFunc.status == Simplify::Status::let)
                        {
                            auto it = declaredFunc.begin();
                            while (it != declaredFunc.end())
                            {
                                if ((*it).Name() == declaringFunc.name)  break;
                                ++it;
                            }
                            if (it != declaredFunc.end()) *it = Simplify::Function(declaringFunc.name, declaringFunc.argn, declaringFunc.body, declaringFunc.status);
                            else declaredFunc.push_back(Simplify::Function(declaringFunc.name, declaringFunc.argn, declaringFunc.body, declaringFunc.status));
                            out = std::list< Simplify::Atom>(oldOut);
                        }
                        if (declaringFunc.status == Simplify::Status::letrec)
                        {
                            out.clear();
                            out.push_back( Simplify::Atom("("));
                            out.push_back( Simplify::Atom("letrec"));
                            out.push_back( Simplify::Atom(declaringFunc.name));
                            out.push_back( Simplify::Atom("("));
                            for (const auto &atom : declaringFunc.argn) out.push_back(atom);
                            out.push_back( Simplify::Atom(")"));
                            out.push_back( Simplify::Atom("("));
                            for (const auto &atom : declaringFunc.body) out.push_back(atom);
                            out.push_back( Simplify::Atom(")"));
                            oldOut.pop_front();
                            for (const auto &atom : oldOut) out.push_back(atom);
                        }
                    }
                    else
                    {
                        if (out.size() != 1) oldOut.push_back( Simplify::Atom("("));
                        for (const auto &atom : out) oldOut.push_back(atom);
                        if (out.size() != 1) oldOut.push_back( Simplify::Atom(")"));
                        out = std::list< Simplify::Atom>(oldOut);
                    }
                }
            }
            else if (token == "(")
            {
                --expectArgs;
                Simplify::Segment segment;
                segment.declaredFunc = declaredFunc;
                segment.declaringFunc = declaringFunc;
                segment.expectArgs = expectArgs;
                segment.args = args;
                segment.out = out;
                segment.stage = stage;
                buffer.push(segment.Clone());
                out = std::list< Simplify::Atom>();
                stage = Simplify::Stage::Declaring;
            }
            else throw Exception("SimplifySourceFile","Token " + token + " was not declared in this scope.");
        }
    }
    if (stage != Simplify::Stage::End) throw Exception("SimplifySourceFile", "At the end source file of expected token ')'.");

    return out;
}

Tree::Root Parser::BuildTree(std::list<Simplify::Atom> &) const
{
    Tree::Root root;

    return root;
}

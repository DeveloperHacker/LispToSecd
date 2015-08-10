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
    std::string fileNameLog;
    for (const auto& ch : fileNameIn)
    {
        fileNameOut.push_back (ch);
        if (ch == '.') break;
    }
    fileNameLog = fileNameOut + "log";
    fileNameOut = fileNameOut + "secd";

    std::ifstream in(fileNameIn);
    std::ofstream out(fileNameOut);
    std::ofstream log(fileNameLog);
    if (!in) throw Exception("Execute", "Source file is not found.");

    std::cout << "Simplification of the source file...\n";
    std::list<Simplify::Atom> list = Simplification(in);
    log << FomatOutSource(list) << "\n\n";

    std::cout << "Construction of the parse tree...\n";
    Tree::Root tree = BuildTree(list);
    PrintFormatTree(tree, log);

    std::cout << "Generation of the source code...\n";
    GenSecdCode(out, tree);    
}

std::list< Simplify::Atom> Parser::Simplification(std::ifstream &in) const
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
                out.push_back(Simplify::Atom(token));
                stage = Simplify::Stage::Declaring;
            }
            else throw Exception("Simplification", "At the beginning source file of expected token '('.");
        }
        else if (stage == Simplify::Stage::End)
        {
            throw Exception("Simplification", "At the end source file of expected token ')'.");
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
            else throw Exception("Simplification", "Token " + token + " was not declared in this scope.");
        }
        else if (stage == Simplify::Stage::ReadNameFunc)
        {
            if (token == "(" || token == ")") throw Exception("Simplification", "It is not possible to overload operator: '" + token + "'.");
            declaringFunc.name = token;
            stage = Simplify::Stage::StartReadArgn;
        }
        else if (stage == Simplify::Stage::StartReadArgn)
        {
            if (token != "(") throw Exception("Simplification" ,"Expected format function declaration: let/letrec Name ( Args ) ( S-Expression )");
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
            if (token != "(") throw Exception("Simplification", "Expected format function declaration: let/letrec Name ( Args ) ( S-Expression )");
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
                if ((*it).Args() != 0) throw Exception("Simplification", "Expected format of a function call: ( Name Args )");
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
                if (expectArgs != 0) throw Exception("Simplification", "Incorrect number of arguments.");
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
                    if (out.front().Name() == "letrec") out.push_front( Simplify::Atom("("));
                    out.push_back( Simplify::Atom(")"));
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
            else throw Exception("Simplification","Token " + token + " was not declared in this scope.");
        }
    }
    if (stage != Simplify::Stage::End) throw Exception("Simplification", "At the end source file of expected token ')'.");

    return out;
}

Tree::Root Parser::BuildTree(std::list<Tree::Atom> &in) const
{
    Tree::Root root;
    Tree::Leaf *currentLeaf = &root;
    size_t id = 1;
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "car", 1, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "cdr", 1, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "q", 1, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "!", 1, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "+", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "-", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "*", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "/", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "&", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "|", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, ">", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "<", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "=", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "!=", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, ">=", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "<=", 2, Tree::Root(), Tree::Status::let));
    currentLeaf->declaredFunc.push_back(new Tree::Function(id++, "if",  3, Tree::Root(), Tree::Status::let));

    Tree::Buffer buffer;
    Tree::Stage stage = Tree::Stage::StartDeclaring;
    auto it = in.begin();
    while (it != in.end())
    {
        if (stage == Tree::Stage::Declaring || stage == Tree::Stage::StartDeclaring)
        {
            if (stage == Tree::Stage::StartDeclaring) ++it;
            if ((*it).Name() == "letrec")
            {
                ++it;
                auto name = (*it).Name();
                ++it;
                ++it;
                std::list<Tree::Atom> argn;
                while ((*it).Name() != ")")
                {
                    argn.push_back(*it);
                    ++it;
                }
                ++it;
                currentLeaf->declaredFunc.push_back(
                            new Tree::Function(currentLeaf->declaredFunc.back()->ID() + 1, name, argn.size(), Tree::Root(), Tree::Status::letrec));
                Tree::Segment segment;
                segment.currentLeaf = currentLeaf;
                segment.stage = stage;
                buffer.push(segment);
                currentLeaf = currentLeaf->declaredFunc.back()->BodyPtr();
                currentLeaf->declaredFunc = buffer.top().currentLeaf->declaredFunc;
                for (const auto &atom : argn)
                    currentLeaf->declaredFunc.push_front(new Tree::Function(0, atom.Name(), 0, Tree::Root(), Tree::Status::let));
                size_t id = 1;
                for (auto &func : currentLeaf->declaredFunc) func->SetID(id++);
                stage = Tree::Stage::StartDeclaring;
            }
            else stage = Tree::Stage::ReadHead;
        }
        if (stage == Tree::Stage::ReadHead)
        {
            for (auto & func : currentLeaf->declaredFunc)
                if (func->Name() == (*it).Name())
                {
                    currentLeaf->sExpression.func = func;
                    break;
                }
            stage = Tree::Stage::ReadTail;
            ++it;
        }
        if (stage == Tree::Stage::ReadTail)
        {
            if ((*it).Name() == "(")
            {
                Tree::Segment segment;
                segment.currentLeaf = currentLeaf;
                segment.stage = stage;
                buffer.push(segment);
                currentLeaf->sExpression.leafs.push_back(Tree::Leaf());
                currentLeaf = &(currentLeaf->sExpression.leafs.back());
                currentLeaf->declaredFunc = buffer.top().currentLeaf->declaredFunc;
                stage = Tree::Stage::StartDeclaring;
            }
            else if ((*it).Name() == ")")
            {
                if (buffer.empty()) break;
                currentLeaf = buffer.top().currentLeaf;
                stage = buffer.top().stage;
                if (stage == Tree::Stage::StartDeclaring) stage = Tree::Stage::Declaring;
                buffer.pop();
                ++it;
            }
            else
            {
                Tree::Function *funcPtr = nullptr;
                for (auto & func : currentLeaf->declaredFunc)
                    if (func->Name() == (*it).Name())
                    {
                        funcPtr = func;
                        break;
                    }
                if (funcPtr == nullptr) funcPtr = new Tree::Function(0, (*it).Name(), 0, Tree::Root(), Tree::Status::let);
                currentLeaf->sExpression.leafs.push_back(Tree::Leaf(currentLeaf->declaredFunc, Tree::SExpression(funcPtr, std::list<Tree::Leaf>())));
                ++it;
            }
        }
    }

    return root;
}

void Parser::GenSecdCode(std::ofstream &out, const Tree::Root &tree) const
{

}

void Parser::PrintFormatTree(Tree::Root &tree, std::ofstream &out)
{
    Tree::Leaf *currentLeaf = &tree;
    std::queue<Tree::Leaf*> queue;
    std::set<Tree::Function*> funcs;
    queue.push(currentLeaf);
    out << "--------||--------||--------||--------\n";
     while (!queue.empty())
    {
        currentLeaf = queue.front();
        queue.pop();
        out << "Leaf: " << currentLeaf << " ;\n";
        out << "Declared functions: <" << currentLeaf->declaredFunc.size() << ">;\n";
        for (auto & func : currentLeaf->declaredFunc)
        {
            if (funcs.count(func) == 0)
            {
                funcs.insert(func);
                if (func->GetStatus() == Tree::Status::letrec) queue.push(func->BodyPtr());
            }
        }
        out << "SExpression: \n";
        out << "  Head: \n";
        out << "    Ptr: " << currentLeaf->sExpression.func << ";\n";
        out << "    Name: " << currentLeaf->sExpression.func->Name() << ";\n";
        out << "  Tail: ";
        if (currentLeaf->sExpression.leafs.size() == 0) out << "Nil;";
        out << "\n";
        for (auto &leaf : currentLeaf->sExpression.leafs)
        {
            queue.push(&leaf);
            out << "    Leaf: " << &leaf << ";\n";
        }
        out << "--------||--------||--------||--------\n";
    }
    out << "Funcions: <" << funcs.size() << ">;\n";
    for (auto & func : funcs)
    {
        out << "Function:\n";
        out << "  Ptr: " << func << ";\n";
        out << "  Name: " << func->Name() << ";\n";
        out << "  Body: " << func->BodyPtr() << ";\n";
        out << "--------||--------||--------||--------\n";
    }
}

std::string Parser::FomatOutSource(std::list<Simplify::Atom> in)
{
    std::string out;
    unsigned long long openBrackets = 0;
    Simplify::Atom prevAtom;
    int counterAtoms = -1;
    bool firstBracket = true;
    for (const auto & atom : in)
    {
        if (atom.Name() == "letrec" || atom.Name() == "let")
        {
            counterAtoms = 2;
        }
        else if (atom.Name() == "(")
        {
            if (counterAtoms != 0)
            {
                if (!firstBracket) out += "\n";
                else firstBracket = false;
                for (auto i = 0ULL; i < openBrackets; ++i) out += "  ";
            }
            ++openBrackets;
        }
        else if (atom.Name() == ")")
        {
            --openBrackets;
        }
        else if (prevAtom.Name() == ")")
        {
            out += "\n";
            for (auto i = 0ULL; i < openBrackets; ++i) out += "  ";
        }
        prevAtom = atom;
        out += atom.Name() + " ";
        counterAtoms -= (counterAtoms == -1) ? 0 : 1;
    }
    return out;
}


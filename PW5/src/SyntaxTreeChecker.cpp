#include "SyntaxTreeChecker.h"

using namespace SyntaxTree;

std::vector<std::pair<std::string, SyntaxTree::Type>> ID;
std::unordered_map<std::string, std::pair<SyntaxTree::Type, int>> FuncName;
std::stack<int> s;
// std::vector<std::vector<std::string>> FuncName(50);
std::stack<bool> StmtType; // ():true
std::unordered_map<std::string, std::unordered_map<int, SyntaxTree::Type>> ParamDef;
std::unordered_map<std::string, std::unordered_map<int, SyntaxTree::Type>> FuncCall;
SyntaxTree::Type FuncType;
int universe = 0;
// int flag_while, flag_if, flag_func;
bool SetCmp(std::unordered_map<int, SyntaxTree::Type> map1, std::unordered_map<int, SyntaxTree::Type> map2)
{
    if (map1.size() != map2.size())
    {
        // //std::cout << "is not equal..";
        return false;
    }
    for (auto iter1 : map1)
    {
        auto iter2 = map2.find(iter1.first);
        if (iter2 == map2.end())
        {
            return false;
        }
        else
        {
            //(iter1.second == SyntaxTree::Type::INT && iter2->second == SyntaxTree::Type::INT) || (iter1.second == SyntaxTree::Type::FLOAT && (iter2->second == SyntaxTree::Type::FLOAT || iter2->second == SyntaxTree::Type::INT))
            if (iter1.second != iter2->second)
            {
                // if (iter1.second == SyntaxTree::Type::INT)
                //     //std::cout << "it1int";
                // else
                //     //std::cout << "it2float";
                // if (iter2->second == SyntaxTree::Type::INT)
                //     //std::cout << "it2int";
                // else
                //     //std::cout << "it2float";
                if (iter1.second == SyntaxTree::Type::INT && iter2->second == SyntaxTree::Type::FLOAT)
                    return false;
            }
        }
    }
    return true;
}
bool checkFunc(std::string id)
{
    auto iter = FuncName.find(id);
    if (iter != FuncName.end())
    {
        return true;
    }
    // //std::cout << id;
    return false;
}
bool checkID_def(std::string id)
{
    // auto iter = ID.find(id);
    for (int i = 0; i < ID.size(); i++)
    {
        if (ID[i].first == id)
        {
            // found
            if (i >= s.top())
                return true;
        }
    }

    return false;
}
bool checkID_call(std::string id)
{
    // int index = distance(ID.begin(), iter) + 1;

    // for (auto iter = ID.begin(); iter != ID.end(); ++iter)
    // {
    //     //std::cout << iter->first;
    //     if (iter->first == "a")
    //     {
    //         //std::cout << iter->second.second << " " << s.top();
    //     }
    // }
    // //std::cout << std::endl;
    for (int i = 0; i < ID.size(); i++)
    {
        if (ID[i].first == id)
        {
            // found
            return true;
        }
    }
    return false;
}

SyntaxTree::Type lookup(std::string id)
{
    for (int i = ID.size() - 1; i >= 0; i--)
    {
        if (ID[i].first == id)
        {
            // found
            return ID[i].second;
        }
    }
}
SyntaxTree::Type lookupFunc(std::string id)
{
    auto iter = FuncName.find(id);
    return iter->second.first;
}
void Insert(SyntaxTree::Type type, std::string id)
{
    // std::pair<SyntaxTree::Type, std::string> it = {type, id};
    // std::pair<std::map<SyntaxTree::Type, std::string>::iterator, bool> ret;
    // ret = ID.insert(it);
    ID.insert(ID.end(), std::pair<std::string, SyntaxTree::Type>(id, type));
    // std::cout << id;
}
void SyntaxTreeChecker::visit(Assembly &node)
{
    s.push(ID.size());
    StmtType.push(false);
    for (auto def : node.global_defs)
    {
        def->accept(*this);
    }
    StmtType.pop();
    s.pop();
}

void SyntaxTreeChecker::visit(FuncDef &node)
{
    FuncType = node.ret_type;
    s.push(ID.size());
    StmtType.push(true);
    int sparam = ID.size();
    node.param_list->accept(*this);
    if (checkFunc(node.name))
    {
        err.error(node.loc, "FuncDuplicated");
        exit(int(ErrorType::FuncDuplicated));
    }
    FuncName[node.name] = std::make_pair(node.ret_type, FuncName.size() + 1);
    for (int i = sparam; i < ID.size(); i++)
    {
        ParamDef[node.name].insert(std::make_pair(i - sparam + 1, ID[i].second));
    }
    node.body->accept(*this);
    StmtType.pop();
}

void SyntaxTreeChecker::visit(BinaryExpr &node)
{
    node.lhs->accept(*this);
    bool lhs_int = this->Expr_int;
    // printf("%d", Expr_int);
    node.rhs->accept(*this);
    bool rhs_int = this->Expr_int;
    if (node.op == SyntaxTree::BinOp::MODULO)
    {
        if (!lhs_int || !rhs_int)
        {
            err.error(node.loc, "Operands of modulo should be integers.");
            exit(int(ErrorType::Modulo));
        }
    }
    this->Expr_int = lhs_int & rhs_int;
}

void SyntaxTreeChecker::visit(UnaryExpr &node)
{
    node.rhs->accept(*this);
}

void SyntaxTreeChecker::visit(LVal &node)
{
    for (auto index : node.array_index)
    {
        index->accept(*this);
    }
    if (!checkID_call(node.name))
    {
        // //std::cout << "un" << node.name;
        err.error(node.loc, "VarUnknown");
        exit(int(ErrorType::VarUnknown));
    }
    else
    {
        this->Expr_int = (lookup(node.name) == SyntaxTree::Type::INT);
        this->Expr_float = (lookup(node.name) == SyntaxTree::Type::FLOAT);
    }
}

void SyntaxTreeChecker::visit(Literal &node)
{
    this->Expr_int = (node.literal_type == SyntaxTree::Type::INT);
    this->Expr_float = (node.literal_type == SyntaxTree::Type::FLOAT);
    // if (Expr_float)
    // //std::cout << "float";
}

void SyntaxTreeChecker::visit(ReturnStmt &node)
{
    if (node.ret.get())
    {
        node.ret->accept(*this);
        // if (FuncType == SyntaxTree::Type::INT)
        // {
        //     //std::cout << "Intfunc";
        //     //std::cout << this->Expr_float;
        //     //std::cout << this->Expr_int;
        // }

        // if (FuncType == SyntaxTree::Type::INT && this->Expr_float)
        // {
        //     //std::cout << "no";
        //     err.error(node.loc, "FuncParams");
        //     exit(int(ErrorType::FuncParams));
        // }
    }
}

void SyntaxTreeChecker::visit(VarDef &node)
{
    // std::cout << "h1";
    for (auto length : node.array_length)
    {
        length->accept(*this);
    }
    if (node.is_inited)
    {
        node.initializers->accept(*this);
    }

    if (checkID_def(node.name))
    {
        err.error(node.loc, "VarDuplicated");
        exit(int(ErrorType::VarDuplicated));
    }
    Insert(node.btype, node.name);
}

void SyntaxTreeChecker::visit(AssignStmt &node)
{
    node.target->accept(*this);
    node.value->accept(*this);
}
void SyntaxTreeChecker::visit(FuncCallStmt &node)
{
    int paramid = 0;
    if (!checkFunc(node.name))
    {
        err.error(node.loc, "FuncUnknown");
        exit(int(ErrorType::FuncUnknown));
    }

    for (auto exp : node.params)
    {
        exp->accept(*this);
        paramid++;
        if (this->Expr_float && (ParamDef[node.name])[paramid] == SyntaxTree::Type::INT)
        {
            err.error(node.loc, "FuncParams");
            exit(int(ErrorType::FuncParams));
        }
    }
    if (paramid != ParamDef[node.name].size())
    {
        {
            err.error(node.loc, "FuncParams");
            exit(int(ErrorType::FuncParams));
        }
    }

    this->Expr_int = (lookupFunc(node.name) == SyntaxTree::Type::INT);
    this->Expr_float = (lookupFunc(node.name) == SyntaxTree::Type::FLOAT);
    // //std::cout << node.name << this->Expr_float;
    // //std::cout << "ok";
    // for (auto iter = ID.begin(); iter != ID.end(); ++iter)
    // {
    //     if (iter->second.second > sparam && iter->second.second <= ID.size())
    //     {
    //         FuncCall[node.name].insert(std::make_pair(iter->second.second, iter->second.first));
    //         //std::cout << iter->first << std::endl;
    //     }
    // }

    // if (!SetCmp(ParamDef[node.name], FuncCall[node.name]))
    // {
    //     //std::cout << "no";
    //     err.error(node.loc, "FuncParams");
    //     exit(int(ErrorType::FuncParams));
    // }
    // FuncCall.clear();
}
void SyntaxTreeChecker::visit(BlockStmt &node)
{
    // //std::cout << "c";
    // std::cout << "h2";
    bool push = (StmtType.top() == false);
    for (auto iter = ID.begin(); iter != ID.end(); ++iter)
    {
        // std::cout << iter->first;
    }
    // //std::cout << s.top() << std::endl;
    if (push)
        s.push(ID.size());
    // //std::cout << s.top() << std::endl;
    StmtType.push(false);
    for (auto stmt : node.body)
    {
        stmt->accept(*this);
    }
    int Topop = ID.size() - s.top();
    for (int i = 0; i < Topop; i++)
    {
        ID.pop_back();
        // //std::cout << iter->first << std::endl;
    }
    // for (auto iter = ID.begin(); iter != ID.end(); ++iter)
    // {
    //     //std::cout << iter->first << std::endl;
    // }
    // //std::cout << s.top();
    if (push)
        s.pop();
    StmtType.pop();
}
void SyntaxTreeChecker::visit(EmptyStmt &node) {}
void SyntaxTreeChecker::visit(SyntaxTree::ExprStmt &node)
{
    node.exp->accept(*this);
}
void SyntaxTreeChecker::visit(SyntaxTree::FuncParam &node)
{
    for (auto exp : node.array_index)
    {
        if (exp != nullptr)
            exp->accept(*this);
    }
    if (checkID_def(node.name))
    {
        err.error(node.loc, "VarDuplicated");
        exit(int(ErrorType::VarDuplicated));
    }
    else
    {
        Insert(node.param_type, node.name);
    }
}
void SyntaxTreeChecker::visit(SyntaxTree::FuncFParamList &node)
{
    for (auto param : node.params)
    {
        param->accept(*this);
    }
}
void SyntaxTreeChecker::visit(SyntaxTree::BinaryCondExpr &node)
{
    node.lhs->accept(*this);
    node.rhs->accept(*this);
}
void SyntaxTreeChecker::visit(SyntaxTree::UnaryCondExpr &node)
{
    node.rhs->accept(*this);
}
void SyntaxTreeChecker::visit(SyntaxTree::IfStmt &node)
{
    s.push(ID.size());
    // flag_if = 1;
    StmtType.push(true);
    node.cond_exp->accept(*this);
    if (dynamic_cast<BlockStmt *>(node.if_statement.get()))
    {
        node.if_statement->accept(*this);
    }
    else
    {

        node.if_statement->accept(*this);
    }
    if (node.else_statement != nullptr)
    {

        if (dynamic_cast<BlockStmt *>(node.else_statement.get()))
        {
            node.else_statement->accept(*this);
        }
        else
        {

            node.else_statement->accept(*this);
        }
    }
    // flag_if = 0;
    StmtType.pop();
}
void SyntaxTreeChecker::visit(SyntaxTree::WhileStmt &node)
{
    s.push(ID.size());
    // flag_while = 1;
    StmtType.push(true);
    node.cond_exp->accept(*this);
    if (dynamic_cast<BlockStmt *>(node.statement.get()))
    {
        node.statement->accept(*this);
    }
    else
    {
        node.statement->accept(*this);
    }
    // flag_while = 0;
    StmtType.pop();
}
void SyntaxTreeChecker::visit(SyntaxTree::BreakStmt &node)
{
}
void SyntaxTreeChecker::visit(SyntaxTree::ContinueStmt &node) {}

void SyntaxTreeChecker::visit(SyntaxTree::InitVal &node)
{
    if (node.isExp)
    {
        node.expr->accept(*this);
    }
    else
    {
        for (auto element : node.elementList)
        {
            element->accept(*this);
        }
    }
}
%skeleton "lalr1.cc" /* -*- c++ -*- */
%require "3.0"
%defines
//%define parser_class_name {sysyfParser}
%define api.parser.class {sysyfParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
#include <cmath>
#include "SyntaxTree.h"
class sysyfDriver;
}

// The parsing context.
%param { sysyfDriver& driver }

// Location tracking
%locations
%initial-action
{
// Initialize the initial location.
@$.begin.filename = @$.end.filename = &driver.file;
};

// Enable tracing and verbose errors (which may be wrong!)
%define parse.trace
%define parse.error verbose

// Parser needs to know about the driver:
%code
{
#include "sysyfDriver.h"
#define yylex driver.lexer.yylex
}

// Tokens:
%define api.token.prefix {TOK_}

%token END
/*********add your token here*********/
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token ASSIGN SEMICOLON
%token COMMA LPARENTHESE RPARENTHESE
%token LBRACE RBRACE
%token LBRACKET RBRACKET
%token NOT 
%token LT GT LTE GTE NEQ EQ
%token LAND LOR
%token INT RETURN VOID FLOAT CONST
%token IF ELSE WHILE CONTINUE BREAK
%token <std::string>IDENTIFIER
%token <int>INTCONST
%token <float>FLOATCONST

// Use variant-based semantic values: %type and %token expect genuine types
%type <SyntaxTree::Assembly*>CompUnit
%type <SyntaxTree::PtrList<SyntaxTree::GlobalDef>>GlobalDecl

/*********add semantic value definition here*********/

%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>Decl
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>ConstDecl
%type <SyntaxTree::VarDef*>ConstDef
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>ConstDefList
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDecl
%type <SyntaxTree::VarDef*>VarDef
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDefList
%type <SyntaxTree::Type>BType
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>L_ConstExp_R
%type <SyntaxTree::PtrList<SyntaxTree::InitVal>>ConstInitList
%type <SyntaxTree::InitVal*>ConstInitVal
%type <SyntaxTree::InitVal*>InitVal
%type <SyntaxTree::PtrList<SyntaxTree::InitVal>>InitValList
%type <SyntaxTree::FuncDef*>FuncDef
%type <SyntaxTree::FuncParam*>FuncFParam
%type <SyntaxTree::FuncFParamList*>FuncFParams
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>L_Exp_R
%type <SyntaxTree::BlockStmt*>Block
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItemList
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItem
%type <SyntaxTree::Stmt*>Stmt
%type <SyntaxTree::Stmt*>MatchedStmt
%type <SyntaxTree::Stmt*>OpenStmt
%type <SyntaxTree::Stmt*>Other
%type <SyntaxTree::ContinueStmt*>ContinueStmt
%type <SyntaxTree::BreakStmt*>BreakStmt
%type <SyntaxTree::ReturnStmt*>ReturnStmt
/* %type <SyntaxTree::IfStmt*>IfStmt
%type <SyntaxTree::WhileStmt*>WhileStmt */
%type <SyntaxTree::Expr*>Exp
%type <SyntaxTree::Expr*>Cond
%type <SyntaxTree::LVal*>LVal
%type <SyntaxTree::Expr*>PrimaryExp
%type <SyntaxTree::Literal*>Number
%type <SyntaxTree::Expr*>UnaryExp
%type <SyntaxTree::FuncCallStmt*>FuncCallStmt
%type <SyntaxTree::Expr*>UnaryCondExp
%type <SyntaxTree::Expr*>UnaryAddExp
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>FuncRParams
%type <SyntaxTree::Expr*>MulExp
%type <SyntaxTree::Expr*>AddExp
%type <SyntaxTree::Expr*>RelExp
%type <SyntaxTree::Expr*>EqExp
%type <SyntaxTree::Expr*>LAndExp
%type <SyntaxTree::Expr*>LOrExp
%type <SyntaxTree::Expr*>ConstExp
/* 
%type<SyntaxTree:: AssignStmt;


%type<SyntaxTree:: EmptyStmt; */



// No %destructors are needed, since memory will be reclaimed by the
// regular destructors.

// Grammar:
%start Begin 

%%
Begin: CompUnit END {
  // std::cout<<"begin\n";
    $1->loc = @$;
    driver.root = $1;
    return 0;
  }
  ;

CompUnit:CompUnit GlobalDecl{
    $1->global_defs.insert($1->global_defs.end(), $2.begin(), $2.end());
		$$=$1;
	} 
	| GlobalDecl{
    $$=new SyntaxTree::Assembly();
		$$->global_defs.insert($$->global_defs.end(), $1.begin(), $1.end());
    // std::cout<<"compunit\n";
	}
	;

/*********add other semantic symbol definition here*********/
GlobalDecl:Decl{
    $$=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | FuncDef{
    /*std::cout<<$1 -> name<<'\n';*/
    $$=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::GlobalDef>($1));
  }
  ;
Decl:ConstDecl{
    $$=$1;
  }
  | VarDecl{
    $$=$1;
  }
;
ConstDecl: CONST BType ConstDefList SEMICOLON
  {
    $$=$3;
    for (auto &node : $$) {
      node -> is_constant = true;
      node->btype = $2;
    }
  }
;
ConstDefList:ConstDef{
    $$=SyntaxTree::PtrList<SyntaxTree::VarDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($1));
    }
  | ConstDefList COMMA ConstDef{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($3));
    $$=$1;
    }
;
BType:INT{
    // std::cout<<"btype\n";
    $$=SyntaxTree::Type::INT;
    }
  | FLOAT{
    $$=SyntaxTree::Type::FLOAT;
    }
;
ConstDef:IDENTIFIER L_ConstExp_R ASSIGN ConstInitVal{
    $$=new SyntaxTree::VarDef();
    $$->name=$1;
    $$->is_inited = true;
    $$->array_length = $2;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($4);
    $$->loc = @$;
  } 
  | IDENTIFIER ASSIGN ConstInitVal{
    $$ = new SyntaxTree::VarDef();
    $$->name = $1;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($3);
    $$->is_inited = true;
    $$->loc = @$;
  }
;
L_ConstExp_R:LBRACKET ConstExp RBRACKET {
    $$=SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($2));
    }
  | 
  L_ConstExp_R LBRACKET ConstExp RBRACKET{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$=$1;
  }
;
ConstInitVal:ConstExp{
    $$ = new SyntaxTree::InitVal();
    $$-> expr = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$-> isExp = true;
    $$-> loc = @$;
    }
  | LBRACE ConstInitList RBRACE {
    $$ = new SyntaxTree::InitVal();
    $$-> isExp = false;
    $$-> elementList = $2;
    $$-> loc = @$;
    }
  | LBRACE RBRACE{
    $$ = new SyntaxTree::InitVal();
    $$-> isExp = false;
    $$-> loc = @$;
  }
    ;
ConstInitList:ConstInitVal {
    $$=SyntaxTree::PtrList<SyntaxTree::InitVal>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($1));
    }
  | ConstInitList COMMA ConstInitVal{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($3));
    $$=$1;
    }
    ;
VarDecl:BType VarDefList SEMICOLON{
      $$=$2;
      for (auto &node : $$) {
        node -> is_constant = false;
        node->btype = $1;
      }
      // std::cout<<"vardecl"<<'\n';
    }
    ;
VarDefList:VarDef {
      // std::cout << "vardeflist\n";
      $$=SyntaxTree::PtrList<SyntaxTree::VarDef>();
      $$.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($1));
    }
  | VarDefList COMMA VarDef{
      // std::cout << "vardeflist\n";
      $1.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($3));
      $$=$1;
    }
    ;
VarDef:IDENTIFIER {
      // std::cout << "vardef\n";
      $$=new SyntaxTree::VarDef();
      $$->name=$1;
      $$->is_inited = false;
      $$->loc = @$;
    }
  | IDENTIFIER ASSIGN InitVal{
      // std::cout << "vardef\n";
      $$ = new SyntaxTree::VarDef();
      $$->name = $1;
      $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($3);
      $$->is_inited = true;
      $$->loc = @$;
    }
  | IDENTIFIER L_ConstExp_R {
      // std::cout << "vardef\n";
      $$=new SyntaxTree::VarDef();
      $$->name=$1;
      $$->is_inited = false;
      $$ -> initializers = NULL;
      $$->array_length = $2;
      $$->loc = @$;
    }
  | IDENTIFIER L_ConstExp_R ASSIGN InitVal{
      // std::cout << "vardef\n";
      $$=new SyntaxTree::VarDef();
      $$->name=$1;
      $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($4);
      $$->is_inited = true;
      $$->array_length = $2;
      $$->loc = @$;
    }
    ;
InitVal: Exp {
    // std::cout << "initval\n";
    $$ = new SyntaxTree::InitVal();
    $$->isExp=true;
    $$->expr = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$->loc = @$;
    }
  | LBRACE InitValList RBRACE {
    // std::cout << "initval\n";
    $$ = new SyntaxTree::InitVal();
    $$->isExp=false;
    $$->elementList=$2;
    $$->loc = @$;
    }
  | LBRACE RBRACE {
    $$ = new SyntaxTree::InitVal();
    $$->isExp=false;
    $$->loc = @$;
    // std::cout << "initval\n";
  }
;
InitValList:InitVal{
    $$=SyntaxTree::PtrList<SyntaxTree::InitVal>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($1));
  }
  | InitValList COMMA InitVal{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($3));
    $$=$1;
  }
;
FuncDef:VOID IDENTIFIER LPARENTHESE FuncFParams RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type=SyntaxTree::Type::VOID;
    $$->name=$2;
    $$ -> param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>($4);
    $$->body=SyntaxTree::Ptr<SyntaxTree::BlockStmt>($6);
    $$->loc = @$;
    
}
| BType IDENTIFIER LPARENTHESE FuncFParams RPARENTHESE Block{
    // std::cout<<"funcdef\n";
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type=$1;
    $$->name=$2;
    $$ -> param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>($4);
    $$->body=SyntaxTree::Ptr<SyntaxTree::BlockStmt>($6);
    $$->loc = @$;
    
}
| VOID IDENTIFIER LPARENTHESE RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type=SyntaxTree::Type::VOID;
    $$->name=$2;
    $$->body=SyntaxTree::Ptr<SyntaxTree::BlockStmt>($5);
    auto temp = new SyntaxTree::FuncFParamList();
    $$ -> param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>(temp);
    $$->loc = @$;  
}
| BType IDENTIFIER LPARENTHESE RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type=$1;
    $$->name=$2;
    $$->body=SyntaxTree::Ptr<SyntaxTree::BlockStmt>($5);
    auto temp = new SyntaxTree::FuncFParamList();
    $$ -> param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>(temp);
    $$->loc = @$;
    // std::cout<<"funcdef\n";
}
;
FuncFParams: FuncFParams COMMA FuncFParam {
    $1->params.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($3));
    $$=$1;
  }
  | FuncFParam{
    $$=new SyntaxTree::FuncFParamList();
    $$->params.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($1));
  }
  ;

FuncFParam:BType IDENTIFIER {
    $$ = new SyntaxTree::FuncParam();
    $$ -> name = $2;
    $$ -> param_type = $1;
    $$ -> loc = @$;
  }
  | BType IDENTIFIER LBRACKET RBRACKET {
    $$ = new SyntaxTree::FuncParam();
    $$ -> name = $2 + "[]";
    $$ -> param_type = $1;
    $$ -> loc = @$;
  }
  | BType IDENTIFIER LBRACKET RBRACKET L_Exp_R{
    $$ = new SyntaxTree::FuncParam();
    $$ -> name = $2;
    $$ -> param_type = $1;
    $$ -> array_index = $5;
    $$ -> loc = @$;
  }
  ;
L_Exp_R:LBRACKET Exp RBRACKET {
    $$=SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($2));
    
  } 
  | L_Exp_R LBRACKET Exp RBRACKET{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$=$1;
  }
  ;
Block: LBRACE BlockItemList RBRACE{
    $$ = new SyntaxTree::BlockStmt();
    $$->body = $2;
    $$->loc = @$;
    // std::cout<<"block\n";
  }
;
BlockItemList:BlockItemList BlockItem{
    $1.insert($1.end(), $2.begin(), $2.end());
    $$ = $1;
  }
  |%empty{
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    // std::cout << "empty\n";
    }  
;
BlockItem : Decl {
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | 
  Stmt{
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Stmt>($1));
  }
    ;
Stmt:MatchedStmt{
    $$ = $1;
  }
  | OpenStmt
  {
    $$ = $1;
  }
;
MatchedStmt:  IF LPARENTHESE Cond RPARENTHESE MatchedStmt ELSE MatchedStmt{
    auto temp = new SyntaxTree::IfStmt();
    temp -> cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp -> if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    temp -> else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($7);
    $$ = temp;
    $$ -> loc = @$;
  }
  | Other{
    $$ = $1;
  }
  | WHILE LPARENTHESE Cond RPARENTHESE MatchedStmt{
    auto temp = new SyntaxTree::WhileStmt();
    temp -> cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp -> statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = temp;
    $$->loc = @$;  
  }
  ;
  OpenStmt: IF LPARENTHESE Cond RPARENTHESE Stmt{
    auto temp = new SyntaxTree::IfStmt();
    temp -> cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp -> if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    temp -> else_statement = NULL;
    $$ = temp;
    $$ -> loc = @$;
  } 
  | IF LPARENTHESE Cond RPARENTHESE MatchedStmt ELSE OpenStmt{
    auto temp = new SyntaxTree::IfStmt();
    temp -> cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp -> if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    temp -> else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($7);
    $$ = temp;
    $$ -> loc = @$;
  }
  | WHILE LPARENTHESE Cond RPARENTHESE OpenStmt{
    auto temp = new SyntaxTree::WhileStmt();
    temp -> cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp -> statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = temp;
    $$->loc = @$;  
  }
    ;

Other : LVal ASSIGN Exp SEMICOLON {
    auto temp = new SyntaxTree::AssignStmt();
    temp->target = SyntaxTree::Ptr<SyntaxTree::LVal>($1);
    temp->value = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
    }
  | SEMICOLON {
    $$ = new SyntaxTree::EmptyStmt();
    $$->loc = @$;    
    }
  | Exp SEMICOLON{
    auto temp = new SyntaxTree::ExprStmt();
    temp->exp = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$ = temp;
    $$->loc = @$;  
    }
  | Block{
    $$ = $1;
  }
  | BreakStmt{
    $$ = $1;  
  }
  | ContinueStmt{
    $$ = $1;
  }
  | ReturnStmt{
    $$ = $1;
  }
;


                   
BreakStmt:BREAK SEMICOLON{
    auto temp = new SyntaxTree::BreakStmt();
    $$ = temp;
    $$->loc = @$;
  }
    ;
ContinueStmt:CONTINUE SEMICOLON{
    auto temp = new SyntaxTree::ContinueStmt();
    $$ = temp;
    $$->loc = @$;
    }
  ;
ReturnStmt:RETURN SEMICOLON{
    
    auto temp = new SyntaxTree::ReturnStmt();
    temp->ret=NULL;
    $$ = temp;
    $$->loc = @$;
    // std::cout<<"return\n";
  }
  | RETURN Exp SEMICOLON{
    auto temp = new SyntaxTree::ReturnStmt();
    temp -> ret = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
    // std::cout<<"return\n";
  }
  ;
Exp : AddExp{
  $$=$1;
  // std::cout << "exp\n";
  }
  ;
Cond : LOrExp{
    $$=$1;
    }
;
LVal : IDENTIFIER L_Exp_R {
    $$ = new SyntaxTree::LVal();
    $$->name = $1;
    $$->array_index=$2;
    $$->loc = @$;
    }
  | IDENTIFIER{
    $$ = new SyntaxTree::LVal();
    $$->name = $1;
    $$->loc = @$;
    }
;
PrimaryExp : LPARENTHESE Exp RPARENTHESE {
    $$ = $2;
    }
  | LVal {
    $$ = $1;
    }
  | Number {
      /*std::cout << "primaryexp number\n";*/
    $$ = $1;
    }
;
Number : INTCONST {
    $$ = new SyntaxTree::Literal();
    $$->literal_type = SyntaxTree::Type::INT;
    $$->int_const = $1;
    $$->loc = @$;
    }
  | FLOATCONST{
    $$ = new SyntaxTree::Literal();
    $$->literal_type = SyntaxTree::Type::FLOAT;
    $$->float_const = $1;
    $$->loc = @$;
    }
;
/* %left PLUS MINUS;
%left MULTIPLY DIVIDE MODULO;
%precedence UPLUS UMINUS; */

UnaryExp : PrimaryExp {
    // std::cout << "Unaryexp\n";    
    $$=$1;
  }
  | FuncCallStmt{
    $$=$1;
  }
  | UnaryAddExp{
    $$=$1;
  }
  | UnaryCondExp{
    $$=$1;
  }
;
FuncCallStmt:IDENTIFIER LPARENTHESE FuncRParams RPARENTHESE {
    $$ = new SyntaxTree::FuncCallStmt();
    $$->name = $1;
    $$->params = $3;
    $$->loc = @$;
  }
  | IDENTIFIER LPARENTHESE RPARENTHESE {
    $$ = new SyntaxTree::FuncCallStmt();
    $$->name = $1;
    $$->loc = @$;
  }
;
UnaryCondExp: NOT UnaryExp {
      auto temp = new SyntaxTree::UnaryCondExpr();
      temp->op = SyntaxTree::UnaryCondOp::NOT;
      temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
      $$ = temp;
      $$->loc = @$;
      // std::cout << "UnaryCondExp\n";    
    }
;
UnaryAddExp:  PLUS UnaryExp {
    auto temp = new SyntaxTree::UnaryExpr();
    temp->op = SyntaxTree::UnaryOp::PLUS;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
    // std::cout << "UnaryAddExp\n"; 
    }
  |  MINUS UnaryExp {
    auto temp = new SyntaxTree::UnaryExpr();
    temp->op = SyntaxTree::UnaryOp::MINUS;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
    // std::cout << "UnaryAddExp\n"; 
    }
  ;

FuncRParams : Exp {
  $$=SyntaxTree::PtrList<SyntaxTree::Expr>();
  $$.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($1));
  }
  | FuncRParams COMMA Exp{
  $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
  $$=$1;
  } 
;

MulExp : UnaryExp {
    $$=$1;
      // std::cout << "Mulexp\n";   
    }
  | MulExp MULTIPLY UnaryExp{
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MULTIPLY;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;    
    // std::cout << "MulExp\n"; 
    }
  | MulExp DIVIDE UnaryExp{
    // std::cout << "MulExp\n";
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::DIVIDE;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
    }
  | MulExp MODULO UnaryExp{
    // std::cout << "MulExp\n";
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MODULO;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
    }
;
AddExp : MulExp {
    $$=$1;
  }
  | AddExp PLUS MulExp {
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::PLUS;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | AddExp MINUS MulExp {
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MINUS;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
;
RelExp : AddExp {
    $$=$1;
  }
  | RelExp LT AddExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LT;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | RelExp GT AddExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::GT;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | RelExp LTE AddExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LTE;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | RelExp GTE AddExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::GTE;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
;
EqExp : RelExp {
    $$=$1;
  }
  | EqExp EQ RelExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::EQ;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | EqExp NEQ RelExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::NEQ;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
;
LAndExp : EqExp {
    $$=$1;
  }
  | LAndExp LAND EqExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LAND;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
;
LOrExp : LAndExp {
    $$=$1;
  }
  | LOrExp LOR LAndExp {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LOR;
    temp->lhs= SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
ConstExp : AddExp{
  $$ = $1;
}
;
%%

// Register errors to the driver:
void yy::sysyfParser::error (const location_type& l,
                          const std::string& m)
{
    driver.error(l, m);
}

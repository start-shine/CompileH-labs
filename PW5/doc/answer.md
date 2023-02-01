# 问题1-1

## VarDef

```c++
struct VarDef : Stmt, GlobalDef
{
    bool is_constant; // 因为也存在常量定义
    Type btype; // 变量类型
    std::string name; // 变量名
    bool is_inited; // 是否初始化
    PtrList<Expr> array_length; // 如果定义的是数组，这部分定义了数组各维的长度
    Ptr<InitVal> initializers; // 用于初始化
    void accept(Visitor &visitor) final; // 通过accept()访问后续结点
};
```



## InitVal

```c++
//node for initial value
struct InitVal: Node{
    bool isExp; // 是否是一个表达式
    PtrList<InitVal> elementList; // 如果是给数组赋初值，这部分定义了需要用到的一系列初值
    Ptr<Expr> expr; // 具体表达式
    void accept(Visitor &visitor) final;
};
```



## LVal

```c++
// Expression like `ident` or `ident[exp]`.
// 左值
struct LVal : AddExpr
{
    std::string name; // 左值的名称
    PtrList<Expr> array_index; // 在左值是数组ident[exp]的情况下，表示数组的各个维度的Exp
    void accept(Visitor &visitor) final; 
};
```



## FuncDef

```c++
// Function definition.
struct FuncDef : GlobalDef
{
    Type ret_type; // 返回值类型
    Ptr<FuncFParamList> param_list; // 参数列表
    std::string name; // 函数名
    Ptr<BlockStmt> body; // 函数体部分
    void accept(Visitor &visitor) final;
};
```



## FuncFParamList

```c++
struct FuncFParamList : Node
{
    PtrList<FuncParam> params; // 一系列参数形成参数列表
    void accept(Visitor &visitor) final;
};
```



## FuncParam

```c++
struct FuncParam : Node
{
    std::string name; // 参数名
    Type param_type; // 参数类型
    PtrList<Expr> array_index; // 如果参数是数组，这部分是数组的各维下标
    void accept(Visitor &visitor) final;
};
```



## BlockStmt

```c++
// BlockStmt statement.
struct BlockStmt : Stmt
{
    PtrList<Stmt> body; // 主体
    void accept(Visitor &visitor) final;
};
```



## visit方法的实现特点

在visit方法中，每个结点中都调用了accept()函数，在accept()函数中，需要判断调用哪一个具体的visit方法，然后进一步调用。起到了递归和重定向的作用。



# 思考题

## 问题3-1 实验总结

### 实验中的困难与解决方案

- 困难
  1. 一开始写代码的时候没有学"作用域信息的保存"相关内容，以为是要用到visitor访问者模式的相关信息来保存作用域信息，思考很久也没有得到解决方案。
  2. 许多问题没有考虑到，导致更改代码中经常出现这边更改正确，那边又出现错误的情况。

- 解决方案
  1. 根据提示，参考了书中相关章节的内容
  2. 一开始根据出错的样例寻找错误之处，但由于对实验整体理解不够，导致仍然出现这种情况。在更改数次代码仍然有错误情况发生的时候，我就重新思考了总体的情况，然后理清楚思路再重新写。

### 实验难点

作用域信息的保存（这个理解了之后就不太难了）。

理解visitor访问者模式的具体用法和功能。

### 整体实现思路

首先要理解visit方法，在visit方法中，每个结点中都调用了accept()函数，在accept()函数中，需要判断调用哪一个具体的visit方法，然后进一步调用。

于是在实现过程中，参照样例，对每个非终结符，都判断其可能产生的语义错误：比如VarDef和FuncDef，可能会涉及变量/函数的重定义；LVal和FuncCallStmt会涉及到变量的使用或函数的调用，可能会涉及变量/函数未定义的错误；BinaryExpr是二元运算，可能会涉及模运算中类型不匹配的错误。

对于变量的重定义：维护一个“变量表”，保存变量名。每次定义新的变量要进行判断，如果之前没定义过，那么将其加入“变量表”中。注意这里的“没定义过”，是指当前作用域内的“没定义过”，关于作用域信息的保存。可以用栈进行。具体而言，在一个栈中保存当前block的起点位置，那么从该栈顶位置，到变量表的最后一位，即是当前作用域内的变量。具体细节同书中相关章节所述，在此不再赘述。当一个block结束时，需要弹出栈顶的值，以及删除变量表中所有该block内的值。使得变量表始终维护当前整个作用域内的变量，栈顶始终标识当前block变量的起始位置。

对于函数的重定义：维护一个“函数表”，保存函数名。每次定义新的函数时要进行判断，如果函数名不在“函数表”中，将其加入函数表。

对于变量的未定义：需要使用变量的时候，在“变量表”里查找，如果没有定义，那么报错。主要这里的“没有定义”，是指在**当前**整个作用域中的没有定义，这一点与“变量重定义”不同。

对于函数的未定义：调用函数时，在“函数表”里查找，如果没有定义，那么报错。

对于模运算的类型不匹配：对于`SyntaxTreeChecker`类，维护`Expr_int`和`Expr_float`，表示当前结点（如果是表达式类型）是`int`类型还是`float`类型。该值随着每个结点调用accept()函数自叶子节点向上传递。每次进行模运算的时候，需要两边的操作数的这两个属性进行判断。

对于参数类型和个数的不匹配：对每个“函数表”中的条目，增加关于参数信息的保存。对于每个“函数名”，保存其需要传递的参数类型：<参数类型1，参数类型2，...>。调用函数的时候，对传递的参数进行一一匹配，遇到不匹配的情况，或是参数个数不一样的情况，则报错。



## 问题3-2

1. 变量应该先初始化。如果初始化的表达式中出现了该变量，应该报错，因为它本身还没有一个值，不应该出现在初始化表达式中。

   但实际上`int a = a;`这样的语句可以通过编译，让我感到比较疑惑。

2. 应该先把函数名加入符号表，因为函数可能会有递归调用，在处理函数体时会用到该函数名。



## 问题3-3

1. 在生成的语法树的每个`Expr`结点都要给`Expr_int`赋值，但大多数情况下我们不需要这个值，造成了冗余和浪费。
2. 每次都要重新扫描一遍代码，生成语法树然后判断错误，也会使情况变得复杂，尤其是代码量巨大的时候，扫描本身会花费很多时间。



## 问题3-4

将当前已经发现的优先级最高的错误都记录，但暂时不报错；每检查到一个新的错误，就更新这个记录。如果优先级更高，那么记录新的错误及其优先级，否则保留原纪录。到结尾处输出优先级最高的错误记录。


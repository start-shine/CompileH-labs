#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG                                             // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl; // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
    ConstantInt::get(num, module)

#define CONST_FP(num) \
    ConstantFloat::get(num, module) // 得到常数值的表示,方便后面多次用到

int main()
{
    // auto module = new Module("SysY code"); // module name是什么无关紧要
    // auto builder = new IRStmtBuilder(nullptr, module);
    auto irbuilder = new IRBuilder();
    auto irmodule = irbuilder->getModule();
    auto module = irmodule.get();
    auto scope = irbuilder->getScope();
    auto builder = irbuilder->getStmtBuilder();
    Type *Int32Type = Type::get_int32_type(module);
    Type *Int32Type_ptr = Type::get_int32_ptr_type(module);
    Type *VoidType = Type::get_void_type(module);
    // exchange
    std::vector<Type *> Ints(2, Int32Type_ptr);
    auto exchangeFunTy = FunctionType::get(VoidType, Ints);
    auto exchangeFun = Function::create(exchangeFunTy, "exchange", module);
    auto bb = BasicBlock::create(module, "entry", exchangeFun);

    builder->set_insert_point(bb); // 一个BB的开始,将当前插入指令点的位置设在bb

    // auto retAlloca = builder->create_alloca(VoidType); // 在内存中分配返回值的位置
    auto aAlloca = builder->create_alloca(Int32Type); // 在内存中分配参数n的位置
    auto bAlloca = builder->create_alloca(Int32Type);
    std::vector<Value *> args; // 获取exchange函数的形参,通过Function中的iterator
    for (auto arg = exchangeFun->arg_begin(); arg != exchangeFun->arg_end(); arg++)
    {
        args.push_back(*arg); // * 号运算符是从迭代器中取出迭代器当前指向的元素
    }
    auto aParaLoad = builder->create_load(args[0]);
    auto tmpAlloca = builder->create_alloca(Int32Type);
    builder->create_store(aParaLoad, tmpAlloca);

    auto bParaLoad = builder->create_load(args[1]);
    builder->create_store(bParaLoad, args[0]);

    auto tmpLoad = builder->create_load(tmpAlloca);
    builder->create_store(tmpLoad, args[1]);

    builder->create_void_ret();
    // partition
    std::vector<Type *> Partitions;
    Partitions.push_back(Int32Type_ptr);
    Partitions.push_back(Int32Type);
    Partitions.push_back(Int32Type);
    auto partitionFunTy = FunctionType::get(Int32Type, Partitions);
    auto partitionFun = Function::create(partitionFunTy, "partition", module);
    bb = BasicBlock::create(module, "entry", partitionFun);
    builder->set_insert_point(bb);
    auto retAlloca = builder->create_alloca(Int32Type); // 在内存中分配返回值的位置
    args.clear();
    for (auto arg = partitionFun->arg_begin(); arg != partitionFun->arg_end(); arg++)
    {
        args.push_back(*arg); // * 号运算符是从迭代器中取出迭代器当前指向的元素
    }
    auto rAlloca = builder->create_alloca(Int32Type);
    builder->create_store(args[2], rAlloca);
    auto pAlloca = builder->create_alloca(Int32Type);
    builder->create_store(args[1], pAlloca);
    auto rLoad = builder->create_load(rAlloca);
    auto arGep = builder->create_gep(args[0], {rLoad});
    auto arLoad = builder->create_load(arGep);
    auto keyAlloca = builder->create_alloca(Int32Type);
    builder->create_store(arLoad, keyAlloca);
    auto iAlloca = builder->create_alloca(Int32Type);
    auto pLoad = builder->create_load(pAlloca);
    auto sub = builder->create_isub(pLoad, CONST_INT(1));
    builder->create_store(sub, iAlloca);
    // j = p
    auto jAlloca = builder->create_alloca(Int32Type);
    builder->create_store(pLoad, jAlloca);
    auto jLoad = builder->create_load(jAlloca);
    auto whileBB = BasicBlock::create(module, "whileBB_entry", partitionFun);
    builder->create_br(whileBB);
    // start while
    builder->set_insert_point(whileBB);
    jLoad = builder->create_load(jAlloca);
    rLoad = builder->create_load(rAlloca);
    auto icmp = builder->create_icmp_lt(jLoad, rLoad);
    auto trueBB = BasicBlock::create(module, "trueBB_while", partitionFun);   // true分支
    auto falseBB = BasicBlock::create(module, "falseBB_while", partitionFun); // false分支
    builder->create_cond_br(icmp, trueBB, falseBB);
    builder->set_insert_point(trueBB);
    // start if
    auto ajGep = builder->create_gep(args[0], {jLoad});
    auto ajLoad = builder->create_load(ajGep);
    auto keyLoad = builder->create_load(keyAlloca);
    icmp = builder->create_icmp_le(ajLoad, keyLoad);
    auto trueBB2 = BasicBlock::create(module, "trueBB_if", partitionFun);   // true分支
    auto falseBB2 = BasicBlock::create(module, "falseBB_if", partitionFun); // false分支
    builder->create_cond_br(icmp, trueBB2, falseBB2);
    // if true
    builder->set_insert_point(trueBB2);
    auto iLoad = builder->create_load(iAlloca);
    auto add = builder->create_iadd(iLoad, CONST_INT(1));
    builder->create_store(add, iAlloca);
    iLoad = builder->create_load(iAlloca);
    auto aiGep = builder->create_gep(args[0], {iLoad});
    ajGep = builder->create_gep(args[0], {jLoad});
    // exchange
    builder->create_call(exchangeFun, {aiGep, ajGep});
    builder->create_br(falseBB2);
    // if false
    builder->set_insert_point(falseBB2);
    // j++
    jLoad = builder->create_load(jAlloca);
    add = builder->create_iadd(jLoad, CONST_INT(1));
    builder->create_store(add, jAlloca);
    // while again
    builder->create_br(whileBB);
    // end while
    builder->set_insert_point(falseBB);
    iLoad = builder->create_load(iAlloca);
    add = builder->create_iadd(iLoad, CONST_INT(1));
    auto ai1Gep = builder->create_gep(args[0], {add});
    rLoad = builder->create_load(rAlloca);
    arGep = builder->create_gep(args[0], {rLoad});

    builder->create_call(exchangeFun, {ai1Gep, arGep});
    // ret
    iLoad = builder->create_load(iAlloca);
    add = builder->create_iadd(iLoad, CONST_INT(1));
    builder->create_store(add, retAlloca);
    auto retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);

    // quicksort
    std::vector<Type *> Quicksorts;
    Quicksorts.push_back(Int32Type_ptr);
    Quicksorts.push_back(Int32Type);
    Quicksorts.push_back(Int32Type);
    auto QuicksortFunTy = FunctionType::get(VoidType, Quicksorts);
    auto QuicksortFun = Function::create(QuicksortFunTy, "Quicksort", module);
    bb = BasicBlock::create(module, "entry", QuicksortFun);
    builder->set_insert_point(bb);
    retAlloca = builder->create_alloca(Int32Type); // 在内存中分配返回值的位置
    args.clear();
    for (auto arg = QuicksortFun->arg_begin(); arg != QuicksortFun->arg_end(); arg++)
    {
        args.push_back(*arg); // * 号运算符是从迭代器中取出迭代器当前指向的元素
    }
    auto posAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), posAlloca);
    icmp = builder->create_icmp_lt(args[1], args[2]);
    trueBB = BasicBlock::create(module, "trueBB_if", QuicksortFun);   // true分支
    falseBB = BasicBlock::create(module, "falseBB_if", QuicksortFun); // false分支
    builder->create_cond_br(icmp, trueBB, falseBB);
    // if true
    builder->set_insert_point(trueBB);
    auto position = builder->create_call(partitionFun, {args[0], args[1], args[2]});
    builder->create_store(position, posAlloca);
    auto pos = builder->create_load(posAlloca);
    sub = builder->create_isub(pos, CONST_INT(1));
    builder->create_call(QuicksortFun, {args[0], args[1], sub});
    pos = builder->create_load(posAlloca);
    add = builder->create_iadd(pos, CONST_INT(1));
    builder->create_call(QuicksortFun, {args[0], add, args[2]});
    builder->create_br(falseBB);
    builder->set_insert_point(falseBB);
    builder->create_void_ret();

    // print
    std::vector<Type *> Prints;
    Prints.push_back(Int32Type_ptr);
    Prints.push_back(Int32Type);
    auto PrintFunTy = FunctionType::get(VoidType, Prints);
    auto PrintFun = Function::create(PrintFunTy, "Print", module);
    bb = BasicBlock::create(module, "entry", PrintFun);
    builder->set_insert_point(bb);
    retAlloca = builder->create_alloca(Int32Type); // 在内存中分配返回值的位置
    args.clear();
    for (auto arg = PrintFun->arg_begin(); arg != PrintFun->arg_end(); arg++)
    {
        args.push_back(*arg); // * 号运算符是从迭代器中取出迭代器当前指向的元素
    }
    iAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), iAlloca);
    whileBB = BasicBlock::create(module, "whileBB_entry", PrintFun);
    builder->create_br(whileBB);
    // start while
    builder->set_insert_point(whileBB);
    iLoad = builder->create_load(iAlloca);
    icmp = builder->create_icmp_lt(iLoad, args[1]);
    trueBB = BasicBlock::create(module, "trueBB_while", PrintFun);   // true分支
    falseBB = BasicBlock::create(module, "falseBB_while", PrintFun); // false分支
    builder->create_cond_br(icmp, trueBB, falseBB);
    builder->set_insert_point(trueBB);
    // putint
    iLoad = builder->create_load(iAlloca);
    aiGep = builder->create_gep(args[0], {iLoad});
    auto aiLoad = builder->create_load(aiGep);
    auto putint = scope->find("putint", true);
    builder->create_call(putint, {aiLoad});
    auto putch = scope->find("putch", true);
    builder->create_call(putch, {CONST_INT(10)});
    // i++
    iLoad = builder->create_load(iAlloca);
    add = builder->create_iadd(iLoad, CONST_INT(1));
    builder->create_store(add, iAlloca);
    builder->create_br(whileBB);
    builder->set_insert_point(falseBB);
    builder->create_void_ret();

    // main
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                    "main", module);
    bb = BasicBlock::create(module, "entry", mainFun);
    // BasicBlock的名字在生成中无所谓,但是可以方便阅读
    builder->set_insert_point(bb);

    retAlloca = builder->create_alloca(Int32Type);
    auto *arrayType_arr = ArrayType::get(Int32Type, 100);
    auto arrAlloca = builder->create_alloca(arrayType_arr);
    auto arr = builder->create_gep(arrAlloca, {CONST_INT(0), CONST_INT(0)});

    // getarr
    auto getarray = scope->find("getarray", true);
    auto getarray_call = builder->create_call(getarray, {arr});
    auto sizeAlloca = builder->create_alloca(Int32Type);
    builder->create_store(getarray_call, sizeAlloca);
    auto sizeLoad = builder->create_load(sizeAlloca);
    sub = builder->create_isub(sizeLoad, CONST_INT(1));
    builder->create_call(QuicksortFun, {arr, CONST_INT(0), sub});
    builder->create_call(PrintFun, {arr, sizeLoad});
    builder->create_store(CONST_INT(0), retAlloca);
    retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);
    std::cout
        << module->print();
    // delete module;
    return 0;
}
#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
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
    auto module = new Module("While gen"); // module name是什么无关紧要
    auto builder = new IRStmtBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);

    auto zero_initializer = ConstantZero::get(Int32Type, module);
    auto a = GlobalVariable::create("a", module, Int32Type, false, zero_initializer);
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                    "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto retBB = BasicBlock::create(
        module, "", mainFun);
    auto retAlloca = builder->create_alloca(Int32Type);
    auto bAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), bAlloca);
    builder->create_store(CONST_INT(3), a);

    auto whileBB = BasicBlock::create(module, "whileBB_entry", mainFun);
    builder->create_br(whileBB);
    builder->set_insert_point(whileBB);
    auto bLoad = builder->create_load(bAlloca);
    auto aLoad = builder->create_load(a);
    auto icmp = builder->create_icmp_gt(aLoad, CONST_INT(0));
    auto trueBB = BasicBlock::create(module, "trueBB_if", mainFun);   // true分支
    auto falseBB = BasicBlock::create(module, "falseBB_if", mainFun); // false分支
    builder->create_cond_br(icmp, trueBB, falseBB);
    builder->set_insert_point(trueBB);
    icmp = builder->create_icmp_lt(bLoad, CONST_INT(10));
    auto trueBB2 = BasicBlock::create(module, "trueBB2_if", mainFun);   // true分支
    auto falseBB2 = BasicBlock::create(module, "falseBB2_if", mainFun); // false分支
    builder->create_cond_br(icmp, trueBB2, falseBB2);
    builder->set_insert_point(falseBB2);
    builder->create_br(retBB);
    builder->set_insert_point(trueBB2);
    bLoad = builder->create_load(bAlloca);
    aLoad = builder->create_load(a);
    auto add = builder->create_iadd(bLoad, aLoad);
    builder->create_store(add, bAlloca);
    aLoad = builder->create_load(a);
    auto sub = builder->create_isub(aLoad, CONST_INT(1));
    builder->create_store(sub, a);
    builder->create_br(whileBB);
    builder->set_insert_point(falseBB);
    builder->create_br(retBB);

    builder->set_insert_point(retBB); // ret分支
    builder->create_store(CONST_INT(0), retAlloca);
    auto retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);

    std::cout << module->print();
    delete module;
    return 0;
}

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
    ConstantInt::get(num, module_ptr)

#define CONST_FP(num) \
    ConstantFloat::get(num, module_ptr) // 得到常数值的表示,方便后面多次用到

int main()
{
    auto builder = new IRBuilder();
    auto module = builder->getModule();
    auto module_ptr = module.get();
    auto scope = builder->getScope();
    auto stmt_builder = builder->getStmtBuilder();
    Type *Int32Type = Type::get_int32_type(module_ptr);
    Type *FloatType = Type::get_float_type(module_ptr);
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                    "main", module_ptr);
    auto bb = BasicBlock::create(module_ptr, "entry", mainFun);
    stmt_builder->set_insert_point(bb);
    auto retAlloca = stmt_builder->create_alloca(Int32Type);
    auto getint = scope->find("getint", true);
    auto getint_call = stmt_builder->create_call(getint, {});
    // std::cout << getint_call;
    auto aAlloca = stmt_builder->create_alloca(Int32Type);
    stmt_builder->create_store(getint_call, aAlloca);
    auto putint = scope->find("putint", true);
    auto aLoad = stmt_builder->create_load(aAlloca);
    stmt_builder->create_call(putint, {aLoad});
    auto *arrayType_b = ArrayType::get(FloatType, 10000);
    auto bAlloca = stmt_builder->create_alloca(arrayType_b);

    auto b0Gep = stmt_builder->create_gep(bAlloca, {CONST_INT(0), CONST_INT(0)});
    stmt_builder->create_store(CONST_FP(1.0), b0Gep);
    auto b1Gep = stmt_builder->create_gep(bAlloca, {CONST_INT(0), CONST_INT(1)});
    stmt_builder->create_store(CONST_FP(2.0), b1Gep);

    auto getfarray = scope->find("getfarray", true);
    auto getfarray_call = stmt_builder->create_call(getfarray, {b0Gep});
    auto nAlloca = stmt_builder->create_alloca(Int32Type);
    stmt_builder->create_store(getfarray_call, nAlloca);

    auto nLoad = stmt_builder->create_load(nAlloca);
    auto add = stmt_builder->create_iadd(nLoad, CONST_INT(1));
    auto putfarray = scope->find("putfarray", true);
    stmt_builder->create_call(putfarray, {add, b0Gep});
    b0Gep = stmt_builder->create_gep(bAlloca, {CONST_INT(0), CONST_INT(0)});
    auto b0Load = stmt_builder->create_load(b0Gep);
    auto b0Down = stmt_builder->create_fptosi(b0Load, Int32Type);

    stmt_builder->create_store(b0Down, retAlloca);
    auto retLoad = stmt_builder->create_load(retAlloca);
    stmt_builder->create_ret(retLoad);
    std::cout
        << module_ptr->print();
    delete builder;
}
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
    auto module = new Module("assign_gen");
    auto builder = new IRStmtBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);
    Type *FloatType = Type::get_float_type(module);
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                    "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    // float b = 1.8;
    DEBUG_OUTPUT
    auto retAlloca = builder->create_alloca(Int32Type);
    auto bAlloca = builder->create_alloca(FloatType);
    builder->create_store(CONST_FP(1.8), bAlloca);
    // int a[2] = {2};
    auto *arrayType_a = ArrayType::get(Int32Type, 2);
    auto aAlloca = builder->create_alloca(arrayType_a);
    auto a0Gep = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
    builder->create_store(CONST_INT(2), a0Gep);
    // std::vector<Constant *> init_val;
    // init_val.push_back(CONST_INT(2));
    // auto a_initializer = ConstantArray::get(arrayType_a, init_val);
    // a[1] = a[0] * b;
    a0Gep = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
    auto a0Load = builder->create_load(a0Gep);

    auto a1Gep = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(1)});
    auto bLoad = builder->create_load(bAlloca);
    auto a0Up = builder->create_sitofp(a0Load, FloatType);
    auto mul = builder->create_fmul(a0Up, bLoad);
    auto mulDown = builder->create_fptosi(mul, Int32Type);
    builder->create_store(mulDown, a1Gep);
    // std::cout << bLoad->print();
    // ret b
    auto bDown = builder->create_fptosi(bLoad, Int32Type);
    builder->create_store(bDown, retAlloca);
    auto retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);
    std::cout << module->print();
    delete module;
    return 0;
}

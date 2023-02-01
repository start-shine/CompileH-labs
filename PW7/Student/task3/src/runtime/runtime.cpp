#include <iostream>
#include "runtime/io.h"
#include "runtime/runtime.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>

using namespace std;
using namespace llvm;

runtime_info::runtime_info(Module *module)
{
    get_int_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {}, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage,
                                    "getint", // IR中的函数命名，保持与输入输出函数命名一致即可
                                    module);
    get_char_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {}, false),
                                     GlobalValue::LinkageTypes::ExternalLinkage,
                                     "getch", // IR中的函数命名，保持与输入输出函数命名一致即可
                                     module);
    get_float_func = Function::Create(FunctionType::get(Type::getFloatTy(module->getContext()), {}, false),
                                      GlobalValue::LinkageTypes::ExternalLinkage,
                                      "getfloat", // IR中的函数命名，保持与输入输出函数命名一致即可
                                      module);
    get_int_array_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {Type::getInt32PtrTy(module->getContext())}, false),
                                          GlobalValue::LinkageTypes::ExternalLinkage,
                                          "getarray", // IR中的函数命名，保持与输入输出函数命名一致即可
                                          module);
    get_float_array_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {Type::getFloatPtrTy(module->getContext())}, false),
                                            GlobalValue::LinkageTypes::ExternalLinkage,
                                            "getfarray", // IR中的函数命名，保持与输入输出函数命名一致即可
                                            module);

    put_int_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), {Type::getInt32Ty(module->getContext())}, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage,
                                    "putint", // IR中的函数命名，保持与输入输出函数命名一致即可
                                    module);
    put_char_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), {Type::getInt32Ty(module->getContext())}, false),
                                     GlobalValue::LinkageTypes::ExternalLinkage,
                                     "putchar", // IR中的函数命名，保持与输入输出函数命名一致即可
                                     module);
    put_float_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), {Type::getFloatTy(module->getContext())}, false),
                                      GlobalValue::LinkageTypes::ExternalLinkage,
                                      "putfloat", // IR中的函数命名，保持与输入输出函数命名一致即可
                                      module);
    put_int_array_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), {Type::getInt32Ty(module->getContext()), Type::getInt32PtrTy(module->getContext())}, false),
                                          GlobalValue::LinkageTypes::ExternalLinkage,
                                          "putarray", // IR中的函数命名，保持与输入输出函数命名一致即可
                                          module);
    put_float_array_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), {Type::getInt32Ty(module->getContext()), Type::getFloatPtrTy(module->getContext())}, false),
                                            GlobalValue::LinkageTypes::ExternalLinkage,
                                            "putfarray", // IR中的函数命名，保持与输入输出函数命名一致即可
                                            module);
    current_time_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {}, false),
                                         GlobalValue::LinkageTypes::ExternalLinkage,
                                         "current_time", // IR中的函数命名，保持与输入输出函数命名一致即可
                                         module);

    //  TODO
}

using namespace string_literals;

std::vector<std::tuple<std::string, void *>> runtime_info::get_runtime_symbols()
{
    return {
        make_tuple("getint"s, (void *)&::getint),
        // TODO
        make_tuple("getfloat"s, (void *)&::getfloat),
        make_tuple("getch"s, (void *)&::getch),
        make_tuple("getfarray"s, (void *)&::getfarray),
        make_tuple("getarray"s, (void *)&::getarray),
        make_tuple("putint"s, (void *)&::putint),
        make_tuple("putfloat"s, (void *)&::putfloat),
        make_tuple("putch"s, (void *)&::putch),
        make_tuple("putarray"s, (void *)&::putarray),
        make_tuple("putfarray"s, (void *)&::putfarray),
        make_tuple("current_time"s, (void *)&::current_time),

    };
}
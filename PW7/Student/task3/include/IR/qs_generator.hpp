#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <iostream>
#include <memory>
#include "runtime/runtime.h"

using namespace llvm; // 指明命名空间为llvm

#define CONST(num) \
    ConstantInt::get(context, APInt(32, num)) //得到常数值的表示,方便后面多次用到
#define CONSTFLOAT(num) \
    ConstantFP::get(context, APFloat(float(num))) //得到常数值的表示,方便后面多次用到

llvm::Module *qs_generator(LLVMContext &context, runtime_info *&runtime)
{
    IRBuilder<> builder(context);
    llvm::Type *TYPE32 = llvm::Type::getInt32Ty(context);
    llvm::Type *TYPEFLOAT = llvm::Type::getFloatTy(context);
    llvm::Type *TYPEFLOATptr = llvm::Type::getFloatPtrTy(context);
    llvm::Type *TYPEVOID = llvm::Type::getVoidTy(context);
    auto module = new llvm::Module("qs", context);
    runtime = new runtime_info(module);
    // auto vec = runtime->get_runtime_symbols();
    llvm::Constant *buf_size = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 4096);
    /**
     * @brief TODO
     * 参照 `include/IR/gcd_generator.hpp` 中对LLVM IR Builder的使用，实现 `tests/quicksort.c` 中算法的 LLVM IR
     */
    std::vector<llvm::Type *> QuickSorts;
    QuickSorts.push_back(TYPEFLOATptr);
    QuickSorts.push_back(TYPE32);
    QuickSorts.push_back(TYPE32);
    auto qsFun = Function::Create(llvm::FunctionType::get(TYPEVOID, QuickSorts, false),
                                  GlobalValue::LinkageTypes::ExternalLinkage,
                                  "qs", module);
    auto bb = BasicBlock::Create(context, "entry", qsFun);
    auto trueBB = BasicBlock::Create(context, "trueBB", qsFun);   // true分支
    auto falseBB = BasicBlock::Create(context, "falseBB", qsFun); // false分支
    auto retBB = BasicBlock::Create(context, "retBB", qsFun);     // return分支,提前create,以便true分支可以br
    auto while1BB = BasicBlock::Create(context, "while1BB", qsFun);
    auto trueBB_while1 = BasicBlock::Create(context, "trueBB_while1", qsFun);     // true分支
    auto falseBB_while1 = BasicBlock::Create(context, "falseBB_while1", qsFun);   // false分支
    auto while2BB = BasicBlock::Create(context, "while2BB", qsFun);               // return分支,提前create,以便true分支可以br
    auto trueBB_while21 = BasicBlock::Create(context, "trueBB_while21", qsFun);   // true分支
    auto falseBB_while21 = BasicBlock::Create(context, "falseBB_while21", qsFun); // false分支
    auto trueBB_while22 = BasicBlock::Create(context, "trueBB_while22", qsFun);   // true分支
    auto falseBB_while22 = BasicBlock::Create(context, "falseBB_while22", qsFun); // false分支
    auto if2BB = BasicBlock::Create(context, "if2BB", qsFun);
    auto trueBB_if2 = BasicBlock::Create(context, "trueBB_if2", qsFun);   // true分支
    auto falseBB_if2 = BasicBlock::Create(context, "falseBB_if2", qsFun); // false分支
    auto while3BB = BasicBlock::Create(context, "while3BB", qsFun);
    auto trueBB_while31 = BasicBlock::Create(context, "trueBB_while31", qsFun);   // true分支
    auto falseBB_while31 = BasicBlock::Create(context, "falseBB_while31", qsFun); // false分支

    auto trueBB_while32 = BasicBlock::Create(context, "trueBB_while32", qsFun);   // true分支
    auto falseBB_while32 = BasicBlock::Create(context, "falseBB_while32", qsFun); // false分支

    auto if3BB = BasicBlock::Create(context, "if3BB", qsFun);
    auto trueBB_if3 = BasicBlock::Create(context, "trueBB_if3", qsFun);   // true分支
    auto falseBB_if3 = BasicBlock::Create(context, "falseBB_if3", qsFun); // false分支
    builder.SetInsertPoint(bb);
    std::vector<Value *> args; //获取gcd函数的参数,通过iterator
    for (auto arg = qsFun->arg_begin(); arg != qsFun->arg_end(); arg++)
    {
        args.push_back(arg);
    }
    auto icmp = builder.CreateICmpSLT(args[1], args[2]); // v和0的比较,注意ICMPEQ

    auto br = builder.CreateCondBr(icmp, trueBB, falseBB); // 条件BR
    builder.SetInsertPoint(falseBB);
    builder.CreateBr(retBB);

    builder.SetInsertPoint(trueBB);
    // i=low
    auto iAlloca = builder.CreateAlloca(TYPE32); // 参数u的空间分配
    builder.CreateStore(args[1], iAlloca);
    // j=high
    auto jAlloca = builder.CreateAlloca(TYPE32);
    builder.CreateStore(args[2], jAlloca);
    auto alowGep = builder.CreateGEP(args[0], {args[1]});
    auto alowLoad = builder.CreateLoad(alowGep);
    auto kAlloca = builder.CreateAlloca(TYPEFLOAT);
    builder.CreateStore(alowLoad, kAlloca);
    builder.CreateBr(while1BB);
    // return分支,提前create,以便true分支可以br
    builder.SetInsertPoint(while1BB);

    auto iLoad = builder.CreateLoad(iAlloca);
    auto jLoad = builder.CreateLoad(jAlloca);
    icmp = builder.CreateICmpSLT(iLoad, jLoad);
    br = builder.CreateCondBr(icmp, trueBB_while1, falseBB_while1); // 条件BR
    builder.SetInsertPoint(trueBB_while1);

    builder.CreateBr(while2BB);
    builder.SetInsertPoint(while2BB);
    // while(i<j && arr[j]>k)
    // i<j?
    iLoad = builder.CreateLoad(iAlloca);
    jLoad = builder.CreateLoad(jAlloca);
    icmp = builder.CreateICmpSLT(iLoad, jLoad);
    br = builder.CreateCondBr(icmp, trueBB_while21, falseBB_while21); // 条件BR
    builder.SetInsertPoint(trueBB_while21);
    // arr[j]>=k?
    jLoad = builder.CreateLoad(jAlloca);
    auto ajGep = builder.CreateGEP(args[0], {jLoad});
    auto ajLoad = builder.CreateLoad(ajGep);
    auto kLoad = builder.CreateLoad(kAlloca);
    icmp = builder.CreateFCmpOGE(ajLoad, kLoad);
    br = builder.CreateCondBr(icmp, trueBB_while22, falseBB_while22); // 条件BR
    builder.SetInsertPoint(trueBB_while22);
    // j--
    jLoad = builder.CreateLoad(jAlloca);
    auto sub = builder.CreateSub(jLoad, CONST(1));
    builder.CreateStore(sub, jAlloca);
    // while2 again
    builder.CreateBr(while2BB);
    // while2 false
    builder.SetInsertPoint(falseBB_while22);
    builder.CreateBr(falseBB_while21);
    builder.SetInsertPoint(falseBB_while21);
    builder.CreateBr(if2BB);
    // if2(if i<j...)

    builder.SetInsertPoint(if2BB);

    iLoad = builder.CreateLoad(iAlloca);
    jLoad = builder.CreateLoad(jAlloca);
    icmp = builder.CreateICmpSLT(iLoad, jLoad);
    br = builder.CreateCondBr(icmp, trueBB_if2, falseBB_if2); // 条件BR
    builder.SetInsertPoint(trueBB_if2);
    // arr[i]=arr[j]
    jLoad = builder.CreateLoad(jAlloca);
    ajGep = builder.CreateGEP(args[0], {jLoad});
    ajLoad = builder.CreateLoad(ajGep);
    iLoad = builder.CreateLoad(iAlloca);
    auto aiGep = builder.CreateGEP(args[0], {iLoad});
    builder.CreateStore(ajLoad, aiGep);

    // i++
    iLoad = builder.CreateLoad(iAlloca);
    auto add = builder.CreateAdd(iLoad, CONST(1));
    builder.CreateStore(add, iAlloca);

    builder.CreateBr(falseBB_if2);
    builder.SetInsertPoint(falseBB_if2);

    builder.CreateBr(while3BB);
    builder.SetInsertPoint(while3BB);

    // start while3
    iLoad = builder.CreateLoad(iAlloca);
    jLoad = builder.CreateLoad(jAlloca);
    icmp = builder.CreateICmpSLT(iLoad, jLoad);
    br = builder.CreateCondBr(icmp, trueBB_while31, falseBB_while31); // 条件BR
    builder.SetInsertPoint(trueBB_while31);
    // arr[i]<k?
    iLoad = builder.CreateLoad(iAlloca);
    aiGep = builder.CreateGEP(args[0], {iLoad});
    auto aiLoad = builder.CreateLoad(aiGep);
    kLoad = builder.CreateLoad(kAlloca);
    icmp = builder.CreateFCmpOLT(aiLoad, kLoad);
    br = builder.CreateCondBr(icmp, trueBB_while32, falseBB_while32); // 条件BR
    builder.SetInsertPoint(trueBB_while32);
    // i++
    iLoad = builder.CreateLoad(iAlloca);
    add = builder.CreateAdd(iLoad, CONST(1));
    builder.CreateStore(add, iAlloca);
    // while3 again
    builder.CreateBr(while3BB);
    // while3 false
    builder.SetInsertPoint(falseBB_while32);
    builder.CreateBr(falseBB_while31);
    builder.SetInsertPoint(falseBB_while31);

    builder.CreateBr(if3BB);
    // if3

    builder.SetInsertPoint(if3BB);
    iLoad = builder.CreateLoad(iAlloca);
    jLoad = builder.CreateLoad(jAlloca);
    icmp = builder.CreateICmpSLT(iLoad, jLoad);
    br = builder.CreateCondBr(icmp, trueBB_if3, falseBB_if3); // 条件BR
    builder.SetInsertPoint(trueBB_if3);

    // arr[j]=arr[i]
    iLoad = builder.CreateLoad(iAlloca);
    aiGep = builder.CreateGEP(args[0], {iLoad});
    aiLoad = builder.CreateLoad(aiGep);
    jLoad = builder.CreateLoad(jAlloca);
    ajGep = builder.CreateGEP(args[0], {jLoad});
    builder.CreateStore(aiLoad, ajGep);

    // j--

    jLoad = builder.CreateLoad(jAlloca);
    sub = builder.CreateSub(jLoad, CONST(1));
    builder.CreateStore(sub, jAlloca);
    builder.CreateBr(falseBB_if3);
    builder.SetInsertPoint(falseBB_if3);
    // while1 again
    builder.CreateBr(while1BB);

    // while1 false
    builder.SetInsertPoint(falseBB_while1);
    // arr[i]=k
    kLoad = builder.CreateLoad(kAlloca);
    iLoad = builder.CreateLoad(iAlloca);
    aiGep = builder.CreateGEP(args[0], {iLoad});
    builder.CreateStore(kLoad, aiGep);
    // Quicksort1
    iLoad = builder.CreateLoad(iAlloca);
    sub = builder.CreateSub(iLoad, CONST(1));
    builder.CreateCall(qsFun, {args[0], args[1], sub});
    // Quicksort2
    iLoad = builder.CreateLoad(iAlloca);
    add = builder.CreateAdd(iLoad, CONST(1));
    builder.CreateCall(qsFun, {args[0], add, args[2]});

    builder.CreateBr(retBB);

    // // end if
    builder.SetInsertPoint(retBB);

    builder.CreateRetVoid();

    // main 函数
    auto mainFun = Function::Create(llvm::FunctionType::get(TYPE32, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage,
                                    "main", module);
    bb = BasicBlock::Create(context, "entry", mainFun);
    // BasicBlock的名字在生成中无所谓,但是可以方便阅读
    builder.SetInsertPoint(bb);
    auto *array_type = llvm::ArrayType::get(llvm::Type::getFloatTy(context), 4096);
    auto arrayAlloca = builder.CreateAlloca(array_type);
    auto a0Gep = builder.CreateGEP(arrayAlloca, {CONST(0), CONST(0)});
    builder.CreateStore(CONSTFLOAT(0), a0Gep);
    auto arrnumAlloca = builder.CreateAlloca(TYPE32);
    // getfarray todo
    auto call = builder.CreateCall(runtime->get_float_array_func, {a0Gep});
    builder.CreateStore(call, arrnumAlloca);
    // current_time todo
    auto startAlloca = builder.CreateAlloca(TYPE32);
    call = builder.CreateCall(runtime->current_time_func, {});
    builder.CreateStore(call, startAlloca);
    // // qs
    auto arrnum = builder.CreateLoad(arrnumAlloca);
    sub = builder.CreateSub(arrnum, CONST(1));
    builder.CreateCall(qsFun, {a0Gep, CONST(0), sub});
    // end
    auto endAlloca = builder.CreateAlloca(TYPE32);
    call = builder.CreateCall(runtime->current_time_func, {});
    builder.CreateStore(call, endAlloca);
    // putfarray todo
    arrnum = builder.CreateLoad(arrnumAlloca);
    builder.CreateCall(runtime->put_float_array_func, {arrnum, a0Gep});
    // putint todo
    auto startLoad = builder.CreateLoad(startAlloca);
    auto endLoad = builder.CreateLoad(endAlloca);
    sub = builder.CreateSub(endLoad, startLoad);
    builder.CreateCall(runtime->put_int_func, {sub});
    // putchar todo
    builder.CreateCall(runtime->put_char_func, {CONST(' ')});
    builder.CreateCall(runtime->put_char_func, {CONST('m')});
    builder.CreateCall(runtime->put_char_func, {CONST('s')});
    builder.CreateCall(runtime->put_char_func, {CONST('\n')});
    auto retAlloca = builder.CreateAlloca(TYPE32); // 返回值的空间分配
    builder.CreateStore(CONST(0), retAlloca);
    auto retLoad = builder.CreateLoad(retAlloca);
    builder.CreateRet(retLoad);
    return module;
}

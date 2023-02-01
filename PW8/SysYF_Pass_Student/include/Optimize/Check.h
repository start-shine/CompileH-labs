#ifndef CHECK_H
#define CHECK_H

#include <algorithm>

#include "BasicBlock.h"
#include "Function.h"
#include "Instruction.h"
#include "Module.h"
#include "Pass.h"
#include "IRPrinter.h"

class Check : public Pass
{
private:
    const std::string name = "Check";
    Function *func_;
    // define_reg;

public:
    explicit Check(Module *m) : Pass(m) {}
    ~Check() {}
    void execute() final;
    const std::string get_name() const override { return name; }
    void CheckLastInst();
    void CheckOp(BasicBlock *bb, std::vector<std::string> defined_op);
    void CheckPreAndSuc();
    void CheckDefUseChain();
};

#endif
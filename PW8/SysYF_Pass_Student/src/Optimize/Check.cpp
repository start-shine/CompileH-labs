#include "Check.h"
#include "Module.h"

static std::set<std::string> defined_label;
static std::set<std::string> used_label;

void Check::execute()
{
    // TODO write your IR Module checker here.
    std::vector<std::string> defined_op;
    for (auto variable : module->get_global_variable())
    {
        defined_op.push_back(variable->get_name());
    }
    for (auto fun : module->get_functions())
    {
        defined_op.push_back(fun->get_name());
    }
    for (auto fun : module->get_functions())
    {
        if (fun->get_basic_blocks().empty())
            continue;
        func_ = fun;
        for (auto arg : func_->get_args())
        {
            defined_op.push_back(arg->get_name());
        }
        CheckOp(func_->get_entry_block(), defined_op);
        for (auto it = used_label.begin(); it != used_label.end(); it++)
        {
            if (defined_label.find(*it) == defined_label.end())
            {
                std::cout << "error not find " << *it << std::endl;
                exit(2);
            }
        }
        CheckLastInst();
        CheckPreAndSuc();
    }
}

void Check::CheckLastInst()
{
    for (auto bb : func_->get_basic_blocks())
    {
        Instruction *last_inst;
        for (auto inst : bb->get_instructions())
        {
            last_inst = inst;
        }
        if (last_inst->get_instr_type() == Instruction::OpID::br || last_inst->get_instr_type() == Instruction::OpID::ret)
        {
            continue;
        }
        else
        {
            std::cout << "error" << std::endl;
            exit(1);
        }
    }
}

std::set<BasicBlock *> visited_bbs;
static std::map<std::string, int> phi_times;
int i = 0;
void Check::CheckOp(BasicBlock *bb, std::vector<std::string> defined_op)
{
    if (visited_bbs.find(bb) == visited_bbs.end())
        visited_bbs.insert(bb);
    defined_label.insert(bb->get_name());
    for (auto inst : bb->get_instructions())
    {
        auto opnum = inst->get_num_operand();
        auto op = inst->get_operands();
        auto op0 = inst->get_name();

        // add defined_op
        if (!op0.empty() && find(defined_op.begin(), defined_op.end(), op0) == defined_op.end())
            defined_op.push_back(op0);
        else
        {
            if (inst->get_instr_type() == Instruction::OpID::store)
                defined_op.push_back(static_cast<StoreInst *>(inst)->get_lval()->get_name());
        }

        // check if op defined?
        if (inst->get_instr_type() == Instruction::OpID::phi)
        {
            continue;
        }
        for (auto opr : inst->get_operands())
        {
            auto op_name = opr->get_name();
            if (op_name.empty())
                continue;
            if (op_name.substr(0, 5) == "label")
            {
                if (used_label.find(op_name) == used_label.end())
                    used_label.insert(op_name);
                continue;
            }
            if (find(defined_op.begin(), defined_op.end(), op_name) != defined_op.end())
                continue;
            else
            {
                std::cout << "error: not define" << op_name << std::endl;
                exit(2);
            }
        }
    }
    for (auto succbb : bb->get_succ_basic_blocks())
    {
        bool finish_phi = true;
        bool start_phi = false;
        for (auto inst : succbb->get_instructions())
        {
            if (inst->get_instr_type() == Instruction::OpID::phi)
            {
                start_phi = true;
                //  %op37 = phi i32 [ -1, %label_entry ], [ %op39, %label26 ]
                // add phi to map
                phi_times[inst->print()] = inst->get_num_operand() / 2;
                // add defined_op
                defined_op.push_back(inst->get_name());
                // check if op defined?
                // labels
                for (auto opr : inst->get_operands())
                {
                    auto op_name = opr->get_name();
                    if (op_name.substr(0, 5) == "label")
                    {
                        if (used_label.find(op_name) == used_label.end())
                            used_label.insert(op_name);
                        continue;
                    }
                }
                // ops
                auto operands_ = inst->get_operands();
                for (int i = 1; i < operands_.size(); i += 2)
                {
                    if (operands_[i]->get_name() == bb->get_name())
                    {
                        auto op_name = operands_[i - 1]->get_name();
                        if (op_name.empty())
                            break;
                        if (find(defined_op.begin(), defined_op.end(), op_name) != defined_op.end())
                            break;
                        else
                        {
                            std::cout << "error: not define" << op_name << std::endl;
                            exit(2);
                        }
                    }
                }
                // sub times
                phi_times[inst->print()]--;
                if (phi_times[inst->print()] > 0)
                    finish_phi = false;
            }
        }
        if (visited_bbs.find(succbb) != visited_bbs.end() && (start_phi == false || finish_phi == false))
            continue;
        CheckOp(succbb, defined_op);
    }
}

void Check::CheckPreAndSuc()
{
    for (auto bb : func_->get_basic_blocks())
    {
        auto succ_bbs_ = bb->get_succ_basic_blocks();
        std::set<std::string> succ_labels;
        succ_labels.clear();
        for (auto sucbb : succ_bbs_)
        {
            succ_labels.insert(sucbb->get_name());
        }
        auto inst_list = bb->get_instructions();
        auto last_inst = inst_list.back();
        if (last_inst->get_instr_type() == Instruction::OpID::br)
        {
            for (auto label : last_inst->get_operands())
            {
                auto label_name = label->get_name();
                if (label_name.substr(0, 2) == "op")
                    continue;
                if (succ_labels.find(label_name) != succ_labels.end())
                    continue;
                else
                {
                    std::cout << "error succ_bbs wrong in " << bb->get_name() << std::endl;
                    exit(3);
                }
            }
        }
    }
}

std::map<std::string, std::vector<std::vector<std::string>>> DefUseChain;
// void Check::CheckDefUseChain()
// {
//     for (auto bb : func_->get_basic_blocks())
//     {
//         std::vector<std::string> defined_op;
//         for (auto inst : bb->get_instructions())
//         {
//             auto def = inst->get_name();
//             defined_op.push_back(def);
//             for (auto opr : inst->get_operands())
//             {
//                 if (find(defined_op.begin(), defined_op.end(), opr->get_name()) != defined_op.end())
//                 {

//                 }
//                 else if ()
//             }
//         }
//         for (auto succbb : bb->get_succ_basic_blocks())
//         {
//             if (visited.find(succbb) != visited.end())
//                 continue;
//             std::cout << succbb->print();
//             CheckOp(succbb, defined_op);
//         }
//     }
// }
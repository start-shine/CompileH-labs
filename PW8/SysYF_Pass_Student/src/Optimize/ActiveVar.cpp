#include "ActiveVar.h"
#include <fstream>

#include <algorithm>

void ActiveVar::execute() {
    //  请不要修改该代码。在被评测时不要在中间的代码中重新调用set_print_name
    module->set_print_name();
    //

    for (auto &func : this->module->get_functions()) {
        if (func->get_basic_blocks().empty()) {
            continue;
        } else {
            func_ = func;
            /*you need to finish this function*/

            //find the exit_block and get reverse order
            BasicBlock*  exit_block;
            for (auto bb:func_ -> get_basic_blocks()){
                auto terminate_instr = bb -> get_terminator();
                if (terminate_instr -> is_ret()){
                    exit_block = bb;
                    break;
                }
            }
            if(!exit_block){
                std::cerr << "exit block is null, function must have only one exit block with a ret instr\n";
                std::cerr << "err function:\n" << func_->print() << std::endl;
                exit(1);
            }
            std::set<BasicBlock*> visited = {};
            get_post_order(exit_block,visited);
            reverse_post_order.reverse();

            // find the use and def

            std::map<BasicBlock *, std::set<Value *>> normal_use_set;
            std::map<BasicBlock *, std::map<BasicBlock *, std::set<Value *>>> phi_use_set;
            std::map<BasicBlock *, std::set<Value *>> def_set;
            for (auto bb:reverse_post_order){
                normal_use_set.insert({bb,{}});
                phi_use_set.insert({bb,{}});
                def_set.insert({bb,{}});
                for (auto inst: bb -> get_instructions()){
                    //std::cout << "hello\n";
                    if (inst -> get_instr_type() == Instruction::OpID::phi){
                        //std::cout << "hello_phi\n";
                        int phi_pair_num = inst -> get_num_operand() / 2;
                        for (int i = 0;i < phi_pair_num;i ++){
                            auto phi_operand = inst -> get_operand(2 * i);
                            BasicBlock * parent_bb = dynamic_cast<BasicBlock *>(inst -> get_operand(2 * i + 1));
                            //sugguest the operand is a variable
                            if (dynamic_cast<GlobalVariable *>(phi_operand) == nullptr && dynamic_cast<Constant *>(phi_operand) == nullptr){
                                if (phi_use_set.find(bb) -> second.find(parent_bb) != phi_use_set.find(bb) -> second.end()){
                                    phi_use_set.find(bb) -> second.find(parent_bb) -> second.insert(phi_operand);
                                }
                                else{
                                    phi_use_set.find(bb) -> second.insert({parent_bb, {phi_operand}});
                                }
                                //std::cout << inst->get_operand(2*i) -> get_name() << '\n';
                            }
                            //std::cout << inst->get_operand(2*i) -> get_name() << '\n';
                        }
                        def_set.find(bb) -> second.insert(inst);
                    }
                    else if (inst -> is_binary() | inst -> is_cmp() | inst -> is_fcmp()){
                        //std::cout << "hello_binary\n";
                        auto v1 = inst -> get_operand(0);
                        auto v2 = inst -> get_operand(1);
                        if (dynamic_cast<GlobalVariable *>(v1) == nullptr && dynamic_cast<Constant *>(v1) == nullptr)
                            if (def_set.find(bb) -> second.find(v1) == def_set.find(bb) -> second.end()){
                                normal_use_set.find(bb) -> second.insert(v1);
                            }
                        if (dynamic_cast<GlobalVariable *>(v2) == nullptr && dynamic_cast<Constant *>(v2) == nullptr)
                            if (def_set.find(bb) -> second.find(v2) == def_set.find(bb) -> second.end()){
                                normal_use_set.find(bb) -> second.insert(v2);
                            }
                        if (dynamic_cast<GlobalVariable *>(inst) == nullptr && dynamic_cast<Constant *>(inst) == nullptr)
                            if (normal_use_set.find(bb) -> second.find(inst) == normal_use_set.find(bb) -> second.end()){
                                def_set.find(bb) -> second.insert(inst);
                            }
                    }
                    else if (inst -> get_instr_type() == Instruction::OpID::store){
                        //std::cout << "hello_store\n";
                        auto rvalue = inst -> get_operand(0);
                        auto lvalue = inst -> get_operand(1);
                        if (dynamic_cast<GlobalVariable *>(rvalue) == nullptr && dynamic_cast<Constant *>(rvalue) == nullptr)
                            if (def_set.find(bb) -> second.find(rvalue) == def_set.find(bb) -> second.end()){
                                normal_use_set.find(bb) -> second.insert(rvalue);
                            }
                        if (dynamic_cast<GlobalVariable *>(lvalue) == nullptr && dynamic_cast<Constant *>(lvalue) == nullptr)
                            if (def_set.find(bb) -> second.find(lvalue) == def_set.find(bb) -> second.end()){
                                normal_use_set.find(bb) -> second.insert(lvalue);
                            }
                    }
                    else if (inst -> get_instr_type() == Instruction::OpID::load){
                        //std::cout << "hello_load\n";
                        auto lvalue = inst -> get_operand(0);
                        if (dynamic_cast<GlobalVariable *>(lvalue) == nullptr && dynamic_cast<Constant *>(lvalue) == nullptr)
                            if (def_set.find(bb) -> second.find(lvalue) == def_set.find(bb) -> second.end()){
                                normal_use_set.find(bb) -> second.insert(lvalue);
                            }
                        if (dynamic_cast<GlobalVariable *>(inst) == nullptr && dynamic_cast<Constant *>(inst) == nullptr)
                            if (normal_use_set.find(bb) -> second.find(inst) == normal_use_set.find(bb) -> second.end()){
                                def_set.find(bb) -> second.insert(inst);
                            }
                    }
                    else if (inst -> is_call()){
                        //std::cout << "hello_call\n";
                        for (int i = 1; i < inst -> get_num_operand(); i ++){
                            auto v = inst -> get_operand(i);
                            if (dynamic_cast<GlobalVariable *>(v) == nullptr && dynamic_cast<Constant *>(v) == nullptr){
                                if (def_set.find(bb) -> second.find(v) == def_set.find(bb) -> second.end()){
                                    normal_use_set.find(bb) -> second.insert(v);
                                }
                            }
                        }
                        if (!inst -> is_void()){
                            if (dynamic_cast<GlobalVariable *>(inst) == nullptr && dynamic_cast<Constant *>(inst) == nullptr)
                                if (normal_use_set.find(bb) -> second.find(inst) == normal_use_set.find(bb) -> second.end()){
                                    def_set.find(bb) -> second.insert(inst);
                                }
                        }
                    }
                    else if (inst -> is_br() && dynamic_cast<BranchInst *>(inst) -> is_cond_br()){
                        //std::cout << "hello_br\n";
                        auto v = inst -> get_operand(0);
                        if (dynamic_cast<GlobalVariable *>(v) == nullptr && dynamic_cast<Constant *>(v) == nullptr){
                            if (def_set.find(bb) -> second.find(v) == def_set.find(bb) -> second.end()){
                                normal_use_set.find(bb) -> second.insert(v);
                            }
                        }
                    }
                    else if (inst -> is_ret()){
                        //std::cout << "hello_ret1\n";
                        if (!dynamic_cast<ReturnInst *>(inst) -> is_void_ret()){
                            auto v = inst -> get_operand(0);
                            if (dynamic_cast<GlobalVariable *>(v) == nullptr && dynamic_cast<Constant *>(v) == nullptr){
                                if (def_set.find(bb) -> second.find(v) == def_set.find(bb) -> second.end()){
                                    normal_use_set.find(bb) -> second.insert(v);
                                }
                            }
                        }
                        //std::cout << "hello_ret2\n";
                    }
                    else if (inst -> is_gep()){
                        //std::cout << "hello_gep\n";
                        for (int i = 0; i < inst->get_num_operand(); i++){
                            auto v = inst -> get_operand(i);
                            if (dynamic_cast<GlobalVariable *>(v) == nullptr && dynamic_cast<Constant *>(v) == nullptr){
                                if (def_set.find(bb) -> second.find(v) == def_set.find(bb) -> second.end()){
                                    normal_use_set.find(bb) -> second.insert(v);
                                }
                            }
                        }
                        if (dynamic_cast<GlobalVariable *>(inst) == nullptr && dynamic_cast<Constant *>(inst) == nullptr)
                            if (normal_use_set.find(bb) -> second.find(inst) == normal_use_set.find(bb) -> second.end()){
                                def_set.find(bb) -> second.insert(inst);
                            }
                    }
                    else if (inst -> is_alloca()){
                        //std::cout << "hello_alloca\n";
                        if (dynamic_cast<GlobalVariable *>(inst) == nullptr && dynamic_cast<Constant *>(inst) == nullptr)
                            if (normal_use_set.find(bb) -> second.find(inst) == normal_use_set.find(bb) -> second.end()){
                                def_set.find(bb) -> second.insert(inst);
                            }
                    }
                    else if (inst -> is_zext() || inst -> is_fptosi() || inst -> is_sitofp()){
                        //std::cout << "hello_zext\n";
                        auto v = inst -> get_operand(0);
                        if (dynamic_cast<GlobalVariable *>(v) == nullptr && dynamic_cast<Constant *>(v) == nullptr){
                            if (def_set.find(bb) -> second.find(v) == def_set.find(bb) -> second.end()){
                                normal_use_set.find(bb) -> second.insert(v);
                            }
                        }
                        if (dynamic_cast<GlobalVariable *>(inst) == nullptr && dynamic_cast<Constant *>(inst) == nullptr)
                            if (normal_use_set.find(bb) -> second.find(inst) == normal_use_set.find(bb) -> second.end()){
                                def_set.find(bb) -> second.insert(inst);
                            }
                    }
                }
                std::cout << bb -> get_name() << '\n';
                std::cout << "def\n";
                for (auto v: def_set.find(bb) -> second){
                    std::cout << v -> get_name() << '\n';
                }
                std::cout << "normal use\n";
                for (auto v: normal_use_set.find(bb) -> second){
                    std::cout << v -> get_name() << '\n';
                }
                std::cout << '\n';
            }
            //Initialize the OUT;
            for (auto bb:reverse_post_order){
                bb -> set_live_out({});
                bb -> set_live_in({});
            }

            //iterations to get IN and OUT;
            bool changed  = true;
            while (changed){
                changed = false;
                //std::cout << "hello\n";
                for (auto bb:reverse_post_order){
                    std::set<Value *> oldIN_set;
                    std::set<Value *> oldOUT_set;
                    std::set<Value *> newIN_set;
                    std::set<Value *> newOUT_set;
                    newOUT_set = {};
                    newIN_set = {};
                    oldIN_set = bb -> get_live_in();
                    oldOUT_set = bb -> get_live_out();

                    //calculate the newOUT_set
                    auto succ_blocks = bb -> get_succ_basic_blocks();
                    for (auto s: succ_blocks){
                        std::set<Value *> temp1_set;
                        std::set<Value *> temp2_set;
                        std::set<Value *> temp3_set;
                        std::set<Value *> temp4_set;
                        //std::cout << "hello\n";
                        //std::cout << "hello1\n";
                        temp1_set = normal_use_set.find(s) -> second;
                        //std::cout << "hello2\n";
                        if (phi_use_set.find(s) -> second.find(bb) != phi_use_set.find(s) -> second.end())
                            temp2_set = phi_use_set.find(s) -> second.find(bb) -> second;
                        else
                            temp2_set = {};
                        //std::cout << "hello3\n";
                        temp3_set = def_set.find(s) -> second;
                        //std::cout << "hello4\n";
                        temp4_set = s -> get_live_out();
                        std::set<Value *> diff;
                        std::set<Value *> uni;
                        set_difference(temp4_set.begin(), temp4_set.end(),temp3_set.begin(),temp3_set.end(),inserter(diff,diff.begin()));
                        set_union(temp1_set.begin(), temp1_set.end(),diff.begin(),diff.end(),inserter(uni,uni.begin()));
                        set_union(temp2_set.begin(), temp2_set.end(),uni.begin(),uni.end(),inserter(uni,uni.begin()));
                        set_union(newOUT_set.begin(), newOUT_set.end(),uni.begin(),uni.end(),inserter(newOUT_set,newOUT_set.begin()));
                    }
                    if (newOUT_set != oldOUT_set){
                        changed = true;
                        bb -> set_live_out(newOUT_set);
                    }

                    //calculate the newIN_set
                    std::set<Value *> temp1_set;
                    std::map<BasicBlock *, std::set<Value *>> temp2_set;
                    std::set<Value *> temp3_set;
                    std::set<Value *> diff;
                    temp1_set = normal_use_set.find(bb) -> second;
                    temp2_set = phi_use_set.find(bb) -> second;
                    temp3_set = def_set.find(bb) -> second;
                    set_difference(newOUT_set.begin(), newOUT_set.end(),temp3_set.begin(),temp3_set.end(),inserter(diff,diff.begin()));
                    set_union(temp1_set.begin(), temp1_set.end(),diff.begin(),diff.end(),inserter(newIN_set,newIN_set.begin()));
                    for(auto iter = temp2_set.begin();iter != temp2_set.end();iter ++){
                        auto temp_set = iter -> second;
                        set_union(temp_set.begin(), temp_set.end(),newIN_set.begin(),newIN_set.end(),inserter(newIN_set,newIN_set.begin()));
                    }
                    if (newIN_set != oldIN_set){
                        changed = true;
                        bb -> set_live_in(newIN_set);
                    }
                }
            }
        }
    }

    //  请不要修改该代码，在被评测时不要删除该代码
    dump();
    //
    return ;
}

void ActiveVar::get_post_order(BasicBlock *bb, std::set<BasicBlock *> &visited) {
    visited.insert(bb);
    auto parents = bb->get_pre_basic_blocks();
    for(auto parent : parents){
        auto is_visited = visited.find(parent);
        if(is_visited==visited.end()){
            get_post_order(parent,visited);
        }
    }
    bb2int[bb] = reverse_post_order.size();
    reverse_post_order.push_back(bb);
}

void ActiveVar::dump() {
    std::fstream f;
    f.open(avdump, std::ios::out);
    for (auto &func: module->get_functions()) {
        for (auto &bb: func->get_basic_blocks()) {
            f << bb->get_name() << std::endl;
            auto &in = bb->get_live_in();
            auto &out = bb->get_live_out();
            auto sorted_in = sort_by_name(in);
            auto sorted_out = sort_by_name(out);
            f << "in:\n";
            for (auto in_v: sorted_in) {
                f << in_v->get_name() << " ";
            }
            f << "\n";
            f << "out:\n";
            for (auto out_v: sorted_out) {
                f << out_v->get_name() << " ";
            }
            f << "\n";
        }
    }
    f.close();
}

bool ValueCmp(Value* a, Value* b) {
    return a->get_name() < b->get_name();
}

std::vector<Value*> sort_by_name(std::set<Value*> &val_set) {
    std::vector<Value*> result;
    result.assign(val_set.begin(), val_set.end());
    std::sort(result.begin(), result.end(), ValueCmp);
    return result;
}
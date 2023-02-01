//
// Created by cjb on 7/24/21.
//

#include "RegAlloc.h"
#include "Type.h"
#include <iostream>
#include <set>
#include <stack>

void Interval::add_range(int from, int to) {
    if(range_list.empty()){
        range_list.push_front(new Range(from, to));
        return;
    }
    auto top_range = *range_list.begin();
    if(from>=top_range->from && from<=top_range->to){
        top_range->to = to > top_range->to?to:top_range->to;
    }else if(from < top_range->from){
        if(to <= top_range->to && to>=top_range->from){
            top_range->from = from;
        }else{
            auto new_range = new Range(from,to);
            range_list.push_front(new_range);
        }
    }else{
        auto new_range = new Range(from,to);
        range_list.push_front(new_range);
    }
}

bool Interval::covers(int id){
    for(auto range:range_list){
        if(range->from<=id&&range->to>id){
            return true;
        }
    }
    return false;
}

bool Interval::covers(Instruction* inst){
    return covers(inst->get_id());
}

bool Interval::intersects(Interval *interval) {
    auto target_it = range_list.begin();
    auto with_it = interval->range_list.begin();
    while(with_it!=interval->range_list.end()&&target_it!=range_list.end()){
        auto target_range = *target_it;
        auto with_range = *with_it;
        if(target_range->to<=with_range->from){
            target_it++;
            continue;
        }else if(with_range->to<=target_range->from){
            with_it++;
            continue;
        }else{
            return true;
        }
    }
    return false;
}


struct cmp_range{
    bool operator()(const Range* a,const Range* b) const {
        return a->from > b->from;
    }
};

void Interval::union_interval(Interval *interval) {
    std::priority_queue<Range*, std::vector<Range*>, cmp_range> all_range;
    for(auto range:range_list){
        all_range.push(range);
    }
    for(auto range:interval->range_list){
        all_range.push(range);
    }
    if(all_range.empty()){
        return;
    }
    range_list.clear();
    auto cur_range = all_range.top();
    all_range.pop();
    while(!all_range.empty()){
        auto merge_range = all_range.top();
        all_range.pop();
        if(merge_range->from > cur_range->to){
            range_list.push_back(cur_range);
            cur_range = merge_range;
        }else{
            cur_range->to = cur_range->to >= merge_range->to?cur_range->to:merge_range->to;
        }
    }
    range_list.push_back(cur_range);
}


void RegAllocDriver::compute_reg_alloc() {
    for(auto func:module->get_functions()){
        if(func->get_basic_blocks().empty()){
            continue;
        }else{
#ifdef DEBUG
            std::cerr << "function " << func->get_name() << std::endl;
#endif
            auto allocator = new RegAlloc(func);
            if (register_optimize){
                allocator -> register_optimize = true;
            }
            allocator->execute();
            unallocate_variable += allocator -> unallocate_variable;
            reg_alloc[func] = allocator->get_reg_alloc();
            bb_order[func] = allocator->get_block_order();
        }
    }
    std::cout << unallocate_variable << std::endl;
#ifdef DEBUG
    std::cerr << "finish reg alloc\n";
#endif
}

void RegAlloc::execute() {
    compute_block_order();
    number_operations();
    build_intervals();
    walk_intervals();
    set_unused_reg_num();
}

void RegAlloc::compute_block_order() {
    block_order.clear();
    auto entry = func->get_entry_block();
    std::set<BasicBlock*> visited = {};
    get_dfs_order(entry,visited);
}

void RegAlloc::get_dfs_order(BasicBlock *bb, std::set<BasicBlock *> &visited) {
    visited.insert(bb);
    block_order.push_back(bb);
    auto children = bb->get_succ_basic_blocks();
    for(auto child : children){
        auto is_visited = visited.find(child);
        if(is_visited == visited.end()){
            get_dfs_order(child,visited);
        }
    }
}

void RegAlloc::number_operations() {
    int next_id = 0;
    for(auto bb:block_order){
        auto instrs = bb->get_instructions();
        for(auto instr:instrs){
            instr->set_id(next_id);
            next_id += 2;
        }
    }
}

void RegAlloc::build_intervals() {
    for(auto iter = block_order.rbegin();iter != block_order.rend();iter++)
    {
        auto bb = *iter;
        auto instrs = bb->get_instructions();
        int block_from = (*(instrs.begin()))->get_id();
        auto lst_instr = instrs.rbegin();
        int block_to = (*(lst_instr))->get_id() + 2;
        for(auto opr:bb->get_live_out()){
            if((!dynamic_cast<Instruction*>(opr) && !dynamic_cast<Argument*>(opr))||dynamic_cast<AllocaInst *>(opr)){
                continue;
            }
            if(val2Inter.find(opr)==val2Inter.end()){
                auto new_interval = new Interval(opr);
                val2Inter[opr] = new_interval;
            }
            val2Inter[opr]->add_range(block_from,block_to);
        }
        for(auto instr_iter = instrs.rbegin();instr_iter!=instrs.rend();instr_iter++){
            auto instr = *instr_iter;

            if(!instr->is_void()){
                if(dynamic_cast<AllocaInst *>(instr))continue;
                if(val2Inter.find(instr)==val2Inter.end()){
                    auto new_interval = new Interval(instr);
                    new_interval->add_range(block_from,block_to);
                    val2Inter[instr] = new_interval;
                }
                auto cur_inter = val2Inter[instr];
                auto top_range = *(cur_inter->range_list.begin());
                top_range->from = instr->get_id();
                cur_inter->add_use_pos(instr->get_id());
            }

            if(instr->is_phi()){//analyze
                continue;
            }

            for(auto opr:instr->get_operands()){
                if((!dynamic_cast<Instruction*>(opr) && !dynamic_cast<Argument*>(opr))||dynamic_cast<AllocaInst *>(opr)){
                    continue;
                }
                if(val2Inter.find(opr)==val2Inter.end()){
                    auto new_interval = new Interval(opr);
                    val2Inter[opr] = new_interval;
                    new_interval->add_range(block_from,instr->get_id()+2);
                    new_interval->add_use_pos(instr->get_id());
                }
                else{
                    auto cur_inter = val2Inter[opr];
                    cur_inter->add_range(block_from,instr->get_id()+2);
                    cur_inter->add_use_pos(instr->get_id());
                }
            }
        }
    }
    for(auto pair:val2Inter){
#ifdef DEBUG
        std::cerr << "op:" <<pair.first->get_name() << std::endl;
#endif
        add_interval(pair.second);
#ifdef DEBUG
        for(auto range:pair.second->range_list){
            std::cerr << "from: " << range->from << " to: " << range->to << std::endl;
        }
#endif
    }
}

// Greedy find max degree
Interval * Findmax(std::map<Interval *, int> degree_map){
    int max_degree = 0;
    Interval * max_inter;
    for (auto iter = degree_map.begin(); iter != degree_map.end(); iter ++){
        if (iter -> second >= max_degree){
            max_degree = iter -> second;
            max_inter = iter -> first;
        }
    }
    return max_inter;
}

void RegAlloc::walk_intervals() {
    //linear
    if (!register_optimize){
        unallocate_variable = 0;
        std::set<int> used_reg_id = {};
        std::list<Interval *>passed_inter = {};
        int inter_index = 0;
        for(auto i_index = interval_list.begin();i_index != interval_list.end(); i_index++){
            auto i = *i_index;
            //std::cout << "interval" << inter_index << std::endl;
            inter_index ++;
            int range_index = 0;
            for (auto r: i -> range_list){
                //std::cout << "range" << range_index << std::endl;
                //std::cout << "from " << r -> from << " to " << r -> to << std::endl;
                range_index ++;
            }
            std::set<int> avail_reg_id = used_reg_id;
            int alloca_color_flag = 0;
            for (auto inter: passed_inter){
                if (i -> intersects(inter)){
                    avail_reg_id.erase(inter -> reg_num);
                }
                if (!avail_reg_id.size()){
                    if (!unused_reg_id.size()){
                        // register spilled
                        i -> reg_num = -1;
                        unallocate_variable ++;
                        //std::cout << i -> reg_num << std::endl;
                        alloca_color_flag = 1;
                    }
                    else{
                        i -> reg_num = *(unused_reg_id.begin());
                        unused_reg_id.erase(i -> reg_num);
                        used_reg_id.insert(i -> reg_num);
                        //std::cout << i -> reg_num << std::endl;
                        alloca_color_flag = 1;
                    }
                    break;
                }
            }
            if (!alloca_color_flag){
                if (avail_reg_id.size()){
                    i -> reg_num = *(avail_reg_id.begin());
                    //std::cout << i -> reg_num << std::endl;
                }
                else{
                    if (!unused_reg_id.size()){
                        // register spilled
                        i -> reg_num = -1;
                        unallocate_variable ++;
                        //std::cout << i -> reg_num << std::endl;
                    }
                    else{
                        i -> reg_num = *(unused_reg_id.begin());
                        unused_reg_id.erase(i -> reg_num);
                        used_reg_id.insert(i -> reg_num);
                        //std::cout << i -> reg_num << std::endl;
                    }
                }
            }
            passed_inter.push_back(i);
        }
    }
    //std::cout << "hello" << std::endl;
    /*you need to finish this function*/

    else{
        unallocate_variable = 0;
        std::map<Interval *, std::map<Interval *, int>> inter_graph;
        std::map<Interval *, int> degree_map;
        std::stack<Interval *> inter_stack;

        //std::cout << "degree" << std::endl;
        // create the graph
        for(auto i_index = interval_list.begin();i_index != interval_list.end(); i_index++){
            int i_degree = 0;
            //std::cout << "hello" << std::endl;
            for(auto j_index = interval_list.begin(); j_index != interval_list.end();j_index ++){
                auto i = * i_index;
                auto j = * j_index;
                if (i == j){
                    inter_graph[i][j] = 0;
                    continue;
                }
                if (i -> intersects(j)){
                    inter_graph[i][j] = 1;
                    i_degree ++;
                }
                else{
                    inter_graph[i][j] = 0;
                }
            }
            //std::cout << i_degree << std::endl;
            degree_map.insert({*i_index, i_degree});
        }

        // build the stack
        //std::cout << "hello" << std::endl;
        while (degree_map.size()){
            Interval *inter_max = Findmax(degree_map);
            //std::cout << degree_map[inter_max] << std::endl;
            inter_stack.push(inter_max);
            degree_map.erase(inter_max);
            for (auto it = degree_map.begin(); it != degree_map.end(); it ++){
                if (inter_graph[it -> first][inter_max] == 1){
                    degree_map[it -> first] --;
                }
            }
        }

        // color the interval
        std::set<int> used_reg_id = {};

        // initialization
        for (auto i_index = interval_list.begin();i_index != interval_list.end(); i_index++){
            Interval *i = *i_index;
            // represent that this interval is not colored
            i -> reg_num = -1;
        }
        //std::cout << "hello" << std::endl;
        //std::cout << "color" << std::endl;
        while (inter_stack.size()){
            //std::cout << "hello" << std::endl;
            Interval *cur_inter = inter_stack.top();
            std::map<Interval *, int> edges = inter_graph.find(cur_inter) -> second;
            std::set<int> avail_reg_id = used_reg_id;
            for (auto iter = inter_graph.begin();iter != inter_graph.end();iter ++){
                // indicate that this interval is colored
                // new color to be used
                if (!avail_reg_id.size()){
                    if (!unused_reg_id.size()){
                        // register spilled
                        cur_inter -> reg_num = -1;
                        unallocate_variable ++;
                        //std::cout << cur_inter -> reg_num << std::endl;
                    }
                    else{
                        cur_inter -> reg_num = *(unused_reg_id.begin());
                        unused_reg_id.erase(cur_inter -> reg_num);
                        used_reg_id.insert(cur_inter -> reg_num);
                        //std::cout << cur_inter -> reg_num << std::endl;
                    }
                    break;
                }

                if (iter -> first -> reg_num != -1 && inter_graph[iter -> first][cur_inter] == 1){
                    avail_reg_id.erase(iter -> first -> reg_num);
                }

            }
            if (avail_reg_id.size()){
                cur_inter -> reg_num = *(avail_reg_id.begin());
                //std::cout << cur_inter -> reg_num << std::endl;
            }
            inter_stack.pop();
        }
        //std::cout << unallocate_variable << "variables unallocated" << std::endl;
    }
}


void RegAlloc::set_unused_reg_num() {
    func->set_unused_reg_num(unused_reg_id);
}
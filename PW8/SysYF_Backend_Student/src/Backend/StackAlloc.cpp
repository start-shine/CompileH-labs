#include <CodeGen.h>
#include <RegAlloc.h>
#include <string>
#include <stack>

/*
    Code for allocating spaces for the varibles in the function (specified by parameter)
        - parameter `fun`: pointer to the function to be handled
    
    You need to implement this function, according to the stack frame layout stated in the documentation
    
    Here are some tips:
        - Be careful with the order of the arguments to be passed through stack. They are pushed in reversed order,
          which means that a parameter in front will be closer to the stack pointer. 
          For example, the fourth parameter will be closer to the stack pointer than the fifth parameter.
        - Don't forget to reserve the space for callee-saved registers. The number of callee-saved registers
          can be obtained with `used_reg.second.size()`. 
        - ...z
*/

int CodeGen::stack_space_allocation(Function *fun)
{
  int size = 0;

  // std::map<Value *, Interval *> CodeGen::reg_map
  auto _reg_map = &reg_map;   // Hint: use this to get register for values

  // std::map<Value *, IR2asm::Regbase *> CodeGen::stack_map
  stack_map.clear();          // You need to fill in this container to finish allocation

  // std::vector<IR2asm::Regbase *> CodeGen::arg_on_stack
  arg_on_stack.clear();       // You need to maintain this information, the order is the same as parameter

  /* TODO：put your code here */
  //calculate the size
  int arg_space = 0;
  std::map<Value *,int> args;
  if (fun -> get_num_of_args() > 4){
    int arg_index = 0;
    for (auto _arg = fun -> arg_begin(); _arg != fun -> arg_end(); _arg ++){
      if (arg_index >= 4){
        if (reg_map[*_arg] -> reg_num != -1){
          //IR2asm::Regbase * regb = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), reg_size);
          args.insert({*_arg, 0});
          arg_space += reg_size;
        }
        else{
          args.insert({*_arg, 0});
          arg_space += reg_size;
          //std::cout << "hello\n";
        }
      }
      arg_index ++;
    }
  }

  for (auto reg = (*_reg_map).begin(); reg != (*_reg_map).end(); reg ++){
    if (reg -> second -> reg_num == -1 && args.find(reg -> first) == args.end()){
      size += reg_size;
    }
  }

  std::list<Instruction *>instructions = fun->get_entry_block()->get_instructions();
  for (auto inst: instructions){
    if (dynamic_cast<AllocaInst *>(inst) != nullptr){
      if (dynamic_cast<AllocaInst *>(inst) -> get_alloca_type() -> is_array_type()){
        size += dynamic_cast<AllocaInst *>(inst) -> get_alloca_type() -> get_size();
      }
    }
  }

  if (have_temp_reg){
    size += temp_reg_store_num * reg_size;
  }

  if (have_func_call){
    size += reg_size * caller_saved_reg_num;
  }

  int callee_reg_num = used_reg.second.size() + 1;
  int offset = callee_reg_num * reg_size + size + arg_space;

  // maintain the arg_on_stack
  int arg_pos = arg_space;
  std::map<Value *,IR2asm::Regbase *> arg_stack;
  if (fun -> get_num_of_args() > 4){
    //std:: cout << "num of args equals" << fun -> get_num_of_args() << std::endl;
    int arg_index = 0;
    for (auto _arg = fun -> arg_begin(); _arg != fun -> arg_end(); _arg ++){
      if (arg_index >= 4){
        if (reg_map[*_arg] -> reg_num != -1){
          //IR2asm::Regbase * regb = nullptr;
          //std:: cout << "offset"<<offset << std::endl;
          IR2asm::Regbase * regb = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), offset - arg_pos);
          arg_pos -= 4;
          arg_stack.insert({*_arg, regb});
          stack_map.insert({*_arg, regb});
          arg_on_stack.push_back(regb);
          //std::cout << "hello\n";
        }
        else{
          IR2asm::Regbase * regb = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), offset - arg_pos);
          arg_pos -= 4;
          //std:: cout << "offset"<<offset << std::endl;
          arg_stack.insert({*_arg, regb});
          stack_map.insert({*_arg, regb});
          arg_on_stack.push_back(regb);
          //std::cout << "hello\n";
        }
      }
      //std::cout << "hello2\n";
      arg_index ++;
    }
    /*for (auto iter = arg_stack.rbegin(); iter != arg_stack.rend(); iter ++){
      stack_map.insert({iter -> first, iter -> second});
    }*/
  }
  offset = offset -  arg_space;
  //reserve the space for callee-saved registers
  offset = offset - callee_reg_num * reg_size;

  // allocate the space for the spilled registers
  for (auto reg = (*_reg_map).begin(); reg != (*_reg_map).end(); reg ++){
    if (reg -> second -> reg_num == -1 && arg_stack.find(reg -> first) == arg_stack.end()){
      IR2asm::Regbase * regb = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), offset - 4);
      offset = offset - 4;
      stack_map.insert({reg -> first, regb});
    }
  }

  // allocate the space for the arrays
  for (auto inst: instructions){
    if (dynamic_cast<AllocaInst *>(inst) != nullptr){
      if (dynamic_cast<AllocaInst *>(inst) -> get_alloca_type() -> is_array_type()){
        IR2asm::Regbase * regb = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), offset - dynamic_cast<AllocaInst *>(inst) -> get_alloca_type() -> get_size());
        offset -= dynamic_cast<AllocaInst *>(inst) -> get_alloca_type() -> get_size();
        stack_map.insert({inst, regb});
      }
    }
  }

  // allocate the space for the temp registers
  if (have_temp_reg){
    offset -= temp_reg_store_num * reg_size;
  }

  // allocate the space for the caller-saved registers
  if (have_func_call){
    offset -= reg_size * caller_saved_reg_num;
  }
  //std::cout << "size" <<size << std::endl;
  return size;
}
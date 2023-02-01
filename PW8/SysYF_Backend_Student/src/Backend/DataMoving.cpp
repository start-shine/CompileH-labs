#include <CodeGen.h>
#include <string>

/*
    code for moving data from src list to dst list:
        - parameter `src` contains data sources, they can be constant (immediate),
          register or address on stack (address specified by base register and offset)
        - parameter `dst` contains data destinations, they can be register
          or address on stack (address specified by base register and offset)
        - parameter `cmpop` is used to generate instructions with conditional excution method

    you should generate code for data moving, the method for moving single data from a source location (or const)
    to a destination location is given, parameters:
        - `src_loc` is data source location, can be constant (immediate),
          register or address on stack (address specified by base register and offset)
        - parameter `dst` contains data destinations, they can be register
          or address on stack (address specified by base register and offset)
        - parameter `reg_tmp` is the temporary register used for data moving under situations like
            - moving data from stack location to another (this function doesn't distinguish if the location is the same)
            - moving constant value (immediate) to a stack location
            - ...
        - parameter `cmpop` is used to generate instructions with conditional excution method
    the function may generate more than one instruction.
*/

std::string CodeGen::data_move(std::vector<IR2asm::Location *> &src,
                               std::vector<IR2asm::Location *> &dst,
                               std::string cmpop)
{

    std::string code;
    if (moving_optimize)
    {
        bool lr_need = false;
        for (int i = 0; i < dst.size(); i++)
        {
            if (dynamic_cast<IR2asm::RegLoc *>(dst[i]))
                lr_need = false;
            else
            {
                lr_need = true;
                break;
            }
        }
        bool have_same = false;
        for (int i = 0; i < src.size(); i++)
        {
            auto src_loc = src[i];
            for (int j = 0; j < dst.size(); j++)
            {
                auto target_loc = dst[j];
                if (src_loc->get_code() == target_loc->get_code())
                {
                    have_same = true;
                    break;
                }
            }
            if (have_same)
                break;
        }
        bool move = 0;
        IR2asm::Location *head;
        bool circle = false;
        if (have_same)
        {
            for (int i = 0; i < dst.size(); i++)
            {
                if (!move)
                {
                    head = src[i];
                }
                move = 0;
                auto target_loc = dst[i];
                for (int j = i + 1; j < src.size(); j++)
                {
                    auto src_loc = src[j];
                    if (target_loc->get_code() == src_loc->get_code())
                    {
                        if (dst[j]->get_code() == head->get_code())
                        {
                            circle = true;
                            break;
                        }
                        auto target_move = dst[j];
                        dst.erase(dst.begin() + j);
                        dst.insert(dst.begin(), target_move);
                        src.erase(src.begin() + j);
                        src.insert(src.begin(), src_loc);
                        move = 1;
                        continue;
                    }
                }
                if (circle)
                {
                    break;
                }
                if (move)
                {
                    i = 0;
                    continue;
                }
            }
        }

        if (!have_same || !circle)
        {
            if (lr_need)
            {
                code += IR2asm::space;
                code += "push {lr}";
                code += IR2asm::endl;
            }
            for (int i = 0; i < src.size(); i++)
            {
                auto src_loc = src[i];
                auto target_loc = dst[i];
                IR2asm::Reg *reg_tmp = new IR2asm::Reg(IR2asm::lr);
                code += single_data_move(src_loc, target_loc, reg_tmp, cmpop);
            }
            if (lr_need)
            {
                code += IR2asm::space;
                code += "pop {lr}";
                code += IR2asm::endl;
            }
            return code;
        }
        if (lr_need)
        {
            code += IR2asm::space;
            code += "push {lr}";
            code += IR2asm::endl;
        }
        for (int i = 0; i < src.size(); i++)
        {
            auto src_loc = src[i];
            IR2asm::Location *target_loc = new IR2asm::RegLoc(IR2asm::lr);
            IR2asm::Reg *reg_tmp = new IR2asm::Reg(IR2asm::lr);
            code += single_data_move(src_loc, target_loc, reg_tmp, cmpop);
            code += IR2asm::space;
            code += "push {lr}";
            code += IR2asm::endl;
        }
        for (int i = dst.size() - 1; i >= 0; i--)
        {
            auto target_loc = dst[i];
            code += IR2asm::space;
            code += "pop {lr}";
            code += IR2asm::endl;
            IR2asm::Reg *reg_tmp = new IR2asm::Reg(IR2asm::lr);
            IR2asm::Location *src_loc = new IR2asm::RegLoc(IR2asm::lr);
            code += single_data_move(src_loc, target_loc, reg_tmp, cmpop);
        }
        if (lr_need)
        {
            code += IR2asm::space;
            code += "pop {lr}";
            code += IR2asm::endl;
        }
    }

    else
    {
        code += IR2asm::space;
        code += "push {lr}";
        code += IR2asm::endl;
        for (int i = 0; i < src.size(); i++)
        {
            auto src_loc = src[i];
            IR2asm::Reg *reg_tmp = new IR2asm::Reg(IR2asm::lr);
            IR2asm::Location *target_loc = new IR2asm::RegLoc(IR2asm::lr);
            code += single_data_move(src_loc, target_loc, reg_tmp, cmpop);
            code += IR2asm::space;
            code += "push {lr}";
            code += IR2asm::endl;
        }
        for (int i = dst.size() - 1; i >= 0; i--)
        {
            auto target_loc = dst[i];
            code += IR2asm::space;
            code += "pop {lr}";
            code += IR2asm::endl;
            IR2asm::Reg *reg_tmp = new IR2asm::Reg(IR2asm::lr);
            IR2asm::Location *src_loc = new IR2asm::RegLoc(IR2asm::lr);
            code += single_data_move(src_loc, target_loc, reg_tmp, cmpop);
        }
        code += IR2asm::space;
        code += "pop {lr}";
        code += IR2asm::endl;
    }
    return code;
}

std::string CodeGen::single_data_move(IR2asm::Location *src_loc,
                                      IR2asm::Location *target_loc,
                                      IR2asm::Reg *reg_tmp,
                                      std::string cmpop)
{
    std::string code;
    if (dynamic_cast<IR2asm::RegLoc *>(src_loc))
    {
        auto regloc = dynamic_cast<IR2asm::RegLoc *>(src_loc);
        if (regloc->is_constant())
        {
            if (dynamic_cast<IR2asm::RegLoc *>(target_loc))
            {
                auto target_reg_loc = dynamic_cast<IR2asm::RegLoc *>(target_loc);
                code += IR2asm::space;
                code += "Ldr" + cmpop + " ";
                code += target_reg_loc->get_code();
                code += ", =";
                code += std::to_string(regloc->get_constant());
                code += IR2asm::endl;
            }
            else
            {
                code += IR2asm::space;
                code += "Ldr" + cmpop + " ";
                code += reg_tmp->get_code();
                code += ", =";
                code += std::to_string(regloc->get_constant());
                code += IR2asm::endl;
                code += IR2asm::safe_store(reg_tmp, target_loc, sp_extra_ofst, long_func, cmpop);
            }
        }
        else
        {
            if (dynamic_cast<IR2asm::RegLoc *>(target_loc))
            {
                auto target_reg_loc = dynamic_cast<IR2asm::RegLoc *>(target_loc);
                code += IR2asm::space;
                code += "Mov" + cmpop + " ";
                code += target_reg_loc->get_code();
                code += ", ";
                code += regloc->get_code();
                code += IR2asm::endl;
            }
            else
            {
                bool is_sp_based = false;
                IR2asm::Regbase *regbase = dynamic_cast<IR2asm::Regbase *>(target_loc);
                is_sp_based = (regbase->get_reg().get_id() == 13);
                int offset = regbase->get_offset() + ((is_sp_based) ? sp_extra_ofst : 0);
                if (regloc->get_reg_id() == 14 && abs(offset) > 4095)
                {
                    // push r0
                    code += IR2asm::space;
                    code += "push {r0}";
                    code += IR2asm::endl;
                    // lr->r0
                    code += IR2asm::space;
                    code += "Mov r0, lr";
                    code += IR2asm::endl;
                    // r0->target_loc : safe_store
                    code += IR2asm::safe_store(new IR2asm::Reg(0),
                                               target_loc, sp_extra_ofst, long_func, cmpop);
                    // pop r0
                    code += IR2asm::space;
                    code += "pop {r0}";
                    code += IR2asm::endl;
                }
                else
                    code += IR2asm::safe_store(new IR2asm::Reg(regloc->get_reg_id()),
                                               target_loc, sp_extra_ofst, long_func, cmpop);
            }
        }
    }
    else
    {
        auto stackloc = dynamic_cast<IR2asm::Regbase *>(src_loc);
        if (dynamic_cast<IR2asm::RegLoc *>(target_loc))
        {
            auto target_reg_loc = dynamic_cast<IR2asm::RegLoc *>(target_loc);
            code += IR2asm::safe_load(new IR2asm::Reg(target_reg_loc->get_reg_id()),
                                      stackloc, sp_extra_ofst, long_func, cmpop);
        }
        else
        {
            code += IR2asm::safe_load(reg_tmp, stackloc, sp_extra_ofst, long_func, cmpop);
            code += IR2asm::safe_store(reg_tmp, target_loc, sp_extra_ofst, long_func, cmpop);
        }
    }
    return code;
}
//
// Created by 杨丰硕 on 2023/4/20.
//
#include "Module.h"
#include "GlobalVariable.h"
#include "IrDumper.h"
#include "Function.h"
#include "BasicBlock.h"
#include "Constant.h"
#include "Instruction.h"

IrDumper::IrDumper(std::ostream &out):
    out_(out){

}

IrDumper::~IrDumper() = default;

std::string IrDumper::getBasicType(Instruction *inst) const {
    return inst->getBasicType() == BasicType::INT_BTYPE ? "i32" : "float";
}

std::string IrDumper::getBasicType(BasicType basic_type) const {
    return basic_type == BasicType::INT_BTYPE ? "i32" : "float";
}

std::string IrDumper::dumpValue(Value *value) const {
    auto to_const = dynamic_cast<ConstantVar *>(value);
    if (to_const) {
        return to_const->isFloat() ?
            std::to_string(to_const->getFValue()) : std::to_string(to_const->getIValue());
    } else {
        std::string is_ptr_ch = value->isPtr() ? "*" : "";
        return is_ptr_ch + "%" + value->getName();
    }
}

std::string IrDumper::getOptype(Instruction *inst) const {
    auto inst_type = inst->getInstType();
    switch (inst_type) {
        case InstructionType::AddType:
            return "add";
        case InstructionType::SubType:
            return "sub";
        case InstructionType::MulType:
            return "mul";
        case InstructionType::DivType:
            return "div";
        case InstructionType::ModType:
            return "mod";
        case InstructionType::CmpLType:
            return "cmpl";
        case InstructionType::CmpLeType:
            return "cmple";
        case InstructionType::CmpGType:
            return "cmpg";
        case InstructionType::CmpGeType:
            return "cmpge";
        case InstructionType::CmpEqType:
            return "cmpeq";
        case InstructionType::CmpNEqType:
            return "cmpneq";
    }
    return "";
}

void IrDumper::dump(Module *module) {
    for (const auto &global: module->global_variables_) {
        dump(global.get());
    }
    for (const auto &function: module->functions_) {
        dump(function.get());
    }
}

void IrDumper::dump(GlobalVariable *global) {
    assert(global);
    out_ << "@" << global->getName() << " = ";
    if (global->isConst()) {
        out_ << "constant ";
    } else {
        out_ << "global ";
    }
    dump(global->getConstInit());
    out_ << "\n";
}

void IrDumper::dump(Function *function) {
    std::string ret_type;
    switch (function->getRetType()) {
        case BasicType::INT_BTYPE:
            ret_type = "int";
            break;
        case BasicType::FLOAT_BTYPE:
            ret_type = "float";
            break;
        case BasicType::VOID_BTYPE:
            ret_type = "void";
            break;
    }
    out_ << "define " << ret_type << " @" << function->getName() << "(";
    auto argument_size = function->getArgumentSize();
    for (int i = 0; i < argument_size; ++i) {
        auto arg = function->getArgument(i);
        std::string type_str = arg->isFloat() ? "float" : "int32";
        out_ << type_str;
        // 一维数组
        auto dimension_size = arg->getArraySize();
        for (int j = 0; j < dimension_size; j++) {
            if (j == 0) {
                out_ << "[]";
            } else {
                out_ << "[" << arg->getDimensionByIdx(j) << "]";
            }
        }
        out_ << " %" << arg->getName();
        if (i != argument_size - 1) {
            out_ << ", ";
        }
    }
    out_ << ") {\n";
    auto &blocks = function->getBlocks();
    for (const auto &block: blocks) {
        dump(block.get());
    }
    out_ << "}\n";
}

void IrDumper::dump(Constant *constant) {
    assert(constant);
    BasicType basic_type;
    auto const_array = dynamic_cast<ConstantArray *>(constant);
    if (const_array) {
        out_ << "[";
        auto &initvalue_map = const_array->getInitValueMap();
        basic_type = const_array->getBasicType();
        int32_t last_idx = -1;
        for (auto &[key, value]: initvalue_map) {
            if (key - last_idx != 1) {
                out_ << "zero init:[" << last_idx + 1 << "," << key << ") ";
            } else {
                out_ << getBasicType(basic_type) << " %" << key << ": ";
                if (value->isFloat()) {
                    out_ << value->getFValue();
                } else {
                    out_ << value->getIValue();
                }
                out_ << " ";
            }
            last_idx = key;
        }
        size_t array_len = const_array->getArrayLen();
        if (last_idx != array_len - 1) {
            out_ << "zero init:[" << last_idx + 1 << "," << array_len << ") ";
        }
        out_ << "]";
    } else {
        auto const_value = dynamic_cast<ConstantVar *>(constant);
        assert(const_value);
        basic_type = constant->getBasicType();
        if (basic_type == BasicType::INT_BTYPE) {
            out_ << "i32 " << const_value->getIValue();
        } else {
            out_ << "float " << const_value->getFValue();
        }
    }
}

void IrDumper::dump(Instruction *inst) {
    out_ << "   ";
    switch (inst->getInstType()) {
        case StoreType:
            dump(dynamic_cast<StoreInstruction *>(inst));
            return;
        case AllocaType:
            dump(dynamic_cast<AllocaInstruction *>(inst));
            return;
        case LoadType:
            dump(dynamic_cast<LoadInstruction *>(inst));
            return;
        case RetType:
            dump(dynamic_cast<RetInstruction *>(inst));
            return;
        case GEPType:
            dump(dynamic_cast<GEPInstruction *>(inst));
            return;
        case MemSetType:
            dump(dynamic_cast<MemSetInstruction *>(inst));
            return;
    }
    if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
        dump(binary_inst);
    }
    if (auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst); unary_inst) {
        dump(unary_inst);
    }

}

void IrDumper::dump(BinaryOpInstruction *binst) {
    out_ << dumpValue(binst) << " = " << getOptype(binst) <<
        " " << getBasicType(binst) << " " << dumpValue(binst->getLeft())
        << "," << dumpValue(binst->getRight()) << '\n';
}

void IrDumper::dump(UnaryOpInstruction *uinst) {
    out_ << dumpValue(uinst) << " = " << getOptype(uinst) <<
        " " << getBasicType(uinst) << " " << dumpValue(uinst->getValue())
        << "\n";
}

void IrDumper::dump(StoreInstruction *inst) {
    std::string basic_type = getBasicType(inst);
    std::string value_name = dumpValue(inst->getValue());

    out_ << "store " << basic_type << " " << value_name << ", %" << inst->getPtr()->getName();
    out_ << '\n';
}

void IrDumper::dump(LoadInstruction *inst) {
    std::string basic_type = getBasicType(inst);
    std::string value_name = dumpValue(inst->getPtr());
    out_ << dumpValue(inst) << " = load " << basic_type << " " << value_name;
    out_ << '\n';
}

void IrDumper::dump(AllocaInstruction *inst) {
    out_ << "%" << inst->getName() << " = alloca ";
    if (inst->getBasicType() == BasicType::INT_BTYPE) {
        out_ << "i32";
    } else {
        out_ << "float";
    }
    if (inst->isArray()) {
        out_ << " array len: " << inst->getValueSize();
    }
    out_ << '\n';
}

void IrDumper::dump(RetInstruction *inst) {
    out_ << "return ";
    if (!inst->isRetVoid()) {
        out_ << dumpValue(inst->getRetValue());
    }
    out_ << '\n';
}

void IrDumper::dump(BasicBlock *block) {
    out_ << block->getName() << ":\n";
    auto &inst_list = block->getInstructionList();
    for (const auto &inst : inst_list) {
        dump(inst.get());
    }
}

void IrDumper::dump(MemSetInstruction *inst) {
    out_ << "memset " << getBasicType(inst->getBasicType()) << " %*" << inst->getBase()->getName() << ", offset:";
    auto offset_const = dynamic_cast<ConstantVar *>(inst->getSize());
    if (offset_const) {
        out_ << offset_const->getIValue();
    } else {
        out_ << "%" << inst->getSize()->getName();
    }
    out_ << ", value:";
    auto init_value = dynamic_cast<ConstantVar *>(inst->getValue());
    if (init_value) {
        if (inst->getBasicType() == BasicType::INT_BTYPE) {
            out_ << init_value->getIValue();
        } else {
            out_ << init_value->getFValue();
        }
    } else {
        out_ << "%" << inst->getValue()->getName();
    }
    out_ << '\n';
}

void IrDumper::dump(GEPInstruction *inst) {
    out_ << "%" << inst->getName() << " = getelemptr " << getBasicType(inst->getBasicType())
     << " %" << inst->getPtr()->getName() << " offset:";
    auto const_offset = dynamic_cast<ConstantVar *>(inst->getOffset());
    if (const_offset) {
        out_ << const_offset->getIValue();
    } else {
        out_ << "%" << inst->getOffset()->getName();
    }
    out_ << " * 4\n";
}




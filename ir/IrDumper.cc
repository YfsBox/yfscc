//
// Created by 杨丰硕 on 2023/4/20.
//
#include <stack>
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
    return getBasicType(inst->getBasicType());
}

std::string IrDumper::getBasicType(BasicType basic_type) const {
    if (basic_type == BasicType::INT_BTYPE) {
        return "i32";
    } else if (basic_type == BasicType::FLOAT_BTYPE) {
        return "float";
    } else if (basic_type == BasicType::VOID_BTYPE) {
        return "void";
    } else {
        return "i32*";
    }
}

std::string IrDumper::dumpValue(Value *value) const {
    auto to_const = dynamic_cast<ConstantVar *>(value);
    if (to_const) {
        return to_const->isFloat() ?
               std::to_string(to_const->getFValue()) : std::to_string(to_const->getIValue());
    } else if (dynamic_cast<GlobalVariable *>(value)) {
        return "@" + value->getName();
    } else if (value != nullptr) {
        return "%" + value->getName();
    }
    return "";
}

std::string IrDumper::dumpValue(BasicType basic_type, Value *value) const {
    assert(value);
    std::string ptr_ch = value->isPtr() ? "* " : " ";
    return getBasicType(basic_type) + ptr_ch + dumpValue(value);
}


std::string IrDumper::getCmpCondType(SetCondInstruction *inst) const {
    switch (inst->getCmpType()) {
        case SetCondInstruction::SetGT:
            return "sgt";
        case SetCondInstruction::SetLT:
            return "slt";
        case SetCondInstruction::SetGE:
            return "sge";
        case SetCondInstruction::SetLE:
            return "sle";
        case SetCondInstruction::SetNE:
            return "ne";
        case SetCondInstruction::SetEQ:
            return "eq";
    }
    return "";
}

std::string IrDumper::getArrayType(const std::vector<int32_t> &dimension, BasicType basic_type) {
    std::string array_type;
    if (dimension.empty()) {
        return array_type;
    }
    if (dimension[0] == Argument::ArrayArgumentNullIdx) {
        array_type += getBasicType(basic_type);
        if (basic_type != BasicType::VOIDPTR_BTYPE) {
            array_type += "*";
        }
    } else {
        array_type += " ";
        for (int i = 0; i < dimension.size(); ++i) {
            array_type += "[" + std::to_string(dimension[i]) + " x ";
        }
        array_type += getBasicType(basic_type);
        for (int i = 0; i < dimension.size(); ++i) {
            array_type += "]";
        }
    }
    return array_type;
}

std::string IrDumper::getVarType(BasicType btype, Value *value, bool isptr) {
    return "";
}

std::string IrDumper::getOptype(Instruction *inst, BasicType basic_type) const {
    auto inst_type = inst->getInstType();
    std::string optype;
    switch (inst_type) {
        case InstructionType::AddType:
            optype = "add";
            break;
        case InstructionType::SubType:
            optype = "sub";
            break;
        case InstructionType::MulType:
            optype = "mul";
            break;
        case InstructionType::DivType:
            optype = "div";
            break;
        case InstructionType::ModType:
            optype = "srem";
            break;
    }
    if (basic_type == BasicType::INT_BTYPE && inst_type == InstructionType::DivType) {
        optype = "s" + optype;
    } else if (basic_type == BasicType::FLOAT_BTYPE) {
        optype = "f" + optype;
    }
    return optype;
}

void IrDumper::dump(Module *module) {
    for (const auto &decl: module->lib_func_decl_) {
        out_ << "declare " << getBasicType(decl->getRetType()) << " @" << decl->getName() << "(";
        for (int i = 0; i < decl->getArgumentSize(); ++i) {
            auto argument = decl->getArgument(i);
            if (argument->isArray()) {      // 获取该formal的类型
                out_ << getArrayType(argument->getDimension(), argument->getBasicType()) << " " << dumpValue(argument);
            } else {
                out_ << dumpValue(argument->getBasicType(), argument);
            }
            if (i != decl->getArgumentSize() - 1) {
                out_ << ", ";
            }
        }
        out_ << ") #1\n\n";
    }

    for (const auto &global: module->global_variables_) {
        dump(global.get());
    }
    out_ << "\n";
    for (const auto &function: module->functions_) {
        dump(function.get());
        out_ << '\n';
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
            ret_type = "i32";
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
        std::string type_str = getBasicType(arg->getBasicType());
        out_ << type_str;
        // 一维数组
        auto dimension_size = arg->getArraySize();
        if (dimension_size > 1) {
            for (int j = 0; j < dimension_size; j++) {
                if (j == 0) {
                    out_ << "[]";
                } else {
                    out_ << "[" << arg->getDimensionByIdx(j) << "]";
                }
            }
        } else {
            out_ << "*";
        }
        out_ << " %" << arg->getName();
        if (i != argument_size - 1) {
            out_ << ", ";
        }
    }
    out_ << ") #0 {\n";
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
        out_ << getArrayType(const_array->getDimensionNumbers(), const_array->getBasicType()) << "zeroinitializer\n";
        /*
        out_ << "[";
        auto &initvalue_map = const_array->getInitValueMap();
        basic_type = const_array->getBasicType();
        int32_t last_idx = -1;
        for (auto &[key, value]: initvalue_map) {
            if (key - last_idx != 1) {
                out_ << "zero init:[" << last_idx + 1 << ", " << key << ") ";
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
            out_ << "zero init:[" << last_idx + 1 << ", " << array_len << ") ";
        }
        out_ << "]";*/
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
    assert(inst);
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
        case BrType:
            dump(dynamic_cast<BranchInstruction *>(inst));
            return;
        case SetCondType:
            dump(dynamic_cast<SetCondInstruction *>(inst));
            return;
        case CallType:
            dump(dynamic_cast<CallInstruction *>(inst));
            return;
        case ZextType:
            dump(dynamic_cast<ZextInstruction *>(inst));
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
    out_ << dumpValue(binst) << " = " << getOptype(binst, binst->getBasicType()) <<
        " " << dumpValue(binst->getBasicType(), binst->getLeft())
        << ", " << dumpValue( binst->getRight()) << '\n';
}

void IrDumper::dump(UnaryOpInstruction *uinst) {
    if (uinst->getInstType() == NegType) {
        out_ << dumpValue(uinst) << " = sub nsw " << getBasicType(uinst->getBasicType()) << " 0, " <<
        dumpValue(uinst->getValue()) << '\n';
    } else if (uinst->getInstType() == NotType) {
        out_ << dumpValue(uinst) << " = xor i1 " << dumpValue(uinst->getValue()) << ", true\n";
    } else {
        /*out_ << dumpValue(uinst) << " = " << getOptype(uinst) <<
             " " << dumpValue(uinst->getBasicType(), uinst->getValue())
             << "\n";*/
    }
}

void IrDumper::dump(StoreInstruction *inst) {
    out_ << "store " << getBasicType(inst->getBasicType()) <<  inst->getValue() << ", " << dumpValue(inst->getBasicType(), inst->getPtr());
    out_ << '\n';
}

void IrDumper::dump(LoadInstruction *inst) {
    out_ << dumpValue(inst) << " = load " << getBasicType(inst->getBasicType()) << ", " << dumpValue(inst->getBasicType(), inst->getPtr());
    out_ << '\n';
}

void IrDumper::dump(AllocaInstruction *inst) {
    out_ << "%" << inst->getName() << " = alloca ";
    if (inst->isArray()) {
        out_ << getArrayType(inst->getArrayDimensionSize(), inst->getBasicType());
    } else {
        if (inst->getBasicType() == BasicType::INT_BTYPE) {
            out_ << "i32";
        } else {
            out_ << "float";
        }
    }
    out_ << '\n';
}

void IrDumper::dump(RetInstruction *inst) {
    out_ << "ret ";
    if (!inst->isRetVoid()) {
        out_ << dumpValue(inst->getBasicType(), inst->getRetValue());
    }
    out_ << '\n';
}

void IrDumper::dump(BasicBlock *block) {
    if (block->getInstructionList().empty()) {
        return;
    }
    out_ << block->getName() << ":";
    out_ << "                       ;preds = ";
    for (auto pre: block->getPreDecessorBlocks()) {
        out_ << dumpValue(pre) << " ";
    }
    out_ << ";successors = ";
    for (auto succ : block->getSuccessorBlocks()) {
        out_ << dumpValue(succ) << " ";
    }
    out_ << '\n';
    auto &inst_list = block->getInstructionList();
    for (const auto &inst : inst_list) {
        dump(inst.get());
    }
}

void IrDumper::dump(MemSetInstruction *inst) {
    out_ << "memset " << getBasicType(inst->getBasicType()) << " " << dumpValue(inst->getBase()) << ", size:";
    out_ << dumpValue(inst->getSize()) << ", value:";
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
    assert(inst && inst->getPtr());
    out_ << "%" << inst->getName() << " = getelementptr ";
    if (!inst->isUseOffset()) {
        std::vector<int32_t> dimension_numbers;
        Value *array_var = nullptr;
        auto array_base = dynamic_cast<AllocaInstruction *>(inst->getPtr());
        if (array_base) {
            dimension_numbers = array_base->getArrayDimensionSize();
            array_var = array_base;
        } else {
            auto global_var = dynamic_cast<GlobalVariable *>(inst->getPtr());
            auto var_array = dynamic_cast<ConstantArray *>(global_var->getConstInit());
            if (var_array) {
                dimension_numbers = var_array->getDimensionNumbers();
                array_var = global_var;
            }
        }
        out_ << getArrayType(dimension_numbers, inst->getBasicType()) << ","
             << getArrayType(dimension_numbers, inst->getBasicType()) << "* "
             << dumpValue(array_var);
        out_ << ", i32 0, ";

        for (int i = 0; i < inst->getIndexSize(); i++) {
            auto index_value = inst->getIndexValue(i);
            out_ << dumpValue(BasicType::INT_BTYPE, index_value);
            if (i != inst->getIndexSize() - 1) {
                out_ << ", ";
            }
        }
    } else {
        out_ << getBasicType(inst->getBasicType()) << ", " << getBasicType(inst->getBasicType()) << "* " << dumpValue(inst->getPtr()) << ", i32 " << dumpValue(inst->getOffset());
    }
    out_ << "\n";
}

void IrDumper::dump(BranchInstruction *inst) {
    out_ << "br ";
    if (inst->isCondBranch()) {
        out_ << "i1 " << dumpValue(inst->getCond()) << ", label " << dumpValue(inst->getTrueLabel()) << ", label "
                                   << dumpValue(inst->getFalseLabel());
    } else {
        out_ << "label " << dumpValue(inst->getLabel());
    }
    out_ << "\n";
}

void IrDumper::dump(SetCondInstruction *inst) {
    std::string cmp_type = inst->isFloatCmp() ? "float" : "i32";
    std::string cmp_str = inst->isFloatCmp() ? "fcmp" : "icmp";
    out_ << dumpValue(inst) << " = " << cmp_str << " " << getCmpCondType(inst) << " " << cmp_type << " "
    << dumpValue(inst->getLeft()) << ", " << dumpValue(inst->getRight()) << "\n";
}

void IrDumper::dump(CallInstruction *inst) {
    if (inst->getBasicType() != BasicType::VOID_BTYPE) {
        out_ << dumpValue(inst) << " = ";
    }
    out_ << "call " << getBasicType(inst->getBasicType()) << " @" << inst->getFunction()->getName();
    out_ << "(";
    auto actual_size = inst->getActualSize();
    auto call_function = inst->getFunction();
    for (int i = 0; i < actual_size; ++i) {
        auto actual = inst->getActual(i);
        auto argument = call_function->getArgument(i);
        if (argument->isArray()) {      // 获取该formal的类型
            out_ << getArrayType(argument->getDimension(), argument->getBasicType()) <<  " " << dumpValue(actual);;
        } else {
            out_ << getBasicType(argument->getBasicType()) << " " << dumpValue(actual);
        }
        if (i != actual_size - 1) {
            out_ << ", ";
        }
    }
    out_ << ")\n";
}

void IrDumper::dump(ZextInstruction *inst) {
    out_ << dumpValue(inst) << " = zext i1 " << dumpValue(inst->getValue()) << " to i32\n";
}


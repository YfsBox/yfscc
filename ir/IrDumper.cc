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
        std::string value_str;
        if (to_const->isFloat()) {
            char dstr[20];
            float fvalue = to_const->getFValue();
            snprintf(dstr, 20, "%.8f", fvalue);
            value_str.assign(dstr);
        } else {
            value_str = std::to_string(to_const->getIValue());
        }
        return value_str;
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
    std::string cmp_cond_type;
    switch (inst->getCmpType()) {
        case SetCondInstruction::SetGT:
            cmp_cond_type = "gt";
            break;
        case SetCondInstruction::SetLT:
            cmp_cond_type = "lt";
            break;
        case SetCondInstruction::SetGE:
            cmp_cond_type = "ge";
            break;
        case SetCondInstruction::SetLE:
            cmp_cond_type = "le";
            break;
        case SetCondInstruction::SetNE:
            cmp_cond_type = "ne";
            break;
        case SetCondInstruction::SetEQ:
            cmp_cond_type = "eq";
            break;
    }
    std::string cmp_flag;
    if (inst->isFloatCmp()) {
        cmp_flag = "u";
    } else {
        if (inst->getCmpType() == SetCondInstruction::SetEQ || inst->getCmpType() == SetCondInstruction::SetNE) {
            return cmp_cond_type;
        }
        cmp_flag = "s";
    }
    return cmp_flag + cmp_cond_type;
}

std::string IrDumper::getArrayType(const std::vector<int32_t> &dimension, BasicType basic_type) {
    std::string array_type;
    if (dimension.empty()) {
        return getBasicType(basic_type);
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

std::string IrDumper::getStrTypeAsOperand(Value *value) {
    auto inst = dynamic_cast<Instruction *>(value);
    if (inst) {
        return getStrTypeAsOperand(inst);
    }
    auto arg = dynamic_cast<Argument *>(value);
    if (arg) {
        return getStrTypeAsOperand(arg);
    }
    auto global = dynamic_cast<GlobalVariable *>(value);
    if (global) {
        return getStrTypeAsOperand(global);
    }
    auto const_value = dynamic_cast<ConstantVar *>(value);
    if (const_value) {
        return getBasicType(const_value->getBasicType());
    }
    return "";
}

std::string IrDumper::getStrTypeAsOperand(Instruction *inst) {
    std::string operand_str;
    auto alloca_inst = dynamic_cast<AllocaInstruction *>(inst);
    auto load_inst = dynamic_cast<LoadInstruction *>(inst);
    if (alloca_inst) {
        if (alloca_inst->isArray()) {
            operand_str += getArrayType(alloca_inst->getArrayDimensionSize(), inst->getBasicType());
        } else {
            operand_str += getBasicType(inst->getBasicType());
        }
        if (alloca_inst->isPtrPtr()) {
            operand_str += "*";
        }
    } else if (load_inst) {
        operand_str = getStrTypeAsOperand(load_inst->getPtr());
        if (operand_str.back() == '*') {
            operand_str.pop_back();
        }
    } else {
        operand_str += getBasicType(inst->getBasicType());
    }
    return operand_str;
}

void IrDumper::setGlobalArrayInitListMap(ConstantArray *const_array, std::vector<int32_t> &init_list_map) {
    init_list_map.resize(const_array->getArrayLen(), 0);
    for (auto &[key, value]: const_array->getInitValueMap()) {
        init_list_map[key] = dynamic_cast<ConstantVar *>(value.get())->getIValue();
    }
}

void IrDumper::dumpGlobalArrayInitListMap(BasicType btype, std::vector<int32_t> dimension, std::vector<int32_t> &init_list_map, int l, int r) {
    if (l + 1 == r) {
        std::string type_str = btype == BasicType::INT_BTYPE ? "i32" : "float";
        out_ << type_str << " " << init_list_map[l];
        return;
    } else if (!dimension.empty()) {
        int step = (r - l + 1) / dimension.front();
        std::vector<int32_t> next_dimension = std::vector<int32_t>(dimension.begin() + 1, dimension.end());
        if (step != 1) {
            out_ << getArrayType(next_dimension, btype);
        }
        out_ << " [";
        for (int i = l; i < r; i += step) {
            dumpGlobalArrayInitListMap(btype, next_dimension, init_list_map, l, l + step);
            if (i + step != r) {
                out_ << ",";
            }
        }
        out_ << "]";
    }
}

std::string IrDumper::getStrTypeAsOperand(Argument *argument) {
    std::string operand_str;
    if (argument->isArray()) {
        operand_str += getArrayType(argument->getDimension(), argument->getBasicType());
    } else {
        operand_str += getBasicType(argument->getBasicType());
    }
    if (argument->isPtrArg()) {
        operand_str += "*";
    }
    return operand_str;
}

std::string IrDumper::getStrTypeAsOperand(GlobalVariable *global) {
    std::string operand_str;
    auto global_value = global->getConstInit();
    auto value_array = dynamic_cast<ConstantArray *>(global_value);
    if (value_array) {
        operand_str += getArrayType(value_array->getDimensionSizeNumbers(), global->getBasicType());
    } else {
        operand_str += getBasicType(global->getBasicType());
    }
    return operand_str;
}


void IrDumper::dump(Module *module) {
    for (const auto &decl: module->lib_func_decl_) {
        out_ << "declare " << getBasicType(decl->getRetType()) << " @" << decl->getName() << "(";
        for (int i = 0; i < decl->getArgumentSize(); ++i) {
            auto argument = decl->getArgument(i);
            out_ << getStrTypeAsOperand(argument);
            if (i != decl->getArgumentSize() - 1) {
                out_ << ", ";
            }
        }
        out_ << ")\n";
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
        if (!arg->isArray()) {
            out_ << type_str;
        } else {
            out_ << getArrayType(arg->getDimension(), arg->getBasicType());
        }
        if (arg->isPtrArg()) {
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
        out_ << getArrayType(const_array->getDimensionNumbers(), const_array->getBasicType()) << " zeroinitializer";
        std::vector<int32_t> array_init_list;
    } else {
        auto const_value = dynamic_cast<ConstantVar *>(constant);
        assert(const_value);
        basic_type = constant->getBasicType();
        if (basic_type == BasicType::INT_BTYPE) {
            out_ << "i32 " << dumpValue(const_value);
        } else {
            out_ << "float " << dumpValue(const_value);
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
        case CastType:
            dump(dynamic_cast<CastInstruction *>(inst));
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
    out_ << "store " << getStrTypeAsOperand(inst->getValue()) << " " << dumpValue(inst->getValue()) << ", "
    << getStrTypeAsOperand(inst->getPtr()) << "* " << dumpValue(inst->getPtr());
    out_ << '\n';
}

void IrDumper::dump(LoadInstruction *inst) {
    std::string str_type = getStrTypeAsOperand(inst->getPtr());
    out_ << dumpValue(inst) << " = load " << str_type << ", " << str_type << "* " << dumpValue(inst->getPtr());
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
    if (inst->isPtrPtr()) {     // 如果是指向指针的指针
        out_ << "*";
    }
    out_ << '\n';
}

void IrDumper::dump(RetInstruction *inst) {
    out_ << "ret ";
    if (!inst->isRetVoid()) {
        out_ << dumpValue(inst->getBasicType(), inst->getRetValue());
    } else {
        out_ << "void";
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
    out_ << ";while loop depth is " << block->getWhileLoopDepth();
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

    std::string strtype = getArrayType(inst->getArrayDimension(), inst->getBasicType());
    out_ << strtype << ","
    << strtype << "* "
    << dumpValue(inst->getPtr());
    out_ << ", ";
    /*if (strtype != "i32" && strtype != "float" && !inst->isPtrOffset()) {           // 维度数组不是空的,并且不是基于offset的
        out_ << "i32 0, ";
    }*/
    for (int i = 0; i < inst->getIndexSize(); i++) {
        auto index_value = inst->getIndexValue(i);
        out_ << dumpValue(BasicType::INT_BTYPE, index_value);
        if (i != inst->getIndexSize() - 1) {
            out_ << ", ";
        }
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
        out_ << getStrTypeAsOperand(argument) << " " << dumpValue(actual);
        if (i != actual_size - 1) {
            out_ << ", ";
        }
    }
    out_ << ")\n";
}

void IrDumper::dump(ZextInstruction *inst) {
    out_ << dumpValue(inst) << " = zext i1 " << dumpValue(inst->getValue()) << " to i32\n";
}

void IrDumper::dump(CastInstruction *inst) {
    out_ << dumpValue(inst) << " = ";
    if (inst->isF2I()) {
        out_ << "fptosi float " << dumpValue(inst->getValue()) << " to i32";
    } else {
        out_ << "sitofp i32 " << dumpValue(inst->getValue()) << " to float";
    }
    out_ << '\n';
}


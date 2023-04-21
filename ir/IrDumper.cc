//
// Created by 杨丰硕 on 2023/4/20.
//
#include "Module.h"
#include "GlobalVariable.h"
#include "IrDumper.h"
#include "Function.h"
#include "BasicBlock.h"
#include "Instruction.h"

IrDumper::IrDumper(std::ostream &out):
    out_(out){

}

IrDumper::~IrDumper() = default;

void IrDumper::dump(Module *module) {
    for (const auto &global: module->global_variables_) {
        dump(global.get());
    }
    for (const auto &function: module->functions_) {
        dump(function.get());
    }
}

void IrDumper::dump(GlobalVariable *global) {
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
    BasicType basic_type;
    if (dynamic_cast<ConstantArray *>(constant)) {

    } else {
        auto const_value = dynamic_cast<ConstantVar *>(constant);
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
    }
    if (auto binary_inst = dynamic_cast<BinaryOpInstruction *>(inst); binary_inst) {
        dump(binary_inst);
    }
    if (auto unary_inst = dynamic_cast<UnaryOpInstruction *>(inst); unary_inst) {
        dump(unary_inst);
    }

}

void IrDumper::dump(BinaryOpInstruction *binst) {

}

void IrDumper::dump(UnaryOpInstruction *uinst) {

}

void IrDumper::dump(StoreInstruction *inst) {
    std::string basic_type = inst->getValueType() == BasicType::INT_BTYPE ? "i32" : "float";
    std::string value_name = "%" + inst->getValue()->getName();
    if (value_name == "%") {
        auto const_var = dynamic_cast<ConstantVar *> (static_cast<Value *>(inst->getValue()));
        if (!const_var->isFloat()) {
            value_name = std::to_string(const_var->getIValue());
        } else {
            value_name = std::to_string(const_var->getFValue());
        }
    }

    out_ << "store " << basic_type << " " << value_name << ", *%" << inst->getPtr()->getName();
    out_ << '\n';
}

void IrDumper::dump(LoadInstruction *inst) {

}

void IrDumper::dump(AllocaInstruction *inst) {
    out_ << "%" << inst->getName() << " = alloca ";
    if (inst->getValueType() == BasicType::INT_BTYPE) {
        out_ << "i32";
    } else {
        out_ << "float";
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



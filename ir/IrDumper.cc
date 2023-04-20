//
// Created by 杨丰硕 on 2023/4/20.
//
#include "Module.h"
#include "GlobalVariable.h"
#include "IrDumper.h"
#include "Function.h"

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



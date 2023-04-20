//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_GLOBALVARIABLE_H
#define YFSCC_GLOBALVARIABLE_H

#include <memory>
#include "Constant.h"
#include "Value.h"

class Constant;

class GlobalVariable: public Value {
public:

    using ConstantPtr = std::unique_ptr<Constant>;

    GlobalVariable(bool is_const, const std::string &name);

    GlobalVariable(bool is_const, int32_t initval, const std::string &name);

    GlobalVariable(bool is_const, float initval, const std::string &name);

    GlobalVariable(bool is_const, bool is_float, int dimension, const std::string &name);

    ~GlobalVariable();


    Constant *getConstInit() const {
        return const_init_.get();
    }

    bool isConst() const {
        return is_const_;
    }

    BasicType getBasicType() const {
        return const_init_->getBasicType();
    }

private:
    bool is_const_;
    ConstantPtr const_init_;
};


#endif //YFSCC_GLOBALVARIABLE_H

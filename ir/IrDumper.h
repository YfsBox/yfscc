//
// Created by 杨丰硕 on 2023/4/20.
//

#ifndef YFSCC_IRDUMPER_H
#define YFSCC_IRDUMPER_H

#include <iostream>

class Module;
class GlobalVariable;
class Function;

class IrDumper {
public:
    explicit IrDumper(std::ostream &out);

    ~IrDumper();

    void dump(Module *module);

    void dump(GlobalVariable *global);

    void dump(Function *function);

    void dump(Constant *constant);


private:
    std::ostream &out_;
};

#endif //YFSCC_IRDUMPER_H

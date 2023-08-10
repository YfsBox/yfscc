//
// Created by 杨丰硕 on 2023/8/7.
//

#ifndef YFSCC_ENDRENAME_H
#define YFSCC_ENDRENAME_H

#include "PassManager.h"

// 只适合作为最后一个pass
class EndRename: public Pass {
public:

    explicit EndRename(Module *module): Pass(module), basic_block_label_no_(-1), value_no_(-1) {
        pass_name_ = "EndRename";
    }

    ~EndRename() = default;

protected:

    void runOnFunction() override;

private:

    void init();

    int basic_block_label_no_;

    int value_no_;

};


#endif //YFSCC_ENDRENAME_H

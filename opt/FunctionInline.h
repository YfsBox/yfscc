//
// Created by 杨丰硕 on 2023/6/14.
//

#ifndef YFSCC_FUNCTIONINLINE_H
#define YFSCC_FUNCTIONINLINE_H

#include <memory>
#include <list>
#include "CallGraphAnalysis.h"
#include "PassManager.h"
#include "../ir/BasicBlock.h"

class FunctionInline: public Pass {
public:

    using BasicBlockUptr = std::unique_ptr<BasicBlock>;

    explicit FunctionInline(Module *module);

    ~FunctionInline() = default;

protected:
    void runOnFunction() override;  // 这里的runOnFunction并不是针对某个function的，而是整个module的所有
private:

    void copyFunction(Function *function, std::list<BasicBlockUptr> &copyed_bbs);

    static const constexpr int32_t inline_insts_size = 200;

    bool canBeInline(Function *function);

    void inlineOnFunction(Function *function);

    std::unique_ptr<CallGraphAnalysis> call_graph_analysis_;
};


#endif //YFSCC_FUNCTIONINLINE_H

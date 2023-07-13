//
// Created by 杨丰硕 on 2023/7/13.
//

#ifndef YFSCC_SPLITGEPINSTS_H
#define YFSCC_SPLITGEPINSTS_H

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "PassManager.h"
#include "../ir/BasicBlock.h"

class Module;
class Instruction;
class GEPInstruction;

class SplitGEPInsts: public Pass {
public:

    using InstsSet = std::vector<Instruction *>;

    explicit SplitGEPInsts(Module *module);

    ~SplitGEPInsts() = default;

protected:

    void runOnFunction() override;

private:

    void findAndSplit();

    void insertAndReplace();

    std::unordered_map<Instruction*, InstsSet> split_insts_map_;

    std::unordered_map<Instruction*, BasicBlock::InstructionListIt> insert_point_map_;

};


#endif //YFSCC_SPLITGEPINSTS_H

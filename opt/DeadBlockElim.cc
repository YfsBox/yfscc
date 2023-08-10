//
// Created by 杨丰硕 on 2023/6/14.
//
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "DeadBlockElim.h"
#include "../ir/Module.h"
#include "../ir/BasicBlock.h"

DeadBlockElim::DeadBlockElim(Module *module): Pass(module), enter_block_(nullptr) {
    pass_name_ = "DeadBlockElim";
}

void DeadBlockElim::runOnFunction() {
    enter_block_ = curr_func_->getBlocks().front().get();
    assert(enter_block_);


    std::unordered_map<BasicBlock *, int> prebb_cnt_map;
    std::queue<BasicBlock *> bb_q;
    std::unordered_set<BasicBlock *> remove_bb_set;
    for (auto &bbuptr: curr_func_->getBlocks()) {
        auto bb = bbuptr.get();
        prebb_cnt_map[bb] = bb->getPreDecessorBlocks().size();
        if (prebb_cnt_map[bb] == 0 && bb != enter_block_) {
            bb_q.push(bb);
            remove_bb_set.insert(bb);
        }
    }

    while (!bb_q.empty()) {
        auto front_bb = bb_q.front();
        bb_q.pop();
        for (auto succ_bb: front_bb->getSuccessorBlocks()) {
            prebb_cnt_map[succ_bb]--;
            if (prebb_cnt_map[succ_bb] == 0) {
                bb_q.push(succ_bb);
                remove_bb_set.insert(succ_bb);
            }
        }
    }

    auto &bb_blocks = curr_func_->getBlocks();

    for (auto &bb_uptr: bb_blocks) {
        auto bb = bb_uptr.get();
        if (!remove_bb_set.count(bb)) {
            auto &pre_bbs = bb->getPreDecessorBlocks();
            for (auto remove_bb: remove_bb_set) {
                pre_bbs.erase(remove_bb);
            }
        }
    }

    // 首先移除phi指令中
    for (auto &bb_uptr: bb_blocks) {
        auto bb = bb_uptr.get();
        for (auto &inst_uptr: bb->getInstructionList()) {
            if (auto phi_inst = dynamic_cast<PhiInstruction *>(inst_uptr.get()); phi_inst) {
                phi_inst->remove(remove_bb_set);
            }
        }
    }

    for (auto bb_it = bb_blocks.begin(); bb_it != bb_blocks.end();) {
        auto bb = bb_it->get();
        if (remove_bb_set.count(bb)) {
            // printf("delete block %s in function %s\n", bb->getName().c_str(), curr_func_->getName().c_str());
            bb_it = bb_blocks.erase(bb_it);
        } else {
            ++bb_it;
        }
    }

}
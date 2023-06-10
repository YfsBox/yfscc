// 已经将所有的样例都跑通
#include <algorithm>
#include "LoopSearch.h"
#include "LoopInvHoist.h"

void LoopInvHoist::run()
{
    // 先通过LoopSearch获取循环的相关信息
    LoopSearch loop_searcher(m_, false);
    loop_searcher.run();

    for (auto func : m_->get_functions())
    {
        for (auto basicbockset : loop_searcher.get_loops_in_func(func))
        {
            while (1)
            {
                std::unordered_set<Value *> appear;
                for (auto bb : *basicbockset)
                {
                    for (auto ins : bb->getInstructionList())
                        appear.insert(ins);
                }
                bool tag = false;
                for (auto bb : *basicbockset)
                {
                    for (auto ins : bb->getInstructionList())
                    {
                        auto deal = *loop_searcher.get_loop_base(basicbockset)->getPreDecessorBlocks().begin();
                        auto term = deal->get_terminator();
                        if (!(ins->is_phi() || ins->is_br() || ins->is_call() || ins->is_load() || ins->is_store()))
                        {
                            bool change = true;
                            // 右侧的操作数是否又在左侧被使用
                            for (auto val : ins->get_operands())
                            {
                                if (appear.find(val) != appear.end())
                                {
                                    change = false;
                                    break;
                                }
                            }
                            if (change == true)
                            {
                                appear.erase(ins);
                                deal->getInstructionList().remove(term);
                                deal->add_instruction(ins);
                                ins->set_parent(deal);
                                deal->add_instruction(term);
                                bb->getInstructionList().remove(ins);
                                tag = true;
                                break;
                            }
                        }
                    }
                    if (tag == true)
                        break;
                }
                if (tag == true)
                    continue;
                break;
            }
        }
    }
}

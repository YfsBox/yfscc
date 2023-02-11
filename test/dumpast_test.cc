//
// Created by 杨丰硕 on 2023/2/11.
//
#include <gtest/gtest.h>
#include "../common/Utils.h"

TEST(DUMPREFIX_TEST, DUMPPREFIX_TEST) {
    for (size_t i = 0; i < 10; ++i) {
        dumpPrefix(std::cout, i);
        std::cout << '\n';
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}




add_executable(dumpast_test dumpast_test.cc ../common/Utils.cc)
target_link_libraries(dumpast_test gtest pthread)
import os
import sys
import glob
import subprocess

test_file = "2022_performance"

if __name__ == "__main__":
    yfscc_test_dir = "yfscc_test"
    if not os.path.exists(yfscc_test_dir):
        os.makedirs(yfscc_test_dir)
    test_cnt = 0
    for file in glob.glob("2022_performance/*.sy"):
        test_cnt = test_cnt + 1
        has_test_in = True
        name_prefix = file[len(test_file) + 1: len(file) - 3]
        #print(name_prefix)

        try:
            complie_result = subprocess.run(["./compiler", "-S", "-O1", "-o", "{}/{}.s".format(yfscc_test_dir, name_prefix) , file], stdout = subprocess.PIPE, timeout=180)
        except subprocess.TimeoutExpired:
            error_test_list.append(test_name)
            print(Fore.RED + "[ERROR]the test {} is timeout when commplie.".format(name_prefix) + Style.RESET_ALL)
            continue

    print(test_cnt)
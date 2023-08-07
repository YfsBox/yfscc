import os
import sys
import glob
import subprocess
import tempfile
from colorama import init, Fore, Style

test_file = "performance"
all_test_number = 0
passed_test_number = 0
error_test_list = []

def compare_files(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        # 逐行读取文件内容，并且对比每一行是否一致
        for line1, line2 in zip(f1, f2):
            if line1 != line2:
                #print("{} {}".format(line1, line2))
                return False
        if f1.readline() != f2.readline():
            return False
    return True

if __name__ == "__main__":
    yfscc_test_dir = "yfscc_test"
    if not os.path.exists(yfscc_test_dir):
        os.makedirs(yfscc_test_dir)
    test_cnt = 0
    for file in glob.glob("yfscc_test/*.s"):
        test_cnt = test_cnt + 1
        has_test_in = True
        name_prefix = file[len("yfscc_test") + 1: len(file) - 2]
        #print(name_prefix)

        print(Fore.BLUE + "[INFO]begin test {}.".format(name_prefix) + Style.RESET_ALL)

        test_in_file = "performance/{}.in".format(name_prefix)
        #print(test_in_file)
        if not os.path.exists(test_in_file):
            #print("not has this file")
            has_test_in = False

        linked_result = subprocess.run(["gcc", file, "libsysy_armv7l.a", "-static", "-o", "test_o"], stdout = subprocess.PIPE)

        if has_test_in == False:
            #print("run in here......")
            qemu_run_result = subprocess.Popen(["./test_o"], stdout=subprocess.PIPE)
            qemu_run_output, error = qemu_run_result.communicate()
        else:
            #print("run in here with in......")
            with open(test_in_file, "r") as test_in_f:
                test_in_content = test_in_f.read()


            qemu_run_result = subprocess.Popen(["./test_o"], stdin = subprocess.PIPE, stdout=subprocess.PIPE, shell=False)
            qemu_run_output, error = qemu_run_result.communicate(input = test_in_content.encode())
        #print(qemu_run_output.decode('utf-8'))
        qemu_run_retcode = qemu_run_result.returncode

        with tempfile.NamedTemporaryFile(mode='w', delete=False) as tmpf:
            tmpf.write(qemu_run_output.decode('utf-8'))
            tmpf.write("{}\n".format(qemu_run_retcode))

        test_out_file = "performance/{}.out".format(name_prefix)
        if compare_files(tmpf.name, test_out_file) == True:
            passed_test_number += 1
            print(Fore.GREEN + "[RIGHT]the test {} is right.".format(name_prefix) + Style.RESET_ALL)
        else:
            error_test_list.append(test_name)
            print(Fore.RED + "[ERROR]the test {} is error.".format(name_prefix) + Style.RESET_ALL)


    print(test_cnt)
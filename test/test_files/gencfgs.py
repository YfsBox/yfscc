import os
import glob
import shutil

function_cfgs = "function_cfgs"
hidden_functional_cfgs = "hidden_functional_cfgs"


if __name__ == "__main__":

    if not os.path.exists(function_cfgs):
        os.makedirs(function_cfgs)

    if not os.path.exists(hidden_functional_cfgs):
        os.makedirs(hidden_functional_cfgs)

    os.chdir("./functional")
    for file in glob.glob("*.sy"):
        #print(file)
        shutil.copy(file, "../{}/{}.c".format(function_cfgs, file))
    
    os.chdir("../{}".format(function_cfgs))

    for file in glob.glob("*.sy.c"):
        file_len = len(file)
        prefix = file[:file_len - 5]
        if not os.path.exists(prefix):
            os.makedirs(prefix)
        os.chdir(prefix)
        os.system("sudo clang -O0 -S -emit-llvm ../{} -o another.ll".format(file))
        os.system("opt -dot-cfg another.ll")
        for file in glob.glob(".*.dot"):
            os.system("dot -Tpng {} -o {}.png".format(file, file[1:]))
        os.chdir("..")
        
    
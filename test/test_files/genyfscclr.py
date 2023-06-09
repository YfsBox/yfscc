import os
import sys
import glob

yfscc_path = "../../build/yfscc"
functional_yfscc = "functional_yfscc"
functional_path = "functional"

if __name__ == "__main__":
    
    if len(sys.argv) < 1:
        exit("")
    
    file_no = sys.argv[1]

    if len(sys.argv) >= 2 and sys.argv[1] == 'h':
        functional_path = "hidden_functional"
        functional_yfscc = "hidden_functional_yfscc"
        file_no = sys.argv[2]

    if len(file_no) <= 1:
        file_no = "0{}".format(file_no)
    
    print("the file no is {}".format(file_no))

    if not os.path.exists(functional_yfscc):
        os.makedirs(functional_yfscc) 
    
    for file in glob.glob("./{}/{}_*.sy".format(functional_path, file_no)):
        print("the file is {}".format(file))
        os.system("sudo cat {} > another.c".format(file))
        os.system("sudo {} another.c > tmp.ll".format(yfscc_path))
        out_file_dir = "{}/{}".format(functional_yfscc, file_no)
        if not os.path.exists(out_file_dir):
            os.makedirs(out_file_dir)

        os.chdir(out_file_dir)
        os.system("opt -dot-cfg ../../tmp.ll")
        for dotf in glob.glob(".*.dot"):
            os.system("dot -Tpng {} -o {}.png".format(dotf, dotf[1:]))

        #os.system("llvm-as {} -o tmp.bc".format("../../tmp.ll"))
        #os.system("llvm-link tmp.bc ../../../../libsysy.a -o program.bc")





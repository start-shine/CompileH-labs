#!/usr/bin/env python3
# 这是一个用于ARM评测的生成ARM汇编代码的脚本框架，仅供参考，请根据需求自行修改
# 该脚本运行在希冀评测平台上
import subprocess
import os
import shutil

if __name__ == '__main__':
    # 通过已有优化，编译程序得到汇编代码的命令，格式为 './compiler {编译选项} {输入程序文件路径} -o {输出汇编文件路径}'
    baseline_asm_gen_cmd = './compiler {} {} -o {}'
    # 加上你的优化，编译程序得到汇编代码的命令，格式为 './compiler {编译选项} {输入程序文件路径} -o {输出汇编文件路径}'
    optimized_asm_gen_cmd = './compiler {} {} -o {}'
    a = 1
    # 修改当前的python的工作目录
    os.chdir('../../build')
    try:
        shutil.rmtree('../test/student/Test')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Easy')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Medium')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Hard')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Test_H')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Easy_H')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Medium_H')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Hard_H')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Special_Reg')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/Special_Reg_Optimize')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/DataMoving')
    except:
        a = 0
    try:
        shutil.rmtree('../test/student/DataMoving_Optimize')
    except:
        a = 0
    shutil.copytree('../test/Test/Easy/', '../test/student/Test/Easy')
    shutil.copytree('../test/Test/Medium/', '../test/student/Test/Medium')
    shutil.copytree('../test/Test/Hard/', '../test/student/Test/Hard')
    shutil.copytree('../test/Test_H/Easy_H/', '../test/student/Test_H/Easy_H')
    shutil.copytree('../test/Test_H/Medium_H/', '../test/student/Test_H/Medium_H')
    shutil.copytree('../test/Test_H/Hard_H/', '../test/student/Test_H/Hard_H')
    shutil.copytree('../test/Test/Special_Reg/', '../test/student/Test/Special_Reg')
    shutil.copytree('../test/Test/DataMoving/', '../test/student/Test/DataMoving')
    os.mkdir('../test/student/Easy')
    os.mkdir('../test/student/Medium')
    os.mkdir('../test/student/Hard')
    os.mkdir('../test/student/Easy_H')
    os.mkdir('../test/student/Medium_H')
    os.mkdir('../test/student/Hard_H')
    os.mkdir('../test/student/Special_Reg')
    os.mkdir('../test/student/Special_Reg_Optimize')
    os.mkdir('../test/student/DataMoving')
    os.mkdir('../test/student/DataMoving_Optimize')
    # 收集所有的`../test/student`目录下的测试样例
    TEST_DIRS = [
                '../test/Test/Easy/',
                '../test/Test/Medium/',
                '../test/Test/Hard/',
                '../test/Test_H/Easy_H/',
                '../test/Test_H/Medium_H/',
                '../test/Test_H/Hard_H/',
                '../test/Test/Special_Reg/',
                '../test/Test/DataMoving'
                ]
    i = 0
    normal_variable_unallocate = 0
    optimize_variable_unallocate = 0
    normal_datamoving_lines = 0
    optimize_datamoving_lines = 0
    for test_dirs in TEST_DIRS:
        for file in os.listdir(test_dirs):
            if file.endswith('.sy'):
                i = i+1
                source_file = os.path.join(test_dirs, file)
                file = str(file)
                file_name = file[0:len(file)-3]
                #print(i,file_name)
        # 执行编译生成汇编
        # 注意生成的汇编代码一定要放在`../test/student`目录下，只有该目录将被打包发送到树莓派上进行可执行程序生成和实际运行
                folder = test_dirs.split('/')[3]
                folder = '../test/student/' + folder
                asm_file = os.path.join(folder,file_name+'.s')
                if folder != '../test/student/Special_Reg':
                    return_val = subprocess.run(baseline_asm_gen_cmd.format('-S -g', source_file, asm_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).stdout
                if folder == '../test/student/Special_Reg':
                    return_val = subprocess.run(baseline_asm_gen_cmd.format('-S', source_file, asm_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).stdout
                if folder == '../test/student/DataMoving':
                    normal_datamoving_lines += len(open(asm_file,'r').readlines())
                folder = folder + '_Optimize'
                #print(folder)
                asm_optimize_file = os.path.join(folder,file_name+'.s')
                #print(i, asm_file)
                if folder == '../test/student/Special_Reg_Optimize':
                    normal_variable_unallocate += int(str(return_val)[2:len(str(return_val)) - 3])
                #print(i, asm_optimize_file)
                if folder == '../test/student/Special_Reg_Optimize':
                    return_optimize_val = subprocess.run(baseline_asm_gen_cmd.format('-S -g', source_file, asm_optimize_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).stdout
                    optimize_variable_unallocate += int(str(return_optimize_val)[2:len(str(return_optimize_val)) - 3])
                if folder == '../test/student/DataMoving_Optimize':
                    return_optimize_val = subprocess.run(baseline_asm_gen_cmd.format('-S -m', source_file, asm_optimize_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).stdout
                    optimize_datamoving_lines += len(open(asm_optimize_file,'r').readlines())
    # subprocess.run(optimized_asm_gen_cmd.format('', '', '../test/student/demo_opt.s'), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print('normal_variable_unallocate ' + str(normal_variable_unallocate))
    print('optimize_variable_unallocate ' + str(optimize_variable_unallocate))
    print('average_lines_reduction ' + str((normal_datamoving_lines - optimize_datamoving_lines) / 5))
    
import subprocess
import os
import time

if __name__ == '__main__':
    baseline_asm_list = []
    baseline_exe_list = []
    normal_Easy_H_time = time.perf_counter()
    for file in os.listdir('Easy_H'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Easy_H', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Easy_H', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test_H/Easy_H'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test_H/Easy_H', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test_H/Easy_H/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_Easy_H_time = time.perf_counter() - normal_Easy_H_time

    normal_Medium_H_time = time.perf_counter()
    baseline_asm_list = []
    baseline_exe_list = []
    for file in os.listdir('Medium_H'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Medium_H', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Medium_H', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test_H/Medium_H'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test_H/Medium_H', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test_H/Medium_H/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_Medium_H_time = time.perf_counter() - normal_Medium_H_time

    normal_Hard_H_time = time.perf_counter()
    baseline_asm_list = []
    baseline_exe_list = []
    for file in os.listdir('Hard_H'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Hard_H', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Hard_H', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test_H/Hard_H'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test_H/Hard_H', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test_H/Hard_H/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_Hard_H_time = time.perf_counter() - normal_Hard_H_time





    baseline_asm_list = []
    baseline_exe_list = []
    normal_Easy_time = time.perf_counter()
    for file in os.listdir('Easy'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Easy', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Easy', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test/Easy'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test/Easy', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test/Easy/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_Easy_time = time.perf_counter() - normal_Easy_time

    normal_Medium_time = time.perf_counter()
    baseline_asm_list = []
    baseline_exe_list = []
    for file in os.listdir('Medium'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Medium', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Medium', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test/Medium'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test/Medium', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test/Medium/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_Medium_time = time.perf_counter() - normal_Medium_time

    normal_Hard_time = time.perf_counter()
    baseline_asm_list = []
    baseline_exe_list = []
    for file in os.listdir('Hard'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Hard', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Hard', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test/Hard'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test/Hard', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test/Hard/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_Hard_time = time.perf_counter() - normal_Hard_time

    baseline_asm_list = []
    baseline_exe_list = []
    normal_Special_Reg_time = time.perf_counter()
    for file in os.listdir('Special_Reg'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Special_Reg', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Special_Reg', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test/Special_Reg'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test/Special_Reg', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test/Special_Reg/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_Special_Reg_time = time.perf_counter() - normal_Special_Reg_time

    baseline_asm_list = []
    baseline_exe_list = []
    optimize_Special_Reg_time = time.perf_counter()
    for file in os.listdir('Special_Reg_Optimize'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('Special_Reg_Optimize', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('Special_Reg_Optimize', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test/Special_Reg'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test/Special_Reg', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test/Special_Reg/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    optimize_Special_Reg_time = time.perf_counter() - optimize_Special_Reg_time

    print('normal_Special_Reg_time ', normal_Special_Reg_time)
    print('optimize_Special_Reg_time ', optimize_Special_Reg_time)


    baseline_asm_list = []
    baseline_exe_list = []
    normal_DataMoving_time = time.perf_counter()
    for file in os.listdir('DataMoving'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('DataMoving', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('DataMoving', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test/DataMoving'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test/DataMoving', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test/DataMoving/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    normal_DataMoving_time = time.perf_counter() - normal_DataMoving_time

    baseline_asm_list = []
    baseline_exe_list = []
    optimize_DataMoving_time = time.perf_counter()
    for file in os.listdir('DataMoving_Optimize'):
        if file.endswith('.s'):
            file_name = str(file)
            asm_file = os.path.join('DataMoving_Optimize', file_name)
            baseline_asm_list.append(asm_file)
            file_name = file_name[0:len(file_name) - 2]
            #print(file_name)
            exec_file = os.path.join('DataMoving_Optimize', file_name)
            baseline_exe_list.append(exec_file)

    gcc_compile_cmd = 'gcc -march=armv7-a -g sylib.c {} -o {}'
    for asm_file, exe_file in zip(baseline_asm_list, baseline_exe_list):
        #print(str(asm_file))
        #print(str(exe_file))
        subprocess.run(gcc_compile_cmd.format(asm_file, exe_file), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    exe_cmd = 'cat {} | ./{} ; echo $?'
    for baseline_exe in baseline_exe_list:
        exe_name = str(baseline_exe) + '.in'
        exe_name = exe_name.split('/')[1]
        #print(exe_name)
        in_name = 'empty.in'
        for exe_file in os.listdir('Test/DataMoving'):
            #print(str(exe_file))
            if (str(exe_file) == exe_name):
                in_name = exe_name
        in_file = os.path.join('Test/DataMoving', in_name)
        result1 = subprocess.run(exe_cmd.format(in_file, baseline_exe), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(in_file)
        stdout_con = str(result1.stdout)
        stdout_con = stdout_con[2:len(stdout_con) - 1]
        stdout_con = stdout_con.split('\\')
        #print(baseline_exe)
        file_in = open('Test/DataMoving/' +str(baseline_exe).split('/')[1] + '.out', mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None, closefd=True, opener=None)
        index = 0
        out_line = file_in.readline().strip('\n').strip('\r')
        file_str = ""
        passed = 1
        while len(out_line):
            file_str += out_line
            out_line = file_in.readline().strip('\n').strip('\r')
        str1 = ""
        index = 0
        for str2 in stdout_con:
            if index > 0:
                str1 += str2[1:len(str2)]
            else:
                str1 += str2
                index = 1
        #print(str1)
        #print(file_str)
        if str1 == file_str:
            print(baseline_exe + '\033[0;32;40m passed !\033[0m')
        else:
            print(baseline_exe + '\033[0;31;40m failed !\033[0m')
    optimize_DataMoving_time = time.perf_counter() - optimize_DataMoving_time

    print('normal_DataMoving_time ', normal_DataMoving_time)
    print('optimize_DataMoving_time ', optimize_DataMoving_time)
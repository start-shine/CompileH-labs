# 实验报告

## 必做部分

### 支配树

#### B1-1

（1）若 x, y 支配 b，可以推知，从流图起点开始，每条到达 b 的路径都要经过 x 和 y。

（2）若 x 不支配 y，这就说明从起点到 y 的至少有一条路径里面不经过 x。这时若是 y 不支配 x 那么，就说明存在起点到 x 里面的一条路径里面不经过 y。

（3）由于（1）中结论，故而可以知道要么 y 到 b 中有一条不经过 x 的路径，要么 x 到 b 中有一条不经过 y 的路径。

（4）不妨假设（3）中存在的是一条 y 到 b 中不经过 x 的路径，这时如（2）中所说有一条从起点到 y 的不经过 x 的路径，那么就会存在一条起点到 b 中不经过 x 的路径，这与（1）中推论矛盾。

（5）推出（2）中假设是错误的，所以要么 x 支配 y，要么 y 支配 x。

#### B1-2

是不一定需要的，因为无论从下往上还是从上往下，在 n = n 的时候，介于在文章中 n = n 是起点，所以都会有一次更新，所以如果是从下往上的话，顶多就是慢一个周期收敛。

#### B1-3

是一定需要的，因为它在计算直接支配节点的时候，会需要拿两个前继的点进行支配节点的回溯，从而找到那个共同的前驱，这时就说明整个过程需要是自上而下的。而在文章中节点 n 为起始节点，故而，就需要逆序。

#### B1-4

Intersect 的作用是找到两个前驱节点共同的前驱节点，然后加入当前节点的 doms，这里也就是尝试找到当前节点的直接支配节点。

此处的小于号不能改成大于号，因为这里默认了起点节点的编号为最大，而且有前驱的编号大于当前节点的编号这一个特点，故而如果小于号改成大于号相当于是会引起正反馈。

#### B1-5

时间上：由于只需要算出直接支配节点而不是所有的支配节点，故而导致收敛的速度更快了。

空间上：同样由于只需要存储直接支配节点，而不是所有的支配节点，故而空间上的开销也变小了。

#### B1-6

它会通过以下这一个式子来判别它是不是 exit：

```c++
if(terminate_instr->is_ret()){
    exit_block = bb;
    break;
}
```

这样以后会将 exit 的 block 存到变量`exit_block`里面。其实简而言之就是看看那个块里头最后一个指令是不是 return 语句。

因为进入 exit 节点之后，程序就彻底结束，但是流图的最后一个节点并不一定是一个结束节点，它可能处在一个循环中，故而就不能拿流图的最后一个块作为 exit。

### Mem2Reg

#### B2-1

- 首先就是 insideBlockForwarding，也就是找到块之前的值传递关系，并对变量进行编号，同时删除一些冗余指令。
- 然后是 genPhi，这一步是用来生成 Phi 指令。
- 之后是 set_print_name，也就是重新命名。
- 之后是 valueDefineCounting，就是计量一下接下来会出现在 value_status 里面的 value 各有那些，并且将其和 block 对应。
- 之后是 valueForwarding，这里重新梳理了值的传递过程，删去了冗余的指令，以及做了对应的替换。
- removeAlloc 这里是删除了冗余的 alloca 指令。

#### B2-2

例子：

```
int a[20];

int ReadArray(int n){
    int count = 0;
    while (count < n){
        a[count] = getint();
        count = count + 1;
    }
    return 0;
}

int Partition(int l, int r){
    int x = a[r];
    int i = l;
    int j = l;
    while (j < r){
        if (a[j] < x){
            int temp = a[j];
            a[j] = a[i];
            a[i] = temp;
            i = i + 1;
        }
        j = j + 1;
    }
    a[r] = a[i];
    a[i] = x;
    return i;
}

int QuickSort(int l, int r){
    int mid_point = Partition(l, r);
    if (l < mid_point - 1)
        QuickSort(l, mid_point - 1);
    if (r > mid_point + 1)
        QuickSort(mid_point + 1, r);
    return 0;
}

int main(){
    int n;
    n = getint();
    ReadArray(n);
    QuickSort(0, n - 1);
    putarray(n, a);
    return 0;
}
```

优化前的 ll：

```
define i32 @ReadArray(i32 %arg0) {
label_entry:
  %op1 = alloca i32
  %op2 = alloca i32
  store i32 %arg0, i32* %op2
  %op3 = alloca i32
  store i32 0, i32* %op3
  br label %label5
label_ret:                                                ; preds = %label17
  %op4 = load i32, i32* %op1
  ret i32 %op4
label5:                                                ; preds = %label_entry, %label11
  %op6 = load i32, i32* %op3
  %op7 = load i32, i32* %op2
  %op8 = icmp slt i32 %op6, %op7
  %op9 = zext i1 %op8 to i32
  %op10 = icmp ne i32 %op9, 0
  br i1 %op10, label %label11, label %label17
label11:                                                ; preds = %label5
  %op12 = call i32 @get_int()
  %op13 = load i32, i32* %op3
  %op14 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op13
  store i32 %op12, i32* %op14
  %op15 = load i32, i32* %op3
  %op16 = add i32 %op15, 1
  store i32 %op16, i32* %op3
  br label %label5
label17:                                                ; preds = %label5
  store i32 0, i32* %op1
  br label %label_ret
}
define i32 @Partition(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = alloca i32
  %op3 = alloca i32
  store i32 %arg0, i32* %op3
  %op4 = alloca i32
  store i32 %arg1, i32* %op4
  %op5 = alloca i32
  %op6 = load i32, i32* %op4
  %op7 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op6
  %op8 = load i32, i32* %op7
  store i32 %op8, i32* %op5
  %op9 = alloca i32
  %op10 = load i32, i32* %op3
  store i32 %op10, i32* %op9
  %op11 = alloca i32
  %op12 = load i32, i32* %op3
  store i32 %op12, i32* %op11
  %op13 = alloca i32
  br label %label15
label_ret:                                                ; preds = %label29
  %op14 = load i32, i32* %op2
  ret i32 %op14
label15:                                                ; preds = %label_entry, %label53
  %op16 = load i32, i32* %op11
  %op17 = load i32, i32* %op4
  %op18 = icmp slt i32 %op16, %op17
  %op19 = zext i1 %op18 to i32
  %op20 = icmp ne i32 %op19, 0
  br i1 %op20, label %label21, label %label29
label21:                                                ; preds = %label15
  %op22 = load i32, i32* %op11
  %op23 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op22
  %op24 = load i32, i32* %op23
  %op25 = load i32, i32* %op5
  %op26 = icmp slt i32 %op24, %op25
  %op27 = zext i1 %op26 to i32
  %op28 = icmp ne i32 %op27, 0
  br i1 %op28, label %label39, label %label53
label29:                                                ; preds = %label15
  %op30 = load i32, i32* %op9
  %op31 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op30
  %op32 = load i32, i32* %op31
  %op33 = load i32, i32* %op4
  %op34 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op33
  store i32 %op32, i32* %op34
  %op35 = load i32, i32* %op5
  %op36 = load i32, i32* %op9
  %op37 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op36
  store i32 %op35, i32* %op37
  %op38 = load i32, i32* %op9
  store i32 %op38, i32* %op2
  br label %label_ret
label39:                                                ; preds = %label21
  %op40 = load i32, i32* %op11
  %op41 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op40
  %op42 = load i32, i32* %op41
  store i32 %op42, i32* %op13
  %op43 = load i32, i32* %op9
  %op44 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op43
  %op45 = load i32, i32* %op44
  %op46 = load i32, i32* %op11
  %op47 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op46
  store i32 %op45, i32* %op47
  %op48 = load i32, i32* %op13
  %op49 = load i32, i32* %op9
  %op50 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op49
  store i32 %op48, i32* %op50
  %op51 = load i32, i32* %op9
  %op52 = add i32 %op51, 1
  store i32 %op52, i32* %op9
  br label %label53
label53:                                                ; preds = %label21, %label39
  %op54 = load i32, i32* %op11
  %op55 = add i32 %op54, 1
  store i32 %op55, i32* %op11
  br label %label15
}
define i32 @QuickSort(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = alloca i32
  %op3 = alloca i32
  store i32 %arg0, i32* %op3
  %op4 = alloca i32
  store i32 %arg1, i32* %op4
  %op5 = alloca i32
  %op6 = load i32, i32* %op3
  %op7 = load i32, i32* %op4
  %op8 = call i32 @Partition(i32 %op6, i32 %op7)
  store i32 %op8, i32* %op5
  %op9 = load i32, i32* %op3
  %op10 = load i32, i32* %op5
  %op11 = sub i32 %op10, 1
  %op12 = icmp slt i32 %op9, %op11
  %op13 = zext i1 %op12 to i32
  %op14 = icmp ne i32 %op13, 0
  br i1 %op14, label %label16, label %label21
label_ret:                                                ; preds = %label33
  %op15 = load i32, i32* %op2
  ret i32 %op15
label16:                                                ; preds = %label_entry
  %op17 = load i32, i32* %op3
  %op18 = load i32, i32* %op5
  %op19 = sub i32 %op18, 1
  %op20 = call i32 @QuickSort(i32 %op17, i32 %op19)
  br label %label21
label21:                                                ; preds = %label_entry, %label16
  %op22 = load i32, i32* %op4
  %op23 = load i32, i32* %op5
  %op24 = add i32 %op23, 1
  %op25 = icmp sgt i32 %op22, %op24
  %op26 = zext i1 %op25 to i32
  %op27 = icmp ne i32 %op26, 0
  br i1 %op27, label %label28, label %label33
label28:                                                ; preds = %label21
  %op29 = load i32, i32* %op5
  %op30 = add i32 %op29, 1
  %op31 = load i32, i32* %op4
  %op32 = call i32 @QuickSort(i32 %op30, i32 %op31)
  br label %label33
label33:                                                ; preds = %label21, %label28
  store i32 0, i32* %op2
  br label %label_ret
}
define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  %op2 = call i32 @get_int()
  store i32 %op2, i32* %op1
  %op3 = load i32, i32* %op1
  %op4 = call i32 @ReadArray(i32 %op3)
  %op5 = load i32, i32* %op1
  %op6 = sub i32 %op5, 1
  %op7 = call i32 @QuickSort(i32 0, i32 %op6)
  %op8 = load i32, i32* %op1
  %op9 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 0
  call void @put_int_array(i32 %op8, i32* %op9)
  store i32 0, i32* %op0
  br label %label_ret
label_ret:                                                ; preds = %label_entry
  %op10 = load i32, i32* %op0
  ret i32 %op10
}
```

优化后的 ll：

```
define i32 @ReadArray(i32 %arg0) {
label_entry:
  br label %label5
label_ret:                                                ; preds = %label17
  ret i32 0
label5:                                                ; preds = %label_entry, %label11
  %op18 = phi i32 [ 0, %label_entry ], [ %op16, %label11 ]
  %op8 = icmp slt i32 %op18, %arg0
  %op9 = zext i1 %op8 to i32
  %op10 = icmp ne i32 %op9, 0
  br i1 %op10, label %label11, label %label17
label11:                                                ; preds = %label5
  %op12 = call i32 @get_int()
  %op14 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op18
  store i32 %op12, i32* %op14
  %op16 = add i32 %op18, 1
  br label %label5
label17:                                                ; preds = %label5
  br label %label_ret
}
define i32 @Partition(i32 %arg0, i32 %arg1) {
label_entry:
  %op7 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %arg1
  %op8 = load i32, i32* %op7
  br label %label15
label_ret:                                                ; preds = %label29
  ret i32 %op57
label15:                                                ; preds = %label_entry, %label53
  %op56 = phi i32 [ %arg0, %label_entry ], [ %op55, %label53 ]
  %op57 = phi i32 [ %arg0, %label_entry ], [ %op58, %label53 ]
  %op18 = icmp slt i32 %op56, %arg1
  %op19 = zext i1 %op18 to i32
  %op20 = icmp ne i32 %op19, 0
  br i1 %op20, label %label21, label %label29
label21:                                                ; preds = %label15
  %op23 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op56
  %op24 = load i32, i32* %op23
  %op26 = icmp slt i32 %op24, %op8
  %op27 = zext i1 %op26 to i32
  %op28 = icmp ne i32 %op27, 0
  br i1 %op28, label %label39, label %label53
label29:                                                ; preds = %label15
  %op31 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op57
  %op32 = load i32, i32* %op31
  %op34 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %arg1
  store i32 %op32, i32* %op34
  %op37 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op57
  store i32 %op8, i32* %op37
  br label %label_ret
label39:                                                ; preds = %label21
  %op41 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op56
  %op42 = load i32, i32* %op41
  %op44 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op57
  %op45 = load i32, i32* %op44
  %op47 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op56
  store i32 %op45, i32* %op47
  %op50 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 %op57
  store i32 %op42, i32* %op50
  %op52 = add i32 %op57, 1
  br label %label53
label53:                                                ; preds = %label21, %label39
  %op58 = phi i32 [ %op57, %label21 ], [ %op52, %label39 ]
  %op55 = add i32 %op56, 1
  br label %label15
}
define i32 @QuickSort(i32 %arg0, i32 %arg1) {
label_entry:
  %op8 = call i32 @Partition(i32 %arg0, i32 %arg1)
  %op11 = sub i32 %op8, 1
  %op12 = icmp slt i32 %arg0, %op11
  %op13 = zext i1 %op12 to i32
  %op14 = icmp ne i32 %op13, 0
  br i1 %op14, label %label16, label %label21
label_ret:                                                ; preds = %label33
  ret i32 0
label16:                                                ; preds = %label_entry
  %op19 = sub i32 %op8, 1
  %op20 = call i32 @QuickSort(i32 %arg0, i32 %op19)
  br label %label21
label21:                                                ; preds = %label_entry, %label16
  %op24 = add i32 %op8, 1
  %op25 = icmp sgt i32 %arg1, %op24
  %op26 = zext i1 %op25 to i32
  %op27 = icmp ne i32 %op26, 0
  br i1 %op27, label %label28, label %label33
label28:                                                ; preds = %label21
  %op30 = add i32 %op8, 1
  %op32 = call i32 @QuickSort(i32 %op30, i32 %arg1)
  br label %label33
label33:                                                ; preds = %label21, %label28
  br label %label_ret
}
define i32 @main() {
label_entry:
  %op2 = call i32 @get_int()
  %op4 = call i32 @ReadArray(i32 %op2)
  %op6 = sub i32 %op2, 1
  %op7 = call i32 @QuickSort(i32 0, i32 %op6)
  %op9 = getelementptr [20 x i32], [20 x i32]* @a, i32 0, i32 0
  call void @put_int_array(i32 %op2, i32* %op9)
  br label %label_ret
label_ret:                                                ; preds = %label_entry
  ret i32 0
}
```

- `Mem2Reg`可能会删除的指令类型是哪些？对哪些分配(alloca)指令会有影响？

store 指令，load 指令，还有 alloca 指令。

会对 INT 类型，FLOAT 类型以及 POINTER 类型的 alloca 分配指令有影响。

- 在基本块内前进`insideBlockForwarding`时，对`store`指令处理时为什么`rvalue`在`forward_list`中存在时，就需要将`rvalue`替换成`forward_list`映射中的`->second`值？

`forward_list`中存储着冗余 load 指令和其对应的变量值，这个时候如果 store 指令检测出来其中的`rvalue`是被`forward_list`里面的 load 语句赋值的话，那么就可将其直接替换成冗余 load 指令对应的真实变量值。

如例子中 Partition 函数中的 op12 被优化成了 arg0，因为：

```
  store i32 %arg0, i32* %op3
  ...
  %op12 = load i32, i32* %op3
  store i32 %op12, i32* %op11
```

其中可以看到 op3 中存储的值就是 arg0，所以可以直接将 op12 优化成 arg0，将新的对应值加入 new_value。

- 在基本块内前进时，`defined_list`代表什么含义？

里面存储着对于不同指针变量所进行过的最新 store 指令。

- 生成 phi 指令`genPhi`的第一步两层 for 循环在收集什么信息，这些信息在后面的循环中如何被利用生成 Phi 指令？

在收集 store 和 load 指令中的`lvalue`与 block 的对应信息。其实也就是指针变量和 block 的对应信息。

查看 block 之间的交汇，然后生成 Phi 指令。对于多个基本块中定义的每个值，都会生成一个 Phi 指令并将其插入到基本块的开头。 该 Phi 指令通过根据程序的控制流选择正确的定义来合并值的不同定义。

- `valueDefineCounting`为`defined_var`记录了什么信息？

记录了每个块对应的 store 和 load 指令中的`lvalue`变量信息，也就是单个 block 中的指针变量信息。

- `valueForwarding`在遍历基本块时采用的什么方式？

递归的方式。

- `valueForwarding`中为什么`value_status`需要对 phi 指令做信息收集？

因为 phi 指令也是涉及到值传递的过程的。

- `valueForwarding`中第二个循环对`load`指令的替换是什么含义？

相当于是原来的一条指令使用现在可以由一个 new_value 来做替换，就如同例子中：

指令`%op7 = load i32, i32* %op2`就被 arg0 给替换了。因为 arg0 已然拥有 op7 的功能，所以接下来就是 arg0 代替 op7 执行它在 load 指令还活跃的时候执行的一切操作。

- `valueForwarding`中出现的`defined_var`和`value_status`插入条目之间有什么联系？

defined_var 用来判断一个值的 Status，而一个值的 Status 用来判断是用正在加载的值替换一条加载指令，还是创建一条新的 phi 指令来合并从不同基本块加载的值。

#### B2-3

例子中有多层的循环以及分支语句嵌套，使得 Phi 指令的生成要素比较丰富，而且给冗余优化带来了一定的空间，并且其中对于数组的运用还有各种下标的计算给 store 和 load 语句的优化也带来了空间。

### 活跃变量分析

#### B3-1

- 首先我先找到了 exit_block

- 首先我先做了一遍逆向的拓扑排序，这样以后得到新的 block 序列
- 这之后，我对 use 分了类，主要是分成：

```
std::map<BasicBlock *, std::set<Value *>> normal_use_set;
std::map<BasicBlock *, std::map<BasicBlock *, std::set<Value *>>> phi_use_set;
```

就一个是正常的 use，一个是 phi 中的 use。（这里的 phi 指令是并行逻辑的，所以做 use 的时候会特殊一点）

- 这样话在计算 IN 的时候就变成了：

$$
normaluse[B] + \sum{phiuse[B][I]} + (OUT[B] - def[B])
$$

- 计算 OUT 的时候就是：

$$
\sum{normaluse[S] + phiuse[S][B] + (OUT[B] - def[B])}
$$

- 然后迭代计算 IN 还有 OUT 就行了。

### 检查器

#### B4-1

检查了以下内容：

1. LLVM IR 中不同基本块之间的前驱后继关系：`void CheckPreAndSuc()`

   如果一个块以`br`指令结束，那么它的后继是跳转的那个块。原本基本块前驱后继关系用`get_succ_basic_blocks()`指示，所以对每个块，都检查`br`指令的目的块，和原先的`get_succ_basic_blocks()`是否相同。

2. 指令的操作数必须有定值才能使用：`CheckOp(func_->get_entry_block(), defined_op)`

   从`entry`块开始，对后面的块递归遍历。对块中的每条指令，将指令中*定义的操作数*加入`defined_list`，对指令中*使用的操作数*，检查是否在`defined_list`中。`defined_list`表示从`entry`块到当前位置的路径上定义的所有操作数，因此通过不同路径到达同一位置的`defined_list`可能不同。

   这里重点需要注意检查`phi`指令的操作数是否定义的过程。比如指令`%op37 = phi i32 [ %op38, %label_30 ], [ %op39, %label26 ]`，需要检查`%op38`是否在从`基本块label_entry->基本块label_30->当前位置`的路径上定义，同理需要检查`%op39`是否在`基本块label_entry->基本块label_26->当前位置`的路径上定义。

   还需要注意函数名和全局变量是全局定义的，指令中使用的函数调用和全局变量也需要检查。

3. 基本块必须以 br、ret 等指令结尾：`CheckLastInst()`

   检查每个基本块的最后一条语句即可。

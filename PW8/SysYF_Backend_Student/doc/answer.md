# 思考题

## 1

ARM对于超范围的立即数，往往会采取循环右移的手段。这种立即数一般被称为`modified immediate constants`，下面举个简单的例子：

比如 `0x23000000`占32bits，远远超过了12 bits，如果直接硬解码是表示不了的，但是其可以由 `0x23 ROR 8`得到，ROR是以32bits的位宽为基础，向右循环位移，从右边旋转出来的比特位被插入到左边空出的位中。

## 2

这是不可以的，虽说可以将部分phi指令生成时涉及的虚拟寄存器映射到固定的寄存器里面，但是当多重分支出现的时候会有寄存器混淆的情况发生。

如果正在使用的物理寄存器已经被另一个值使用，则寄存器分配器可能需要将该值溢出到内存中以使该寄存器可用于 phi 指令。

## 3

就是可以将ABI规定中使用的部分临时寄存器以及phi语句生成中需要使用的一部分虚拟寄存器直接映射到物理寄存器上，当映射合适数量的时候，就可以实现代码效率的提高。

仔细选择用于函数参数和返回值的寄存器，并避免对两个 Phi 输入使用相同的寄存器 和函数参数或返回值。 这可以减少所需的寄存器到内存或内存到寄存器传输的次数。

# 实现思路

## Register Allocation

### 基础部分

我们的基础部分是由线性扫描寄存器分配算法实现的。其中实验中所提供的数据结构Interval表示一个要分配寄存器的变量的活跃区间（Range）结合。Interval和要分配寄存器的变量是一一对应的。我们要实现的就是使得每两个有重叠的Interval没有使用重叠的寄存器即可，而线性扫描分配算法则是根据Interval的启示位置进行排序，然后进行寄存器分配。

### 优化部分

原有的线性扫描算法在寄存器的分配方面是不紧凑的，也就是说，并不能充分地分配寄存器。最充分利用寄存器的方法自然是图染色法。但是考虑到这是一个NP Complete问题。这里采取了贪心的图染色法来优化寄存器分配。

* 首先现根据Interval的交叠情况建图，以Interval为节点，一但两个Interval有交叠，那么两个节点之间就会有一条边。
* 建完图之后建起一个基于Interval的栈，其中的Interval根据残留图（残留图指的是拿去放入栈中的节点后留下的图）中该节点的度数大小从大到小依次压入栈中。
* 将根据栈顶到栈底的次序依次给节点染色。

## Data Moving

## 基础部分

* single_data_move：将数据从源移动到目的位置，代码由助教提供。
* data_move：
  * 将源中所有数据同时移动到目的对应位置，方法是将源中所有数据入栈，再按顺序出栈并移动到目的位置。
  * 入栈和出栈过程都需要临时寄存器，使用lr，所以在此之前还需要保存lr的值在栈中，移动数据结束后再恢复。
  * 调用single_data_move实现单个数据的移动。

## 优化部分

每次移动都要先入栈再出栈，如果移动数据顺序和结果无关，则可以直接移动；如果有关，也可以通过调整移动顺序使得结果尽可能无关，则不需要多余的入栈和出栈操作。

* 如果源地址和目的地址互不相同，则可以直接移动。
* 如果存在r1->r2,r2->r3的情况，则顺序执行和同时执行的结果会不同。此时可以通过调整两个语句的顺序为r2->r3,r1->r2来避免。
* 如果存在r1->r2,r2->r1这种情况，那么沿用之前入栈再出栈的做法。

# 评测部分

## Special Reg Samples

我们引入了Special_Reg_Samples这一组样例（一共五个），这组样例的特点是会有大量的Interval产生重叠，从而可以充分体现我们对于寄存器分配的算法优化效果。其中我们用来衡量优化效果的维度有两个，一个是未分配寄存器的变量数，还有一个就是时间（以秒为单位）。

## Data Moving Samples

我们对于DataMoving的优化提供了-m的选项，在汇编代码行数以及运行时间上都实现了良好的优化。选取的Samples涉及数据移动但目的地址与源地址无关的情况比较多，所以减少了较多的push和pop操作。




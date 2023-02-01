# 问题1-1
区别在于展开宏引用时，第14行原先对宏M的引用展开成-4.而之前的sample.i中展开为4.
这是因为gcc的参数-DNEG 以字符串“1”定义 NEG 宏 ，根据NEG有定义选择了M定义为-4.

# 问题1-2
- popl 与 popq 
  l 表示long，32位
  q 表示四个字节，64位

- pushl 与 pushq
  在于l与q的区别，同上

- esp 与 rsp
  sp(Stack Pointer)表示栈顶指针，在32位汇编中，esp(Extended SP)寄存器存放的就是栈指针。在64位中，表现为rsp寄存器。

- ebp 与 rbp
  bp(Base Pointer)表示栈底指针，在32位汇编中，ebp(Extended SP)寄存器存放的就是基指指针。在64位中，表现为rbp寄存器。

-             .cfi_def_cfa_offset 16
            .cfi_offset 6, -16

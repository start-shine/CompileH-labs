@a = dso_local global i32 0, align 4
define dso_local i32 @main() #0 {
    store i32 10, i32* @a, align 4
    %1 = load i32, i32* @a, align 4 ; %1 存 a的值 
    %2 = icmp sgt i32 %1, 0
    br i1 %2, label %6, label %3    ; a > 0 ?
3:
    %4 = load i32, i32* @a, align 4
    %5 = icmp slt i32 %1, 6         ; a < 6 ?
    br i1 %5, label %6, label %8
6:                                  ; return a 
    %7 = load i32, i32* @a, align 4
    ret i32 %7
8:                                  ; return -a 
    %9 = load i32, i32* @a, align 4
    %10 = sub nsw i32 0, %9
    ret i32 %10
}
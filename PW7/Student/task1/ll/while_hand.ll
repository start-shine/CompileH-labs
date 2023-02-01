
@a = dso_local global i32 0, align 4
define dso_local i32 @main() #0 {
    %1 = alloca i32, align 4        ; b
    store i32 0, i32* %1, align 4
    store i32 3, i32* @a, align 4
    %2 = load i32, i32* @a, align 4
    %3 = icmp sgt i32 %2, 0         ; a > 0 ?
    br i1 %3, label %4, label %15
4:
    %5 = load i32, i32* %1, align 4
    %6 = icmp slt i32 %5, 10        ; b < 10 ?
    br i1 %3, label %7, label %15
7:
    %8 = load i32, i32* %1, align 4
    %9 = load i32, i32* @a, align 4
    %10 = add nsw i32 %8, %9
    store i32 %10, i32* %1, align 4 ; b = b + a
    %11 = load i32, i32* @a, align 4
    %12 = sub nsw i32 %11, 1
    store i32 %12, i32* @a, align 4 ; a = a - 1;
    %13 = load i32, i32* @a, align 4
    %14 = icmp sgt i32 %13, 0         ; a > 0 ?
    br i1 %14, label %4, label %15
15:
    ret i32 0
}
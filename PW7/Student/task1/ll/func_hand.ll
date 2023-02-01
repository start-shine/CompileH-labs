define dso_local i32 @add(i32 %0, i32 %1) #0{
    ;/* %2 = alloca i32, align 4
    ;%3 = alloca i32, align 4 */
    %3 = add nsw i32 %0, %1
    ret i32 %3
}

define dso_local i32 @main() #0 {
    %1 = alloca i32, align 4
    %2 = alloca i32, align 4
    store i32 3, i32* %1, align 4
    store i32 5, i32* %2, align 4
    %3 = load i32, i32* %1, align 4
    %4 = add nsw i32 %3, %3
    %5 = call i32 @add(i32 %3, i32 %4)
    %6 = load i32, i32* %2, align 4
    %7 = add nsw i32 %6, %5
    ret i32 %7
}
source_filename = "go_upstairs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@num = dso_local global [2 x i32] [i32 4, i32 8], align 4
@tmp = dso_local global i32 1, align 4
@n = common dso_local global i32 0, align 4
@x = common dso_local global [1 x i32] zeroinitializer, align 4

define dso_local i32 @main() #0 {
    %1 = alloca float, align 4
    store float 0x3FFCCCCCC0000000, float* %1, align 4
    %2 = alloca [2 x i32], align 4
    %3 = getelementptr inbounds [2 x i32], [2 x i32]* %2, i64 0, i64 0
    store i32 2, i32* %3, align 4
    %4 = getelementptr inbounds [2 x i32], [2 x i32]* %2, i64 0, i64 1
    store i32 2, i32* %4, align 4
    %5 = getelementptr inbounds [2 x i32], [2 x i32]* %2, i64 0, i64 0
    %6 = load i32, i32* %5, align 4
    %7 = sitofp i32 %6 to float
    %8 = load float, float* %1, align 4
    %9 = fmul float %7, %8
    %10 = fptosi float %9 to i32
    %11 = getelementptr inbounds [2 x i32], [2 x i32]* %2, i64 0, i64 1
    store i32 %10, i32* %11, align 4
    %12 = fptosi float %8 to i32
    ret i32 %12

}
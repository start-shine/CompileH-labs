define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4        ; a 
  %2 = call i32 @getint()
  store i32 %2, i32* %1, align 4
  %3 = alloca [10000 x float], align 16
  %4 = load i32, i32* %1, align 4
  call void @putint(i32 %4)
  %5 = bitcast [10000 x float]* %3 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %5, i8 0, i64 40000, i1 false)
  %6 = bitcast i8* %5 to <{ float, float, [9998 x float] }>*
  %7 = getelementptr inbounds <{ float, float, [9998 x float] }>, <{ float, float, [9998 x float] }>* %6, i32 0, i32 0
  store float 1.000000e+00, float* %7, align 16
  %8 = getelementptr inbounds <{ float, float, [9998 x float] }>, <{ float, float, [9998 x float] }>* %6, i32 0, i32 1
  store float 2.000000e+00, float* %8, align 4
  %9 = getelementptr inbounds [10000 x float], [10000 x float]* %3, i32 0, i32 0
  %10 = call i32 @getfarray(float* %9)
  %11 = alloca i32, align 4
  store i32 %10, i32* %11, align 4
  %12 = load i32, i32* %11, align 4
  %13 = add nsw i32 %12, 1
  %14 = getelementptr inbounds [10000 x float], [10000 x float]* %3, i64 0, i64 0
  call void @putfarray(i32 %13, float* %14)
  %15 = getelementptr inbounds [10000 x float], [10000 x float]* %3, i64 0, i64 0
  %16 = load float, float* %15, align 16
  %17 = fptosi float %16 to i32
  ret i32 %17
}
declare dso_local i32 @getint() #1

declare dso_local void @putint(i32) #1

declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

declare dso_local i32 @getfarray(float*) #1

declare dso_local void @putfarray(i32, float*) #1

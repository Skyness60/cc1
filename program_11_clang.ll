; ModuleID = 'tests/samples/program_11.c'
source_filename = "tests/samples/program_11.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-i128:128-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

%struct.anon = type { float, [2 x i32], %union.anon }
%union.anon = type { i32 }
%struct.T = type { %struct.anon, i8, [2 x i32] }

@array1 = dso_local global [4 x { %struct.anon, i8, [3 x i8], [2 x i32] }] [{ %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+00, [2 x i32] [i32 2, i32 0], %union.anon { i32 3 } }, i8 84, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon zeroinitializer, i8 118, [3 x i8] zeroinitializer, [2 x i32] [i32 8, i32 9] }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+01, [2 x i32] [i32 11, i32 12], %union.anon { i32 13 } }, i8 -2, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.600000e+01, [2 x i32] zeroinitializer, %union.anon zeroinitializer }, i8 0, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }], align 4
@array2 = dso_local global [4 x { %struct.anon, i8, [3 x i8], [2 x i32] }] [{ %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+00, [2 x i32] [i32 2, i32 0], %union.anon { i32 3 } }, i8 84, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon zeroinitializer, i8 118, [3 x i8] zeroinitializer, [2 x i32] [i32 8, i32 9] }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+01, [2 x i32] [i32 11, i32 12], %union.anon { i32 13 } }, i8 -2, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.600000e+01, [2 x i32] zeroinitializer, %union.anon zeroinitializer }, i8 0, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }], align 4
@.str = private unnamed_addr constant [3 x i8] c"{\0A\00", align 1
@.str.1 = private unnamed_addr constant [15 x i8] c"    .[%d] = {\0A\00", align 1
@.str.2 = private unnamed_addr constant [16 x i8] c"        .a = {\0A\00", align 1
@.str.3 = private unnamed_addr constant [22 x i8] c"            .a = %f,\0A\00", align 1
@.str.4 = private unnamed_addr constant [20 x i8] c"            .b = {\0A\00", align 1
@.str.5 = private unnamed_addr constant [28 x i8] c"                .[0] = %d,\0A\00", align 1
@.str.6 = private unnamed_addr constant [28 x i8] c"                .[1] = %d,\0A\00", align 1
@.str.7 = private unnamed_addr constant [16 x i8] c"            },\0A\00", align 1
@.str.8 = private unnamed_addr constant [29 x i8] c"            .c = {.a = %d},\0A\00", align 1
@.str.9 = private unnamed_addr constant [12 x i8] c"        },\0A\00", align 1
@.str.10 = private unnamed_addr constant [19 x i8] c"        .b1 = %d,\0A\00", align 1
@.str.11 = private unnamed_addr constant [19 x i8] c"        .b2 = %d,\0A\00", align 1
@.str.12 = private unnamed_addr constant [16 x i8] c"        .c = {\0A\00", align 1
@.str.13 = private unnamed_addr constant [24 x i8] c"            .[0] = %d,\0A\00", align 1
@.str.14 = private unnamed_addr constant [24 x i8] c"            .[1] = %d,\0A\00", align 1
@.str.15 = private unnamed_addr constant [8 x i8] c"    },\0A\00", align 1
@.str.16 = private unnamed_addr constant [3 x i8] c"}\0A\00", align 1
@.str.17 = private unnamed_addr constant [9 x i8] c"array1:\0A\00", align 1
@.str.18 = private unnamed_addr constant [9 x i8] c"array2:\0A\00", align 1
@.str.19 = private unnamed_addr constant [43 x i8] c"error: struct isn't correctly initialized\0A\00", align 1
@.str.20 = private unnamed_addr constant [4 x i8] c"OK\0A\00", align 1

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local void @print_array(ptr noundef %0) #0 {
  %2 = alloca ptr, align 4
  %3 = alloca i32, align 4
  store ptr %0, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  store i32 0, ptr %3, align 4
  br label %5

5:                                                ; preds = %79, %1
  %6 = load i32, ptr %3, align 4
  %7 = icmp slt i32 %6, 4
  br i1 %7, label %8, label %82

8:                                                ; preds = %5
  %9 = load i32, ptr %3, align 4
  %10 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %9)
  %11 = call i32 (ptr, ...) @printf(ptr noundef @.str.2)
  %12 = load ptr, ptr %2, align 4
  %13 = load i32, ptr %3, align 4
  %14 = getelementptr inbounds %struct.T, ptr %12, i32 %13
  %15 = getelementptr inbounds nuw %struct.T, ptr %14, i32 0, i32 0
  %16 = getelementptr inbounds nuw %struct.anon, ptr %15, i32 0, i32 0
  %17 = load float, ptr %16, align 4
  %18 = fpext float %17 to double
  %19 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, double noundef %18)
  %20 = call i32 (ptr, ...) @printf(ptr noundef @.str.4)
  %21 = load ptr, ptr %2, align 4
  %22 = load i32, ptr %3, align 4
  %23 = getelementptr inbounds %struct.T, ptr %21, i32 %22
  %24 = getelementptr inbounds nuw %struct.T, ptr %23, i32 0, i32 0
  %25 = getelementptr inbounds nuw %struct.anon, ptr %24, i32 0, i32 1
  %26 = getelementptr inbounds [2 x i32], ptr %25, i32 0, i32 0
  %27 = load i32, ptr %26, align 4
  %28 = call i32 (ptr, ...) @printf(ptr noundef @.str.5, i32 noundef %27)
  %29 = load ptr, ptr %2, align 4
  %30 = load i32, ptr %3, align 4
  %31 = getelementptr inbounds %struct.T, ptr %29, i32 %30
  %32 = getelementptr inbounds nuw %struct.T, ptr %31, i32 0, i32 0
  %33 = getelementptr inbounds nuw %struct.anon, ptr %32, i32 0, i32 1
  %34 = getelementptr inbounds [2 x i32], ptr %33, i32 0, i32 1
  %35 = load i32, ptr %34, align 4
  %36 = call i32 (ptr, ...) @printf(ptr noundef @.str.6, i32 noundef %35)
  %37 = call i32 (ptr, ...) @printf(ptr noundef @.str.7)
  %38 = load ptr, ptr %2, align 4
  %39 = load i32, ptr %3, align 4
  %40 = getelementptr inbounds %struct.T, ptr %38, i32 %39
  %41 = getelementptr inbounds nuw %struct.T, ptr %40, i32 0, i32 0
  %42 = getelementptr inbounds nuw %struct.anon, ptr %41, i32 0, i32 2
  %43 = load i32, ptr %42, align 4
  %44 = call i32 (ptr, ...) @printf(ptr noundef @.str.8, i32 noundef %43)
  %45 = call i32 (ptr, ...) @printf(ptr noundef @.str.9)
  %46 = load ptr, ptr %2, align 4
  %47 = load i32, ptr %3, align 4
  %48 = getelementptr inbounds %struct.T, ptr %46, i32 %47
  %49 = getelementptr inbounds nuw %struct.T, ptr %48, i32 0, i32 1
  %50 = load i8, ptr %49, align 4
  %51 = and i8 %50, 15
  %52 = zext i8 %51 to i32
  %53 = call i32 (ptr, ...) @printf(ptr noundef @.str.10, i32 noundef %52)
  %54 = load ptr, ptr %2, align 4
  %55 = load i32, ptr %3, align 4
  %56 = getelementptr inbounds %struct.T, ptr %54, i32 %55
  %57 = getelementptr inbounds nuw %struct.T, ptr %56, i32 0, i32 1
  %58 = load i8, ptr %57, align 4
  %59 = lshr i8 %58, 4
  %60 = zext i8 %59 to i32
  %61 = call i32 (ptr, ...) @printf(ptr noundef @.str.11, i32 noundef %60)
  %62 = call i32 (ptr, ...) @printf(ptr noundef @.str.12)
  %63 = load ptr, ptr %2, align 4
  %64 = load i32, ptr %3, align 4
  %65 = getelementptr inbounds %struct.T, ptr %63, i32 %64
  %66 = getelementptr inbounds nuw %struct.T, ptr %65, i32 0, i32 2
  %67 = getelementptr inbounds [2 x i32], ptr %66, i32 0, i32 0
  %68 = load i32, ptr %67, align 4
  %69 = call i32 (ptr, ...) @printf(ptr noundef @.str.13, i32 noundef %68)
  %70 = load ptr, ptr %2, align 4
  %71 = load i32, ptr %3, align 4
  %72 = getelementptr inbounds %struct.T, ptr %70, i32 %71
  %73 = getelementptr inbounds nuw %struct.T, ptr %72, i32 0, i32 2
  %74 = getelementptr inbounds [2 x i32], ptr %73, i32 0, i32 1
  %75 = load i32, ptr %74, align 4
  %76 = call i32 (ptr, ...) @printf(ptr noundef @.str.14, i32 noundef %75)
  %77 = call i32 (ptr, ...) @printf(ptr noundef @.str.9)
  %78 = call i32 (ptr, ...) @printf(ptr noundef @.str.15)
  br label %79

79:                                               ; preds = %8
  %80 = load i32, ptr %3, align 4
  %81 = add nsw i32 %80, 1
  store i32 %81, ptr %3, align 4
  br label %5

82:                                               ; preds = %5
  %83 = call i32 (ptr, ...) @printf(ptr noundef @.str.16)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = call i32 (ptr, ...) @printf(ptr noundef @.str.17)
  call void @print_array(ptr noundef @array1)
  %3 = call i32 (ptr, ...) @printf(ptr noundef @.str.18)
  call void @print_array(ptr noundef @array2)
  %4 = call i32 @memcmp(ptr noundef @array1, ptr noundef @array2, i32 noundef 112)
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %8

6:                                                ; preds = %0
  %7 = call i32 (ptr, ...) @printf(ptr noundef @.str.19)
  br label %10

8:                                                ; preds = %0
  %9 = call i32 (ptr, ...) @printf(ptr noundef @.str.20)
  br label %10

10:                                               ; preds = %8, %6
  %11 = load i32, ptr %1, align 4
  ret i32 %11
}

declare i32 @memcmp(ptr noundef, ptr noundef, i32 noundef) #1

attributes #0 = { noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4, !5}
!llvm.ident = !{!6}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"PIE Level", i32 2}
!4 = !{i32 7, !"uwtable", i32 2}
!5 = !{i32 7, !"frame-pointer", i32 2}
!6 = !{!"clang version 21.1.6"}

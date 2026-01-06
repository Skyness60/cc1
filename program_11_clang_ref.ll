; ModuleID = 'tests/samples/program_11.c'
source_filename = "tests/samples/program_11.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.anon = type { float, [2 x i32], %union.anon }
%union.anon = type { i32 }
%struct.T = type { %struct.anon, i8, [2 x i32] }

@array1 = dso_local global [4 x { %struct.anon, i8, [3 x i8], [2 x i32] }] [{ %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+00, [2 x i32] [i32 2, i32 0], %union.anon { i32 3 } }, i8 84, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon zeroinitializer, i8 118, [3 x i8] zeroinitializer, [2 x i32] [i32 8, i32 9] }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+01, [2 x i32] [i32 11, i32 12], %union.anon { i32 13 } }, i8 -2, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.600000e+01, [2 x i32] zeroinitializer, %union.anon zeroinitializer }, i8 0, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }], align 16
@array2 = dso_local global [4 x { %struct.anon, i8, [3 x i8], [2 x i32] }] [{ %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+00, [2 x i32] [i32 2, i32 0], %union.anon { i32 3 } }, i8 84, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon zeroinitializer, i8 118, [3 x i8] zeroinitializer, [2 x i32] [i32 8, i32 9] }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.000000e+01, [2 x i32] [i32 11, i32 12], %union.anon { i32 13 } }, i8 -2, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, { %struct.anon, i8, [3 x i8], [2 x i32] } { %struct.anon { float 1.600000e+01, [2 x i32] zeroinitializer, %union.anon zeroinitializer }, i8 0, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }], align 16
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
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  store ptr %0, ptr %2, align 8
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  store i32 0, ptr %3, align 4
  br label %5

5:                                                ; preds = %87, %1
  %6 = load i32, ptr %3, align 4
  %7 = icmp slt i32 %6, 4
  br i1 %7, label %8, label %90

8:                                                ; preds = %5
  %9 = load i32, ptr %3, align 4
  %10 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %9)
  %11 = call i32 (ptr, ...) @printf(ptr noundef @.str.2)
  %12 = load ptr, ptr %2, align 8
  %13 = load i32, ptr %3, align 4
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds %struct.T, ptr %12, i64 %14
  %16 = getelementptr inbounds nuw %struct.T, ptr %15, i32 0, i32 0
  %17 = getelementptr inbounds nuw %struct.anon, ptr %16, i32 0, i32 0
  %18 = load float, ptr %17, align 4
  %19 = fpext float %18 to double
  %20 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, double noundef %19)
  %21 = call i32 (ptr, ...) @printf(ptr noundef @.str.4)
  %22 = load ptr, ptr %2, align 8
  %23 = load i32, ptr %3, align 4
  %24 = sext i32 %23 to i64
  %25 = getelementptr inbounds %struct.T, ptr %22, i64 %24
  %26 = getelementptr inbounds nuw %struct.T, ptr %25, i32 0, i32 0
  %27 = getelementptr inbounds nuw %struct.anon, ptr %26, i32 0, i32 1
  %28 = getelementptr inbounds [2 x i32], ptr %27, i64 0, i64 0
  %29 = load i32, ptr %28, align 4
  %30 = call i32 (ptr, ...) @printf(ptr noundef @.str.5, i32 noundef %29)
  %31 = load ptr, ptr %2, align 8
  %32 = load i32, ptr %3, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds %struct.T, ptr %31, i64 %33
  %35 = getelementptr inbounds nuw %struct.T, ptr %34, i32 0, i32 0
  %36 = getelementptr inbounds nuw %struct.anon, ptr %35, i32 0, i32 1
  %37 = getelementptr inbounds [2 x i32], ptr %36, i64 0, i64 1
  %38 = load i32, ptr %37, align 4
  %39 = call i32 (ptr, ...) @printf(ptr noundef @.str.6, i32 noundef %38)
  %40 = call i32 (ptr, ...) @printf(ptr noundef @.str.7)
  %41 = load ptr, ptr %2, align 8
  %42 = load i32, ptr %3, align 4
  %43 = sext i32 %42 to i64
  %44 = getelementptr inbounds %struct.T, ptr %41, i64 %43
  %45 = getelementptr inbounds nuw %struct.T, ptr %44, i32 0, i32 0
  %46 = getelementptr inbounds nuw %struct.anon, ptr %45, i32 0, i32 2
  %47 = load i32, ptr %46, align 4
  %48 = call i32 (ptr, ...) @printf(ptr noundef @.str.8, i32 noundef %47)
  %49 = call i32 (ptr, ...) @printf(ptr noundef @.str.9)
  %50 = load ptr, ptr %2, align 8
  %51 = load i32, ptr %3, align 4
  %52 = sext i32 %51 to i64
  %53 = getelementptr inbounds %struct.T, ptr %50, i64 %52
  %54 = getelementptr inbounds nuw %struct.T, ptr %53, i32 0, i32 1
  %55 = load i8, ptr %54, align 4
  %56 = and i8 %55, 15
  %57 = zext i8 %56 to i32
  %58 = call i32 (ptr, ...) @printf(ptr noundef @.str.10, i32 noundef %57)
  %59 = load ptr, ptr %2, align 8
  %60 = load i32, ptr %3, align 4
  %61 = sext i32 %60 to i64
  %62 = getelementptr inbounds %struct.T, ptr %59, i64 %61
  %63 = getelementptr inbounds nuw %struct.T, ptr %62, i32 0, i32 1
  %64 = load i8, ptr %63, align 4
  %65 = lshr i8 %64, 4
  %66 = zext i8 %65 to i32
  %67 = call i32 (ptr, ...) @printf(ptr noundef @.str.11, i32 noundef %66)
  %68 = call i32 (ptr, ...) @printf(ptr noundef @.str.12)
  %69 = load ptr, ptr %2, align 8
  %70 = load i32, ptr %3, align 4
  %71 = sext i32 %70 to i64
  %72 = getelementptr inbounds %struct.T, ptr %69, i64 %71
  %73 = getelementptr inbounds nuw %struct.T, ptr %72, i32 0, i32 2
  %74 = getelementptr inbounds [2 x i32], ptr %73, i64 0, i64 0
  %75 = load i32, ptr %74, align 4
  %76 = call i32 (ptr, ...) @printf(ptr noundef @.str.13, i32 noundef %75)
  %77 = load ptr, ptr %2, align 8
  %78 = load i32, ptr %3, align 4
  %79 = sext i32 %78 to i64
  %80 = getelementptr inbounds %struct.T, ptr %77, i64 %79
  %81 = getelementptr inbounds nuw %struct.T, ptr %80, i32 0, i32 2
  %82 = getelementptr inbounds [2 x i32], ptr %81, i64 0, i64 1
  %83 = load i32, ptr %82, align 4
  %84 = call i32 (ptr, ...) @printf(ptr noundef @.str.14, i32 noundef %83)
  %85 = call i32 (ptr, ...) @printf(ptr noundef @.str.9)
  %86 = call i32 (ptr, ...) @printf(ptr noundef @.str.15)
  br label %87

87:                                               ; preds = %8
  %88 = load i32, ptr %3, align 4
  %89 = add nsw i32 %88, 1
  store i32 %89, ptr %3, align 4
  br label %5, !llvm.loop !6

90:                                               ; preds = %5
  %91 = call i32 (ptr, ...) @printf(ptr noundef @.str.16)
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
  %4 = call i32 @memcmp(ptr noundef @array1, ptr noundef @array2, i64 noundef 112)
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

declare i32 @memcmp(ptr noundef, ptr noundef, i64 noundef) #1

attributes #0 = { noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 21.1.6"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}

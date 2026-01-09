; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

%struct.anon.0 = type { i32, i8, [3 x i8] }
%struct.anon.1 = type { i32, i8, [3 x i8] }
%struct.s = type { i8, [1 x i8], i16, %struct.anon.0 }
%struct.u = type { %struct.anon.1 }
@s1 = dso_local global [1337 x %struct.s] zeroinitializer
@.str.0 = private unnamed_addr constant [19 x i8] c"size of char: %zu\0A\00"
@.str.1 = private unnamed_addr constant [24 x i8] c"size of short int: %zu\0A\00"
@.str.2 = private unnamed_addr constant [18 x i8] c"size of int: %zu\0A\00"
@.str.3 = private unnamed_addr constant [23 x i8] c"size of long int: %zu\0A\00"
@.str.4 = private unnamed_addr constant [20 x i8] c"size of float: %zu\0A\00"
@.str.5 = private unnamed_addr constant [21 x i8] c"size of double: %zu\0A\00"
@.str.6 = private unnamed_addr constant [26 x i8] c"size of long double: %zu\0A\00"
@.str.7 = private unnamed_addr constant [23 x i8] c"size of struct s: %zu\0A\00"
@.str.8 = private unnamed_addr constant [22 x i8] c"size of union u: %zu\0A\00"
@.str.9 = private unnamed_addr constant [21 x i8] c"enum constant A: %d\0A\00"
@.str.10 = private unnamed_addr constant [21 x i8] c"enum constant B: %d\0A\00"
@.str.11 = private unnamed_addr constant [21 x i8] c"enum constant C: %d\0A\00"
@.str.12 = private unnamed_addr constant [21 x i8] c"enum constant D: %d\0A\00"
@.str.13 = private unnamed_addr constant [21 x i8] c"enum constant E: %d\0A\00"
@.str.14 = private unnamed_addr constant [21 x i8] c"enum constant F: %d\0A\00"
@.str.15 = private unnamed_addr constant [21 x i8] c"enum constant G: %d\0A\00"
declare i32 @printf(i8*, ...)

define dso_local i32 @main() {
entry:
  %retval = alloca i32
  store i32 0, i32* %retval
  %0 = getelementptr inbounds [19 x i8], [19 x i8]* @.str.0, i32 0, i32 0
  %1 = call i32 @printf(i8* %0, i32 1)
  %2 = getelementptr inbounds [24 x i8], [24 x i8]* @.str.1, i32 0, i32 0
  %3 = call i32 @printf(i8* %2, i32 2)
  %4 = getelementptr inbounds [18 x i8], [18 x i8]* @.str.2, i32 0, i32 0
  %5 = call i32 @printf(i8* %4, i32 4)
  %6 = getelementptr inbounds [23 x i8], [23 x i8]* @.str.3, i32 0, i32 0
  %7 = call i32 @printf(i8* %6, i32 4)
  %8 = getelementptr inbounds [20 x i8], [20 x i8]* @.str.4, i32 0, i32 0
  %9 = call i32 @printf(i8* %8, i32 4)
  %10 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.5, i32 0, i32 0
  %11 = call i32 @printf(i8* %10, i32 8)
  %12 = getelementptr inbounds [26 x i8], [26 x i8]* @.str.6, i32 0, i32 0
  %13 = call i32 @printf(i8* %12, i32 12)
  %14 = getelementptr inbounds [23 x i8], [23 x i8]* @.str.7, i32 0, i32 0
  %15 = call i32 @printf(i8* %14, i32 12)
  %16 = getelementptr inbounds [22 x i8], [22 x i8]* @.str.8, i32 0, i32 0
  %17 = call i32 @printf(i8* %16, i32 8)
  %18 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.9, i32 0, i32 0
  %19 = call i32 @printf(i8* %18, i32 1337)
  %20 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.10, i32 0, i32 0
  %21 = call i32 @printf(i8* %20, i32 42)
  %22 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.11, i32 0, i32 0
  %23 = call i32 @printf(i8* %22, i32 43)
  %24 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.12, i32 0, i32 0
  %25 = call i32 @printf(i8* %24, i32 44)
  %26 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.13, i32 0, i32 0
  %27 = call i32 @printf(i8* %26, i32 42)
  %28 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.14, i32 0, i32 0
  %29 = call i32 @printf(i8* %28, i32 43)
  %30 = getelementptr inbounds [21 x i8], [21 x i8]* @.str.15, i32 0, i32 0
  %31 = call i32 @printf(i8* %30, i32 2)
  br label %return0
return0:
  %32 = load i32, i32* %retval
  ret i32 %32
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

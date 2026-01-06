; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@a = dso_local global i32 17
@b = dso_local global i32 0
@f.a.0 = internal global i32 42
@g.a.1 = internal global i32 84
@h.a.2 = internal global i32 168
@.str.0 = private unnamed_addr constant [4 x i8] c"%d \00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%d \00"
@.str.2 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.3 = private unnamed_addr constant [4 x i8] c"%d \00"
@.str.4 = private unnamed_addr constant [4 x i8] c"%d \00"
@.str.5 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.6 = private unnamed_addr constant [4 x i8] c"%d \00"
@.str.7 = private unnamed_addr constant [4 x i8] c"%d \00"
@.str.8 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.9 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00"
declare i32 @printf(i8*, ...)

define dso_local i32 @f() {
entry:
  %retval = alloca i32
  %0 = load i32, i32* @b
  %1 = add i32 %0, 1
  store i32 %1, i32* @b
  %2 = load i32, i32* @f.a.0
  %3 = add i32 %2, 1
  store i32 %3, i32* @f.a.0
  store i32 %2, i32* %retval
  br label %return0
return0:
  %4 = load i32, i32* %retval
  ret i32 %4
}

define dso_local i32 @g() {
entry:
  %retval = alloca i32
  %0 = load i32, i32* @b
  %1 = add i32 %0, 1
  store i32 %1, i32* @b
  %2 = load i32, i32* @g.a.1
  %3 = add i32 %2, 1
  store i32 %3, i32* @g.a.1
  store i32 %2, i32* %retval
  br label %return1
return1:
  %4 = load i32, i32* %retval
  ret i32 %4
}

define dso_local i32 @main() {
entry:
  %retval = alloca i32
  store i32 0, i32* %retval
  %0 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.0, i32 0, i32 0
  %1 = call i32 @f()
  %2 = call i32 @printf(i8* %0, i32 %1)
  %3 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.1, i32 0, i32 0
  %4 = call i32 @f()
  %5 = call i32 @printf(i8* %3, i32 %4)
  %6 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.2, i32 0, i32 0
  %7 = call i32 @f()
  %8 = call i32 @printf(i8* %6, i32 %7)
  %9 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.3, i32 0, i32 0
  %10 = call i32 @g()
  %11 = call i32 @printf(i8* %9, i32 %10)
  %12 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.4, i32 0, i32 0
  %13 = call i32 @g()
  %14 = call i32 @printf(i8* %12, i32 %13)
  %15 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.5, i32 0, i32 0
  %16 = call i32 @g()
  %17 = call i32 @printf(i8* %15, i32 %16)
  %18 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.6, i32 0, i32 0
  %19 = call i32 @h()
  %20 = call i32 @printf(i8* %18, i32 %19)
  %21 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.7, i32 0, i32 0
  %22 = call i32 @h()
  %23 = call i32 @printf(i8* %21, i32 %22)
  %24 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.8, i32 0, i32 0
  %25 = call i32 @h()
  %26 = call i32 @printf(i8* %24, i32 %25)
  %27 = getelementptr inbounds [7 x i8], [7 x i8]* @.str.9, i32 0, i32 0
  %28 = load i32, i32* @a
  %29 = load i32, i32* @b
  %30 = call i32 @printf(i8* %27, i32 %28, i32 %29)
  br label %return2
return2:
  %31 = load i32, i32* %retval
  ret i32 %31
}

define dso_local i32 @h() {
entry:
  %retval = alloca i32
  %0 = load i32, i32* @b
  %1 = add i32 %0, 1
  store i32 %1, i32* @b
  %2 = load i32, i32* @h.a.2
  %3 = add i32 %2, 1
  store i32 %3, i32* @h.a.2
  store i32 %2, i32* %retval
  br label %return3
return3:
  %4 = load i32, i32* %retval
  ret i32 %4
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

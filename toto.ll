; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@size_t = dso_local global i32 0
%struct..anon.0 = type { i32 }
%struct..anon.1 = type { float, [2 x i32], %struct..anon.0 }
%struct..anon.2 = type { %struct..anon.1, i8, i8, [2 x i32] }
@T = dso_local global %struct..anon.2 zeroinitializer
@array1 = dso_local global [4 x %struct..anon.2] zeroinitializer
@array2 = dso_local global [4 x %struct..anon.2] zeroinitializer
@.str.0 = private unnamed_addr constant [3 x i8] c"{\0A\00"
@.str.1 = private unnamed_addr constant [15 x i8] c"    .[%d] = {\0A\00"
@.str.2 = private unnamed_addr constant [16 x i8] c"        .a = {\0A\00"
@.str.3 = private unnamed_addr constant [22 x i8] c"            .a = %f,\0A\00"
@.str.4 = private unnamed_addr constant [20 x i8] c"            .b = {\0A\00"
@.str.5 = private unnamed_addr constant [28 x i8] c"                .[0] = %d,\0A\00"
@.str.6 = private unnamed_addr constant [28 x i8] c"                .[1] = %d,\0A\00"
@.str.7 = private unnamed_addr constant [16 x i8] c"            },\0A\00"
@.str.8 = private unnamed_addr constant [29 x i8] c"            .c = {.a = %d},\0A\00"
@.str.9 = private unnamed_addr constant [12 x i8] c"        },\0A\00"
@.str.10 = private unnamed_addr constant [19 x i8] c"        .b1 = %d,\0A\00"
@.str.11 = private unnamed_addr constant [19 x i8] c"        .b2 = %d,\0A\00"
@.str.12 = private unnamed_addr constant [16 x i8] c"        .c = {\0A\00"
@.str.13 = private unnamed_addr constant [24 x i8] c"            .[0] = %d,\0A\00"
@.str.14 = private unnamed_addr constant [24 x i8] c"            .[1] = %d,\0A\00"
@.str.15 = private unnamed_addr constant [12 x i8] c"        },\0A\00"
@.str.16 = private unnamed_addr constant [8 x i8] c"    },\0A\00"
@.str.17 = private unnamed_addr constant [3 x i8] c"}\0A\00"
@.str.18 = private unnamed_addr constant [9 x i8] c"array1:\0A\00"
@.str.19 = private unnamed_addr constant [9 x i8] c"array2:\0A\00"
@.str.20 = private unnamed_addr constant [43 x i8] c"error: struct isn't correctly initialized\0A\00"
@.str.21 = private unnamed_addr constant [4 x i8] c"OK\0A\00"
declare i32 @memcmp(i8*, i8*, i32)
declare i32 @printf(i8*, ...)

define dso_local void @print_array(%struct..anon.2* %0) {
entry:
  %array.addr = alloca %struct..anon.2*
  store %struct..anon.2* %0, %struct..anon.2** %array.addr
  %i.addr1 = alloca i32
  %2 = getelementptr inbounds [3 x i8], [3 x i8]* @.str.0, i32 0, i32 0
  %3 = call i32 @printf(i8* %2)
  store i32 0, i32* %i.addr1
  br label %for.cond1
for.cond1:
  %4 = load i32, i32* %i.addr1
  %5 = icmp slt i32 %4, 4
  %6 = zext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %for.body2, label %for.end4
for.body2:
  %8 = getelementptr inbounds [15 x i8], [15 x i8]* @.str.1, i32 0, i32 0
  %9 = load i32, i32* %i.addr1
  %10 = call i32 @printf(i8* %8, i32 %9)
  %11 = getelementptr inbounds [16 x i8], [16 x i8]* @.str.2, i32 0, i32 0
  %12 = call i32 @printf(i8* %11)
  %13 = getelementptr inbounds [22 x i8], [22 x i8]* @.str.3, i32 0, i32 0
  %14 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %15 = load i32, i32* %i.addr1
  %16 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %14, i32 %15
  %17 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %16, i32 0, i32 0
  %18 = getelementptr inbounds %struct..anon.1, %struct..anon.1* %17, i32 0, i32 0
  %19 = load float, float* %18
  %20 = call i32 @printf(i8* %13, float %19)
  %21 = getelementptr inbounds [20 x i8], [20 x i8]* @.str.4, i32 0, i32 0
  %22 = call i32 @printf(i8* %21)
  %23 = getelementptr inbounds [28 x i8], [28 x i8]* @.str.5, i32 0, i32 0
  %24 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %25 = load i32, i32* %i.addr1
  %26 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %24, i32 %25
  %27 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %26, i32 0, i32 0
  %28 = getelementptr inbounds %struct..anon.1, %struct..anon.1* %27, i32 0, i32 1
  %29 = getelementptr inbounds [2 x i32], [2 x i32]* %28, i32 0, i32 0
  %30 = load i32, i32* %29
  %31 = call i32 @printf(i8* %23, i32 %30)
  %32 = getelementptr inbounds [28 x i8], [28 x i8]* @.str.6, i32 0, i32 0
  %33 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %34 = load i32, i32* %i.addr1
  %35 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %33, i32 %34
  %36 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %35, i32 0, i32 0
  %37 = getelementptr inbounds %struct..anon.1, %struct..anon.1* %36, i32 0, i32 1
  %38 = getelementptr inbounds [2 x i32], [2 x i32]* %37, i32 0, i32 1
  %39 = load i32, i32* %38
  %40 = call i32 @printf(i8* %32, i32 %39)
  %41 = getelementptr inbounds [16 x i8], [16 x i8]* @.str.7, i32 0, i32 0
  %42 = call i32 @printf(i8* %41)
  %43 = getelementptr inbounds [29 x i8], [29 x i8]* @.str.8, i32 0, i32 0
  %44 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %45 = load i32, i32* %i.addr1
  %46 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %44, i32 %45
  %47 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %46, i32 0, i32 0
  %48 = getelementptr inbounds %struct..anon.1, %struct..anon.1* %47, i32 0, i32 2
  %49 = getelementptr inbounds %struct..anon.0, %struct..anon.0* %48, i32 0, i32 0
  %50 = load i32, i32* %49
  %51 = call i32 @printf(i8* %43, i32 %50)
  %52 = getelementptr inbounds [12 x i8], [12 x i8]* @.str.9, i32 0, i32 0
  %53 = call i32 @printf(i8* %52)
  %54 = getelementptr inbounds [19 x i8], [19 x i8]* @.str.10, i32 0, i32 0
  %55 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %56 = load i32, i32* %i.addr1
  %57 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %55, i32 %56
  %58 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %57, i32 0, i32 1
  %59 = load i8, i8* %58
  %60 = call i32 @printf(i8* %54, i8 %59)
  %61 = getelementptr inbounds [19 x i8], [19 x i8]* @.str.11, i32 0, i32 0
  %62 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %63 = load i32, i32* %i.addr1
  %64 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %62, i32 %63
  %65 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %64, i32 0, i32 2
  %66 = load i8, i8* %65
  %67 = call i32 @printf(i8* %61, i8 %66)
  %68 = getelementptr inbounds [16 x i8], [16 x i8]* @.str.12, i32 0, i32 0
  %69 = call i32 @printf(i8* %68)
  %70 = getelementptr inbounds [24 x i8], [24 x i8]* @.str.13, i32 0, i32 0
  %71 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %72 = load i32, i32* %i.addr1
  %73 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %71, i32 %72
  %74 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %73, i32 0, i32 3
  %75 = getelementptr inbounds [2 x i32], [2 x i32]* %74, i32 0, i32 0
  %76 = load i32, i32* %75
  %77 = call i32 @printf(i8* %70, i32 %76)
  %78 = getelementptr inbounds [24 x i8], [24 x i8]* @.str.14, i32 0, i32 0
  %79 = load %struct..anon.2*, %struct..anon.2** %array.addr
  %80 = load i32, i32* %i.addr1
  %81 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %79, i32 %80
  %82 = getelementptr inbounds %struct..anon.2, %struct..anon.2* %81, i32 0, i32 3
  %83 = getelementptr inbounds [2 x i32], [2 x i32]* %82, i32 0, i32 1
  %84 = load i32, i32* %83
  %85 = call i32 @printf(i8* %78, i32 %84)
  %86 = getelementptr inbounds [12 x i8], [12 x i8]* @.str.15, i32 0, i32 0
  %87 = call i32 @printf(i8* %86)
  %88 = getelementptr inbounds [8 x i8], [8 x i8]* @.str.16, i32 0, i32 0
  %89 = call i32 @printf(i8* %88)
  br label %for.inc3
for.inc3:
  %90 = load i32, i32* %i.addr1
  %91 = add i32 %90, 1
  store i32 %91, i32* %i.addr1
  br label %for.cond1
for.end4:
  %92 = getelementptr inbounds [3 x i8], [3 x i8]* @.str.17, i32 0, i32 0
  %93 = call i32 @printf(i8* %92)
  br label %return0
return0:
  ret void
}

define dso_local i32 @main() {
entry:
  %retval = alloca i32
  store i32 0, i32* %retval
  %0 = getelementptr inbounds [9 x i8], [9 x i8]* @.str.18, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  %2 = load [4 x %struct..anon.2], [4 x %struct..anon.2]* @array1
  call void @print_array([4 x %struct..anon.2] %2)
  %3 = getelementptr inbounds [9 x i8], [9 x i8]* @.str.19, i32 0, i32 0
  %4 = call i32 @printf(i8* %3)
  %5 = load [4 x %struct..anon.2], [4 x %struct..anon.2]* @array2
  call void @print_array([4 x %struct..anon.2] %5)
  %6 = load [4 x %struct..anon.2], [4 x %struct..anon.2]* @array1
  %7 = load [4 x %struct..anon.2], [4 x %struct..anon.2]* @array2
  %8 = call i32 @memcmp([4 x %struct..anon.2] %6, [4 x %struct..anon.2] %7, i32 4)
  %9 = icmp ne i32 %8, 0
  br i1 %9, label %if.then6, label %if.else7
if.then6:
  %10 = getelementptr inbounds [43 x i8], [43 x i8]* @.str.20, i32 0, i32 0
  %11 = call i32 @printf(i8* %10)
  br label %if.end8
if.else7:
  %12 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.21, i32 0, i32 0
  %13 = call i32 @printf(i8* %12)
  br label %if.end8
if.end8:
  br label %return5
return5:
  %14 = load i32, i32* %retval
  ret i32 %14
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

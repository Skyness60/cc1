; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

%struct.T = type { %struct.anon.0, i8, [3 x i8], [2 x i32] }
%struct.anon.0 = type { float, [2 x i32], %struct.anon.1 }
%struct.anon.1 = type { i32 }
@array1 = dso_local global [4 x %struct.T] [%struct.T { %struct.anon.0 { float 1.0, [2 x i32] [i32 2, i32 0], %struct.anon.1 { i32 3 } }, i8 84, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, %struct.T { %struct.anon.0 { float 0.0, [2 x i32] zeroinitializer, %struct.anon.1 { i32 0 } }, i8 118, [3 x i8] zeroinitializer, [2 x i32] [i32 8, i32 9] }, %struct.T { %struct.anon.0 { float 10.0, [2 x i32] [i32 11, i32 12], %struct.anon.1 { i32 13 } }, i8 254, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }, %struct.T { %struct.anon.0 { float 16.0, [2 x i32] zeroinitializer, %struct.anon.1 { i32 0 } }, i8 0, [3 x i8] zeroinitializer, [2 x i32] zeroinitializer }]
@array2 = dso_local global [4 x %struct.T] [%struct.T { %struct.anon.0 { float 1.0, [2 x i32] [i32 2, i32 0], %struct.anon.1 { i32 3 } }, i8 84, [3 x i8] zeroinitializer, [2 x i32] [i32 0, i32 0] }, %struct.T { %struct.anon.0 { float 0.0, [2 x i32] [i32 0, i32 0], %struct.anon.1 { i32 0 } }, i8 118, [3 x i8] zeroinitializer, [2 x i32] [i32 8, i32 9] }, %struct.T { %struct.anon.0 { float 10.0, [2 x i32] [i32 11, i32 12], %struct.anon.1 { i32 13 } }, i8 254, [3 x i8] zeroinitializer, [2 x i32] [i32 0, i32 0] }, %struct.T { %struct.anon.0 { float 16.0, [2 x i32] [i32 0, i32 0], %struct.anon.1 { i32 0 } }, i8 0, [3 x i8] zeroinitializer, [2 x i32] [i32 0, i32 0] }]
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

define dso_local void @print_array(%struct.T* %0) {
entry:
  %array.addr = alloca %struct.T*
  store %struct.T* %0, %struct.T** %array.addr
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
  %14 = load %struct.T*, %struct.T** %array.addr
  %15 = load i32, i32* %i.addr1
  %16 = getelementptr inbounds %struct.T, %struct.T* %14, i32 %15
  %17 = getelementptr inbounds %struct.T, %struct.T* %16, i32 0, i32 0
  %18 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %17, i32 0, i32 0
  %19 = load float, float* %18
  %20 = fpext float %19 to double
  %21 = call i32 @printf(i8* %13, double %20)
  %22 = getelementptr inbounds [20 x i8], [20 x i8]* @.str.4, i32 0, i32 0
  %23 = call i32 @printf(i8* %22)
  %24 = getelementptr inbounds [28 x i8], [28 x i8]* @.str.5, i32 0, i32 0
  %25 = load %struct.T*, %struct.T** %array.addr
  %26 = load i32, i32* %i.addr1
  %27 = getelementptr inbounds %struct.T, %struct.T* %25, i32 %26
  %28 = getelementptr inbounds %struct.T, %struct.T* %27, i32 0, i32 0
  %29 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %28, i32 0, i32 1
  %30 = getelementptr inbounds [2 x i32], [2 x i32]* %29, i32 0, i32 0
  %31 = load i32, i32* %30
  %32 = call i32 @printf(i8* %24, i32 %31)
  %33 = getelementptr inbounds [28 x i8], [28 x i8]* @.str.6, i32 0, i32 0
  %34 = load %struct.T*, %struct.T** %array.addr
  %35 = load i32, i32* %i.addr1
  %36 = getelementptr inbounds %struct.T, %struct.T* %34, i32 %35
  %37 = getelementptr inbounds %struct.T, %struct.T* %36, i32 0, i32 0
  %38 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %37, i32 0, i32 1
  %39 = getelementptr inbounds [2 x i32], [2 x i32]* %38, i32 0, i32 1
  %40 = load i32, i32* %39
  %41 = call i32 @printf(i8* %33, i32 %40)
  %42 = getelementptr inbounds [16 x i8], [16 x i8]* @.str.7, i32 0, i32 0
  %43 = call i32 @printf(i8* %42)
  %44 = getelementptr inbounds [29 x i8], [29 x i8]* @.str.8, i32 0, i32 0
  %45 = load %struct.T*, %struct.T** %array.addr
  %46 = load i32, i32* %i.addr1
  %47 = getelementptr inbounds %struct.T, %struct.T* %45, i32 %46
  %48 = getelementptr inbounds %struct.T, %struct.T* %47, i32 0, i32 0
  %49 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %48, i32 0, i32 2
  %50 = getelementptr inbounds %struct.anon.1, %struct.anon.1* %49, i32 0, i32 0
  %51 = load i32, i32* %50
  %52 = call i32 @printf(i8* %44, i32 %51)
  %53 = getelementptr inbounds [12 x i8], [12 x i8]* @.str.9, i32 0, i32 0
  %54 = call i32 @printf(i8* %53)
  %55 = getelementptr inbounds [19 x i8], [19 x i8]* @.str.10, i32 0, i32 0
  %56 = load %struct.T*, %struct.T** %array.addr
  %57 = load i32, i32* %i.addr1
  %58 = getelementptr inbounds %struct.T, %struct.T* %56, i32 %57
  %59 = getelementptr inbounds %struct.T, %struct.T* %58, i32 0, i32 1
  %60 = load i8, i8* %59
  %61 = and i8 %60, 15
  %62 = zext i8 %61 to i32
  %63 = call i32 @printf(i8* %55, i32 %62)
  %64 = getelementptr inbounds [19 x i8], [19 x i8]* @.str.11, i32 0, i32 0
  %65 = load %struct.T*, %struct.T** %array.addr
  %66 = load i32, i32* %i.addr1
  %67 = getelementptr inbounds %struct.T, %struct.T* %65, i32 %66
  %68 = getelementptr inbounds %struct.T, %struct.T* %67, i32 0, i32 1
  %69 = load i8, i8* %68
  %70 = lshr i8 %69, 4
  %71 = and i8 %70, 15
  %72 = zext i8 %71 to i32
  %73 = call i32 @printf(i8* %64, i32 %72)
  %74 = getelementptr inbounds [16 x i8], [16 x i8]* @.str.12, i32 0, i32 0
  %75 = call i32 @printf(i8* %74)
  %76 = getelementptr inbounds [24 x i8], [24 x i8]* @.str.13, i32 0, i32 0
  %77 = load %struct.T*, %struct.T** %array.addr
  %78 = load i32, i32* %i.addr1
  %79 = getelementptr inbounds %struct.T, %struct.T* %77, i32 %78
  %80 = getelementptr inbounds %struct.T, %struct.T* %79, i32 0, i32 3
  %81 = getelementptr inbounds [2 x i32], [2 x i32]* %80, i32 0, i32 0
  %82 = load i32, i32* %81
  %83 = call i32 @printf(i8* %76, i32 %82)
  %84 = getelementptr inbounds [24 x i8], [24 x i8]* @.str.14, i32 0, i32 0
  %85 = load %struct.T*, %struct.T** %array.addr
  %86 = load i32, i32* %i.addr1
  %87 = getelementptr inbounds %struct.T, %struct.T* %85, i32 %86
  %88 = getelementptr inbounds %struct.T, %struct.T* %87, i32 0, i32 3
  %89 = getelementptr inbounds [2 x i32], [2 x i32]* %88, i32 0, i32 1
  %90 = load i32, i32* %89
  %91 = call i32 @printf(i8* %84, i32 %90)
  %92 = getelementptr inbounds [12 x i8], [12 x i8]* @.str.15, i32 0, i32 0
  %93 = call i32 @printf(i8* %92)
  %94 = getelementptr inbounds [8 x i8], [8 x i8]* @.str.16, i32 0, i32 0
  %95 = call i32 @printf(i8* %94)
  br label %for.inc3
for.inc3:
  %96 = load i32, i32* %i.addr1
  %97 = add i32 %96, 1
  store i32 %97, i32* %i.addr1
  br label %for.cond1
for.end4:
  %98 = getelementptr inbounds [3 x i8], [3 x i8]* @.str.17, i32 0, i32 0
  %99 = call i32 @printf(i8* %98)
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
  call void @print_array([4 x %struct.T]* @array1)
  %2 = getelementptr inbounds [9 x i8], [9 x i8]* @.str.19, i32 0, i32 0
  %3 = call i32 @printf(i8* %2)
  call void @print_array([4 x %struct.T]* @array2)
  %4 = bitcast [4 x %struct.T]* @array1 to i8*
  %5 = bitcast [4 x %struct.T]* @array2 to i8*
  %6 = call i32 @memcmp(i8* %4, i8* %5, i32 4)
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %if.then6, label %if.else7
if.then6:
  %8 = getelementptr inbounds [43 x i8], [43 x i8]* @.str.20, i32 0, i32 0
  %9 = call i32 @printf(i8* %8)
  br label %if.end8
if.else7:
  %10 = getelementptr inbounds [4 x i8], [4 x i8]* @.str.21, i32 0, i32 0
  %11 = call i32 @printf(i8* %10)
  br label %if.end8
if.end8:
  br label %return5
return5:
  %12 = load i32, i32* %retval
  ret i32 %12
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

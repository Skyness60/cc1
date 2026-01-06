; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

%struct.u = type { i8 }
@.str.0 = private unnamed_addr constant [24 x i8] c"alpha:\0Aposition: %d\0A%s\0A\00"
@.str.1 = private unnamed_addr constant [10 x i8] c"lowercase\00"
@.str.2 = private unnamed_addr constant [10 x i8] c"uppercase\00"
@.str.3 = private unnamed_addr constant [14 x i8] c"digit:\0An: %d\0A\00"
declare i32 @isalnum(i32)
declare i32 @printf(i8*, ...)

define dso_local i32 @main(i32 %0, i8** %1) {
entry:
  %ac.addr = alloca i32
  store i32 %0, i32* %ac.addr
  %av.addr = alloca i8**
  store i8** %1, i8*** %av.addr
  %retval = alloca i32
  store i32 0, i32* %retval
  %c.addr2 = alloca %struct.u
  %3 = load i32, i32* %ac.addr
  %4 = icmp ne i32 %3, 2
  %5 = zext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br label %land.lhs.eval7
land.lhs.eval7:
  br i1 %6, label %land.end6, label %land.rhs5
land.rhs5:
  %7 = load i8**, i8*** %av.addr
  %8 = getelementptr inbounds i8*, i8** %7, i32 1
  %9 = load i8*, i8** %8
  %10 = getelementptr inbounds i8, i8* %9, i32 0
  %11 = load i8, i8* %10
  %12 = icmp eq i8 %11, 0
  %13 = zext i1 %12 to i32
  %14 = icmp ne i32 %13, 0
  br label %land.rhs.from8
land.rhs.from8:
  br label %land.end6
land.end6:
  %15 = phi i1 [ true, %land.lhs.eval7 ], [ %14, %land.rhs.from8 ]
  %16 = zext i1 %15 to i32
  %17 = icmp ne i32 %16, 0
  br label %land.lhs.eval9
land.lhs.eval9:
  br i1 %17, label %land.end4, label %land.rhs3
land.rhs3:
  %18 = load i8**, i8*** %av.addr
  %19 = getelementptr inbounds i8*, i8** %18, i32 1
  %20 = load i8*, i8** %19
  %21 = getelementptr inbounds i8, i8* %20, i32 1
  %22 = load i8, i8* %21
  %23 = icmp ne i8 %22, 0
  br label %land.rhs.from10
land.rhs.from10:
  br label %land.end4
land.end4:
  %24 = phi i1 [ true, %land.lhs.eval9 ], [ %23, %land.rhs.from10 ]
  %25 = zext i1 %24 to i32
  %26 = icmp ne i32 %25, 0
  br i1 %26, label %if.then1, label %if.end2
if.then1:
  store i32 1, i32* %retval
  br label %return0
if.end2:
  %27 = load i8**, i8*** %av.addr
  %28 = getelementptr inbounds i8*, i8** %27, i32 1
  %29 = load i8*, i8** %28
  %30 = getelementptr inbounds i8, i8* %29, i32 0
  %31 = load i8, i8* %30
  %32 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  store i8 %31, i8* %32
  %33 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %34 = load i8, i8* %33
  %35 = call i32 @isalnum(i8 %34)
  %36 = icmp eq i32 %35, 0
  %37 = zext i1 %36 to i32
  %38 = icmp ne i32 %37, 0
  br i1 %38, label %if.then11, label %if.end12
if.then11:
  store i32 1, i32* %retval
  br label %return0
if.end12:
  %39 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %40 = load i8, i8* %39
  %41 = lshr i8 %40, 6
  %42 = and i8 %41, 1
  %43 = icmp ne i8 %42, 0
  br i1 %43, label %if.then13, label %if.else14
if.then13:
  %44 = getelementptr inbounds [24 x i8], [24 x i8]* @.str.0, i32 0, i32 0
  %45 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %46 = load i8, i8* %45
  %47 = and i8 %46, 31
  %48 = sext i8 %47 to i32
  %49 = sub i32 %48, 1
  %50 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %51 = load i8, i8* %50
  %52 = lshr i8 %51, 5
  %53 = and i8 %52, 1
  %54 = icmp ne i8 %53, 0
  br i1 %54, label %cond.true16, label %cond.false17
cond.true16:
  %55 = getelementptr inbounds [10 x i8], [10 x i8]* @.str.1, i32 0, i32 0
  br label %then.from19
then.from19:
  br label %cond.end18
cond.false17:
  %56 = getelementptr inbounds [10 x i8], [10 x i8]* @.str.2, i32 0, i32 0
  br label %else.from20
else.from20:
  br label %cond.end18
cond.end18:
  %57 = phi i8* [ %55, %then.from19 ], [ %56, %else.from20 ]
  %58 = call i32 @printf(i8* %44, i32 %49, i8* %57)
  br label %if.end15
if.else14:
  %59 = getelementptr inbounds [14 x i8], [14 x i8]* @.str.3, i32 0, i32 0
  %60 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %61 = load i8, i8* %60
  %62 = and i8 %61, 15
  %63 = call i32 @printf(i8* %59, i8 %62)
  br label %if.end15
if.end15:
  br label %return0
return0:
  %64 = load i32, i32* %retval
  ret i32 %64
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

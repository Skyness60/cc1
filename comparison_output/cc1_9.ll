; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

%struct.anon.0 = type { i8 }
%struct.anon.1 = type { i8 }
%struct.anon.2 = type { i8 }
%struct.u = type { %struct.anon.0 }
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
return.dead11:
  br label %if.end2
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
  %35 = sext i8 %34 to i32
  %36 = call i32 @isalnum(i32 %35)
  %37 = icmp eq i32 %36, 0
  %38 = zext i1 %37 to i32
  %39 = icmp ne i32 %38, 0
  br i1 %39, label %if.then12, label %if.end13
if.then12:
  store i32 1, i32* %retval
  br label %return0
return.dead14:
  br label %if.end13
if.end13:
  %40 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %41 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %40, i32 0, i32 0
  %42 = load i8, i8* %41
  %43 = lshr i8 %42, 6
  %44 = and i8 %43, 1
  %45 = zext i8 %44 to i32
  %46 = icmp ne i32 %45, 0
  br i1 %46, label %if.then15, label %if.else16
if.then15:
  %47 = getelementptr inbounds [24 x i8], [24 x i8]* @.str.0, i32 0, i32 0
  %48 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %49 = getelementptr inbounds %struct.anon.1, %struct.anon.1* %48, i32 0, i32 0
  %50 = load i8, i8* %49
  %51 = and i8 %50, 31
  %52 = zext i8 %51 to i32
  %53 = sub i32 %52, 1
  %54 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %55 = getelementptr inbounds %struct.anon.1, %struct.anon.1* %54, i32 0, i32 0
  %56 = load i8, i8* %55
  %57 = lshr i8 %56, 5
  %58 = and i8 %57, 1
  %59 = zext i8 %58 to i32
  %60 = icmp ne i32 %59, 0
  br i1 %60, label %cond.true18, label %cond.false19
cond.true18:
  %61 = getelementptr inbounds [10 x i8], [10 x i8]* @.str.1, i32 0, i32 0
  br label %then.from21
then.from21:
  br label %cond.end20
cond.false19:
  %62 = getelementptr inbounds [10 x i8], [10 x i8]* @.str.2, i32 0, i32 0
  br label %else.from22
else.from22:
  br label %cond.end20
cond.end20:
  %63 = phi i8* [ %61, %then.from21 ], [ %62, %else.from22 ]
  %64 = call i32 @printf(i8* %47, i32 %53, i8* %63)
  br label %if.end17
if.else16:
  %65 = getelementptr inbounds [14 x i8], [14 x i8]* @.str.3, i32 0, i32 0
  %66 = getelementptr inbounds %struct.u, %struct.u* %c.addr2, i32 0, i32 0
  %67 = getelementptr inbounds %struct.anon.2, %struct.anon.2* %66, i32 0, i32 0
  %68 = load i8, i8* %67
  %69 = and i8 %68, 15
  %70 = zext i8 %69 to i32
  %71 = call i32 @printf(i8* %65, i32 %70)
  br label %if.end17
if.end17:
  br label %return0
return0:
  %72 = load i32, i32* %retval
  ret i32 %72
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

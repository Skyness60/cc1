; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

declare i8* @malloc(i32)
declare i32 @puts(i8*)

define dso_local i32 @main() {
entry:
  %retval = alloca i32
  store i32 0, i32* %retval
  %array.addr0 = alloca i8*
  %i.addr1 = alloca i32
  store i32 0, i32* %i.addr1
  %p.addr2 = alloca i32*
  %3 = call i8* @malloc(i32 101)
  store i8* %3, i8** %array.addr0
  %4 = load i8*, i8** %array.addr0
  %5 = icmp eq i8* %4, null
  %6 = zext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %if.then1, label %if.end2
if.then1:
  store i32 1, i32* %retval
  br label %return0
if.end2:
  %8 = load i8*, i8** %array.addr0
  %9 = getelementptr inbounds i8, i8* %8, i32 100
  store i32 0, i8* %9
  br label %while.cond3
while.cond3:
  %10 = load i32, i32* %i.addr1
  %11 = icmp slt i32 %10, 100
  %12 = zext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %while.body4, label %while.end5
while.body4:
  %14 = load i32, i32* %i.addr1
  %15 = add i32 %14, 1
  store i32 %15, i32* %i.addr1
  %16 = load i8*, i8** %array.addr0
  %17 = getelementptr inbounds i8, i8* %16, i32 %14
  store i8 48, i8* %17
  br label %while.cond3
while.end5:
  store i32 0, i32* %i.addr1
  br label %while.cond6
while.cond6:
  %18 = sdiv i32 100, 2
  %19 = load i32, i32* %i.addr1
  %20 = icmp slt i32 %19, %18
  %21 = zext i1 %20 to i32
  %22 = icmp ne i32 %21, 0
  br i1 %22, label %while.body7, label %while.end8
while.body7:
  %23 = load i8*, i8** %array.addr0
  %24 = bitcast i8* %23 to i16*
  %25 = load i32, i32* %i.addr1
  %26 = add i32 %25, 1
  store i32 %26, i32* %i.addr1
  %27 = getelementptr inbounds i16, i16* %24, i32 %25
  %28 = load i16, i16* %27
  %29 = add i16 %28, 1
  store i16 %29, i16* %27
  br label %while.cond6
while.end8:
  %30 = load i8*, i8** %array.addr0
  %31 = bitcast i8* %30 to i32*
  store i32* %31, i32** %p.addr2
  br label %while.cond9
while.cond9:
  %32 = load i32*, i32** %p.addr2
  %33 = bitcast i32* %32 to i8*
  %34 = load i8*, i8** %array.addr0
  %35 = getelementptr inbounds i8, i8* %34, i32 100
  %36 = icmp ult i8* %33, %35
  %37 = zext i1 %36 to i32
  %38 = icmp ne i32 %37, 0
  br i1 %38, label %while.body10, label %while.end11
while.body10:
  %39 = load i32*, i32** %p.addr2
  %40 = getelementptr inbounds i32, i32* %39, i32 1
  store i32* %40, i32** %p.addr2
  %41 = load i32, i32* %39
  %42 = add i32 %41, 1
  store i32 %42, i32* %39
  br label %while.cond9
while.end11:
  %43 = load i8*, i8** %array.addr0
  %44 = getelementptr inbounds i8, i8* %43, i32 5
  store i8 42, i8* %44
  %45 = load i8*, i8** %array.addr0
  %46 = call i32 @puts(i8* %45)
  br label %return0
return0:
  %47 = load i32, i32* %retval
  ret i32 %47
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

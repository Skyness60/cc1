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
return.dead3:
  br label %if.end2
if.end2:
  %8 = load i8*, i8** %array.addr0
  %9 = getelementptr inbounds i8, i8* %8, i32 100
  %10 = trunc i32 0 to i8
  store i8 %10, i8* %9
  br label %while.cond4
while.cond4:
  %11 = load i32, i32* %i.addr1
  %12 = icmp slt i32 %11, 100
  %13 = zext i1 %12 to i32
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %while.body5, label %while.end6
while.body5:
  %15 = load i32, i32* %i.addr1
  %16 = add i32 %15, 1
  store i32 %16, i32* %i.addr1
  %17 = load i8*, i8** %array.addr0
  %18 = getelementptr inbounds i8, i8* %17, i32 %15
  store i8 48, i8* %18
  br label %while.cond4
while.end6:
  store i32 0, i32* %i.addr1
  br label %while.cond7
while.cond7:
  %19 = load i32, i32* %i.addr1
  %20 = sdiv i32 100, 2
  %21 = icmp slt i32 %19, %20
  %22 = zext i1 %21 to i32
  %23 = icmp ne i32 %22, 0
  br i1 %23, label %while.body8, label %while.end9
while.body8:
  %24 = load i8*, i8** %array.addr0
  %25 = bitcast i8* %24 to i16*
  %26 = load i32, i32* %i.addr1
  %27 = add i32 %26, 1
  store i32 %27, i32* %i.addr1
  %28 = getelementptr inbounds i16, i16* %25, i32 %26
  %29 = load i16, i16* %28
  %30 = add i16 %29, 1
  store i16 %30, i16* %28
  br label %while.cond7
while.end9:
  %31 = load i8*, i8** %array.addr0
  %32 = bitcast i8* %31 to i32*
  store i32* %32, i32** %p.addr2
  br label %while.cond10
while.cond10:
  %33 = load i32*, i32** %p.addr2
  %34 = bitcast i32* %33 to i8*
  %35 = load i8*, i8** %array.addr0
  %36 = getelementptr inbounds i8, i8* %35, i32 100
  %37 = icmp ult i8* %34, %36
  %38 = zext i1 %37 to i32
  %39 = icmp ne i32 %38, 0
  br i1 %39, label %while.body11, label %while.end12
while.body11:
  %40 = load i32*, i32** %p.addr2
  %41 = getelementptr inbounds i32, i32* %40, i32 1
  store i32* %41, i32** %p.addr2
  %42 = load i32, i32* %40
  %43 = add i32 %42, 1
  store i32 %43, i32* %40
  br label %while.cond10
while.end12:
  %44 = load i8*, i8** %array.addr0
  %45 = getelementptr inbounds i8, i8* %44, i32 5
  store i8 42, i8* %45
  %46 = load i8*, i8** %array.addr0
  %47 = call i32 @puts(i8* %46)
  br label %return0
return0:
  %48 = load i32, i32* %retval
  ret i32 %48
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

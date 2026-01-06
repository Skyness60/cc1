; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@.str.0 = private unnamed_addr constant [13 x i8] c"aeiouyAEIOUY\00"
@.str.1 = private unnamed_addr constant [11 x i8] c"%c: vowel\0A\00"
@.str.2 = private unnamed_addr constant [15 x i8] c"%c: consonant\0A\00"
@.str.3 = private unnamed_addr constant [18 x i8] c"%c: not a letter\0A\00"
declare i32 @isalpha(i32)
declare i32 @printf(i8*, ...)
declare i8* @strchr(i8*, i32)

define dso_local i32 @main(i32 %0, i8** %1) {
entry:
  %ac.addr = alloca i32
  store i32 %0, i32* %ac.addr
  %av.addr = alloca i8**
  store i8** %1, i8*** %av.addr
  %retval = alloca i32
  store i32 0, i32* %retval
  %s.addr2 = alloca i8*
  %3 = load i8**, i8*** %av.addr
  %4 = getelementptr inbounds i8*, i8** %3, i32 1
  %5 = load i8*, i8** %4
  store i8* %5, i8** %s.addr2
  %i.addr6 = alloca i32
  store i32 0, i32* %i.addr6
  %7 = load i32, i32* %ac.addr
  %8 = icmp ne i32 %7, 2
  %9 = zext i1 %8 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %if.then1, label %if.end2
if.then1:
  store i32 1, i32* %retval
  br label %return0
if.end2:
  br label %while.cond3
while.cond3:
  %11 = load i8*, i8** %s.addr2
  %12 = load i32, i32* %i.addr6
  %13 = getelementptr inbounds i8, i8* %11, i32 %12
  %14 = load i8, i8* %13
  %15 = icmp ne i8 %14, 0
  br i1 %15, label %while.body4, label %while.end5
while.body4:
  %16 = getelementptr inbounds [13 x i8], [13 x i8]* @.str.0, i32 0, i32 0
  %17 = load i8*, i8** %s.addr2
  %18 = load i32, i32* %i.addr6
  %19 = getelementptr inbounds i8, i8* %17, i32 %18
  %20 = load i8, i8* %19
  %21 = call i8* @strchr(i8* %16, i8 %20)
  %22 = icmp ne i8* %21, null
  br i1 %22, label %if.then6, label %if.else7
if.then6:
  %23 = getelementptr inbounds [11 x i8], [11 x i8]* @.str.1, i32 0, i32 0
  %24 = load i8*, i8** %s.addr2
  %25 = load i32, i32* %i.addr6
  %26 = getelementptr inbounds i8, i8* %24, i32 %25
  %27 = load i8, i8* %26
  %28 = call i32 @printf(i8* %23, i8 %27)
  br label %if.end8
if.else7:
  %29 = load i8*, i8** %s.addr2
  %30 = load i32, i32* %i.addr6
  %31 = getelementptr inbounds i8, i8* %29, i32 %30
  %32 = load i8, i8* %31
  %33 = call i32 @isalpha(i8 %32)
  %34 = icmp ne i32 %33, 0
  br i1 %34, label %if.then9, label %if.else10
if.then9:
  %35 = getelementptr inbounds [15 x i8], [15 x i8]* @.str.2, i32 0, i32 0
  %36 = load i8*, i8** %s.addr2
  %37 = load i32, i32* %i.addr6
  %38 = getelementptr inbounds i8, i8* %36, i32 %37
  %39 = load i8, i8* %38
  %40 = call i32 @printf(i8* %35, i8 %39)
  br label %if.end11
if.else10:
  %41 = getelementptr inbounds [18 x i8], [18 x i8]* @.str.3, i32 0, i32 0
  %42 = load i8*, i8** %s.addr2
  %43 = load i32, i32* %i.addr6
  %44 = getelementptr inbounds i8, i8* %42, i32 %43
  %45 = load i8, i8* %44
  %46 = call i32 @printf(i8* %41, i8 %45)
  br label %if.end11
if.end11:
  br label %if.end8
if.end8:
  %47 = load i32, i32* %i.addr6
  %48 = add i32 %47, 1
  store i32 %48, i32* %i.addr6
  br label %while.cond3
while.end5:
  br label %return0
return0:
  %49 = load i32, i32* %retval
  ret i32 %49
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

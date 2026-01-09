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
return.dead3:
  br label %if.end2
if.end2:
  br label %while.cond4
while.cond4:
  %11 = load i8*, i8** %s.addr2
  %12 = load i32, i32* %i.addr6
  %13 = getelementptr inbounds i8, i8* %11, i32 %12
  %14 = load i8, i8* %13
  %15 = icmp ne i8 %14, 0
  br i1 %15, label %while.body5, label %while.end6
while.body5:
  %16 = getelementptr inbounds [13 x i8], [13 x i8]* @.str.0, i32 0, i32 0
  %17 = load i8*, i8** %s.addr2
  %18 = load i32, i32* %i.addr6
  %19 = getelementptr inbounds i8, i8* %17, i32 %18
  %20 = load i8, i8* %19
  %21 = sext i8 %20 to i32
  %22 = call i8* @strchr(i8* %16, i32 %21)
  %23 = icmp ne i8* %22, null
  br i1 %23, label %if.then7, label %if.else8
if.then7:
  %24 = getelementptr inbounds [11 x i8], [11 x i8]* @.str.1, i32 0, i32 0
  %25 = load i8*, i8** %s.addr2
  %26 = load i32, i32* %i.addr6
  %27 = getelementptr inbounds i8, i8* %25, i32 %26
  %28 = load i8, i8* %27
  %29 = sext i8 %28 to i32
  %30 = call i32 @printf(i8* %24, i32 %29)
  br label %if.end9
if.else8:
  %31 = load i8*, i8** %s.addr2
  %32 = load i32, i32* %i.addr6
  %33 = getelementptr inbounds i8, i8* %31, i32 %32
  %34 = load i8, i8* %33
  %35 = sext i8 %34 to i32
  %36 = call i32 @isalpha(i32 %35)
  %37 = icmp ne i32 %36, 0
  br i1 %37, label %if.then10, label %if.else11
if.then10:
  %38 = getelementptr inbounds [15 x i8], [15 x i8]* @.str.2, i32 0, i32 0
  %39 = load i8*, i8** %s.addr2
  %40 = load i32, i32* %i.addr6
  %41 = getelementptr inbounds i8, i8* %39, i32 %40
  %42 = load i8, i8* %41
  %43 = sext i8 %42 to i32
  %44 = call i32 @printf(i8* %38, i32 %43)
  br label %if.end12
if.else11:
  %45 = getelementptr inbounds [18 x i8], [18 x i8]* @.str.3, i32 0, i32 0
  %46 = load i8*, i8** %s.addr2
  %47 = load i32, i32* %i.addr6
  %48 = getelementptr inbounds i8, i8* %46, i32 %47
  %49 = load i8, i8* %48
  %50 = sext i8 %49 to i32
  %51 = call i32 @printf(i8* %45, i32 %50)
  br label %if.end12
if.end12:
  br label %if.end9
if.end9:
  %52 = load i32, i32* %i.addr6
  %53 = add i32 %52, 1
  store i32 %53, i32* %i.addr6
  br label %while.cond4
while.end6:
  br label %return0
return0:
  %54 = load i32, i32* %retval
  ret i32 %54
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

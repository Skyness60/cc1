; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

declare i32 @puts(i8*)

define dso_local i32 @main(i32 %0, i8** %1) {
entry:
  %ac.addr = alloca i32
  store i32 %0, i32* %ac.addr
  %av.addr = alloca i8**
  store i8** %1, i8*** %av.addr
  %retval = alloca i32
  store i32 0, i32* %retval
  %i.addr2 = alloca i32
  store i32 0, i32* %i.addr2
  br label %while.cond1
while.cond1:
  %3 = load i32, i32* %ac.addr
  %4 = load i32, i32* %i.addr2
  %5 = sub i32 %3, %4
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %while.body2, label %while.end3
while.body2:
  %7 = load i8**, i8*** %av.addr
  %8 = load i32, i32* %i.addr2
  %9 = getelementptr inbounds i8, i8* %7, i32 %8
  %10 = load i8, i8* %9
  %11 = call i32 @puts(i8 %10)
  %12 = load i32, i32* %i.addr2
  %13 = add i32 %12, 1
  store i32 %13, i32* %i.addr2
  br label %while.cond1
while.end3:
  store i32 1, i32* %retval
  br label %return0
return.dead4:
  br label %return0
return0:
  %14 = load i32, i32* %retval
  ret i32 %14
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

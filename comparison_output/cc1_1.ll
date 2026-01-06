; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@.str.0 = private unnamed_addr constant [14 x i8] c"hello, world\0A\00"
declare i32 @printf(i8*, ...)

define dso_local i32 @main() {
entry:
  %retval = alloca i32
  store i32 0, i32* %retval
  %0 = getelementptr inbounds [14 x i8], [14 x i8]* @.str.0, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  br label %return0
return0:
  %2 = load i32, i32* %retval
  ret i32 %2
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

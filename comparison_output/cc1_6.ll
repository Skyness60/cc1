; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@size_t = dso_local global i32 0
declare void @free(i8*)
declare i8* @malloc(i32)
declare i32 @printf(i8*, ...)
declare i32 @puts(i8*)

define dso_local i32 @ft_strlen(i8* %0) {
entry:
  %str.addr = alloca i8*
  store i8* %0, i8** %str.addr
  %retval = alloca i32
  %a.addr1 = alloca i32
  store i32 0, i32* %a.addr1
  br label %while.cond1
while.cond1:
  %2 = load i8*, i8** %str.addr
  %3 = load i32, i32* %a.addr1
  %4 = getelementptr inbounds i8, i8* %2, i32 %3
  %5 = load i8, i8* %4
  %6 = icmp ne i8 %5, 0
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %while.body2, label %while.end3
while.body2:
  %9 = load i32, i32* %a.addr1
  %10 = add i32 %9, 1
  store i32 %10, i32* %a.addr1
  br label %while.cond1
while.end3:
  %11 = load i32, i32* %a.addr1
  store i32 %11, i32* %retval
  br label %return0
return0:
  %12 = load i32, i32* %retval
  ret i32 %12
}

define dso_local i8* @ft_memcpy(i8* %0, i8* %1, i32 %2) {
entry:
  %dst.addr = alloca i8*
  store i8* %0, i8** %dst.addr
  %src.addr = alloca i8*
  store i8* %1, i8** %src.addr
  %n.addr = alloca i32
  store i32 %2, i32* %n.addr
  %retval = alloca i8*
  %i.addr3 = alloca i32
  %4 = load i8*, i8** %dst.addr
  %5 = icmp eq i8* %4, null
  %6 = zext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br label %land.lhs.eval9
land.lhs.eval9:
  br i1 %7, label %land.end8, label %land.rhs7
land.rhs7:
  %8 = load i8*, i8** %src.addr
  %9 = icmp eq i8* %8, null
  %10 = zext i1 %9 to i32
  %11 = icmp ne i32 %10, 0
  br label %land.rhs.from10
land.rhs.from10:
  br label %land.end8
land.end8:
  %12 = phi i1 [ true, %land.lhs.eval9 ], [ %11, %land.rhs.from10 ]
  %13 = zext i1 %12 to i32
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %if.then5, label %if.end6
if.then5:
  %15 = inttoptr i32 0 to i8*
  store i8* %15, i8** %retval
  br label %return4
if.end6:
  store i32 0, i32* %i.addr3
  br label %for.cond11
for.cond11:
  %16 = load i32, i32* %n.addr
  %17 = sdiv i32 %16, 4
  %18 = load i32, i32* %i.addr3
  %19 = icmp slt i32 %18, %17
  %20 = zext i1 %19 to i32
  %21 = icmp ne i32 %20, 0
  br i1 %21, label %for.body12, label %for.end14
for.body12:
  %22 = load i8*, i8** %src.addr
  %23 = bitcast i8* %22 to i32*
  %24 = load i32, i32* %i.addr3
  %25 = getelementptr inbounds i32, i32* %23, i32 %24
  %26 = load i32, i32* %25
  %27 = load i8*, i8** %dst.addr
  %28 = bitcast i8* %27 to i32*
  %29 = load i32, i32* %i.addr3
  %30 = getelementptr inbounds i32, i32* %28, i32 %29
  store i32 %26, i32* %30
  br label %for.inc13
for.inc13:
  %31 = load i32, i32* %i.addr3
  %32 = add i32 %31, 1
  store i32 %32, i32* %i.addr3
  br label %for.cond11
for.end14:
  %33 = load i32, i32* %i.addr3
  %34 = mul i32 %33, 4
  store i32 %34, i32* %i.addr3
  br label %for.cond15
for.cond15:
  %35 = load i32, i32* %i.addr3
  %36 = load i32, i32* %n.addr
  %37 = icmp slt i32 %35, %36
  %38 = zext i1 %37 to i32
  %39 = icmp ne i32 %38, 0
  br i1 %39, label %for.body16, label %for.end18
for.body16:
  %40 = load i8*, i8** %src.addr
  %41 = load i32, i32* %i.addr3
  %42 = getelementptr inbounds i8, i8* %40, i32 %41
  %43 = load i8, i8* %42
  %44 = load i8*, i8** %dst.addr
  %45 = load i32, i32* %i.addr3
  %46 = getelementptr inbounds i8, i8* %44, i32 %45
  store i8 %43, i8* %46
  br label %for.inc17
for.inc17:
  %47 = load i32, i32* %i.addr3
  %48 = add i32 %47, 1
  store i32 %48, i32* %i.addr3
  br label %for.cond15
for.end18:
  %49 = load i8*, i8** %dst.addr
  store i8* %49, i8** %retval
  br label %return4
return4:
  %50 = load i8*, i8** %retval
  ret i8* %50
}

define dso_local i8* @ft_strdup(i8* %0) {
entry:
  %s1.addr = alloca i8*
  store i8* %0, i8** %s1.addr
  %retval = alloca i8*
  %dest.addr1 = alloca i8*
  %len.addr2 = alloca i32
  %3 = load i8*, i8** %s1.addr
  %4 = call i32 @ft_strlen(i8* %3)
  store i32 %4, i32* %len.addr2
  %5 = load i32, i32* %len.addr2
  %6 = add i32 %5, 1
  %7 = call i8* @malloc(i32 %6)
  store i8* %7, i8** %dest.addr1
  %8 = load i8*, i8** %dest.addr1
  %9 = icmp ne i8* %8, null
  br i1 %9, label %if.then20, label %if.end21
if.then20:
  %10 = load i8*, i8** %dest.addr1
  %11 = load i8*, i8** %s1.addr
  %12 = load i32, i32* %len.addr2
  %13 = add i32 %12, 1
  %14 = call i8* @ft_memcpy(i8* %10, i8* %11, i32 %13)
  store i8* %14, i8** %dest.addr1
  br label %if.end21
if.end21:
  %15 = load i8*, i8** %dest.addr1
  store i8* %15, i8** %retval
  br label %return19
return19:
  %16 = load i8*, i8** %retval
  ret i8* %16
}

define dso_local i8* @ft_strmapi(i8* %0, i8* (i32, i8)* %1) {
entry:
  %s.addr = alloca i8*
  store i8* %0, i8** %s.addr
  %f.addr = alloca i8* (i32, i8)*
  store i8* (i32, i8)* %1, i8* (i32, i8)** %f.addr
  %retval = alloca i8*
  %i.addr2 = alloca i32
  %dst.addr3 = alloca i8*
  %4 = inttoptr i32 0 to i8*
  store i8* %4, i8** %dst.addr3
  %5 = load i8*, i8** %s.addr
  %6 = icmp eq i8* %5, null
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br label %land.lhs.eval27
land.lhs.eval27:
  br i1 %8, label %land.end26, label %land.rhs25
land.rhs25:
  %9 = load i8* (i32, i8)*, i8* (i32, i8)** %f.addr
  %10 = icmp eq i8* (i32, i8)* %9, null
  %11 = zext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br label %land.rhs.from28
land.rhs.from28:
  br label %land.end26
land.end26:
  %13 = phi i1 [ true, %land.lhs.eval27 ], [ %12, %land.rhs.from28 ]
  %14 = zext i1 %13 to i32
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %if.then23, label %if.end24
if.then23:
  br label %label.end29
if.end24:
  %16 = sub i32 0, 1
  store i32 %16, i32* %i.addr2
  br label %label.iamalabel30
label.map31:
  %17 = load i8* (i32, i8)*, i8* (i32, i8)** %f.addr
  %18 = load i32, i32* %i.addr2
  %19 = load i8*, i8** %dst.addr3
  %20 = load i32, i32* %i.addr2
  %21 = getelementptr inbounds i8, i8* %19, i32 %20
  %22 = load i8, i8* %21
  %23 = call i32 %17(i32 %18, i8 %22)
  %24 = load i8*, i8** %dst.addr3
  %25 = load i32, i32* %i.addr2
  %26 = getelementptr inbounds i8, i8* %24, i32 %25
  store i32 %23, i8* %26
  br label %label.loop32
label.iamalabel30:
  %27 = load i8*, i8** %s.addr
  %28 = call i8* @ft_strdup(i8* %27)
  store i8* %28, i8** %dst.addr3
  %29 = load i8*, i8** %dst.addr3
  %30 = icmp eq i8* %29, null
  %31 = zext i1 %30 to i32
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %if.then33, label %if.end34
if.then33:
  br label %label.end29
if.end34:
  br label %for.cond35
for.cond35:
  br label %for.body36
for.body36:
  br label %label.cond39
label.loop32:
  br label %for.inc37
label.cond39:
  %33 = load i32, i32* %i.addr2
  %34 = add i32 %33, 1
  store i32 %34, i32* %i.addr2
  %35 = load i8*, i8** %dst.addr3
  %36 = getelementptr inbounds i8, i8* %35, i32 %34
  %37 = load i8, i8* %36
  %38 = icmp eq i8 %37, 0
  %39 = zext i1 %38 to i32
  %40 = icmp ne i32 %39, 0
  br i1 %40, label %if.then40, label %if.else41
if.then40:
  br label %for.end38
if.else41:
  br label %label.map31
if.end42:
  br label %for.inc37
for.inc37:
  br label %for.cond35
for.end38:
  br label %label.end29
label.end29:
  %41 = load i8*, i8** %dst.addr3
  store i8* %41, i8** %retval
  br label %return22
return22:
  %42 = load i8*, i8** %retval
  ret i8* %42
}

define dso_local i32 @factorial(i32 %0) {
entry:
  %n.addr = alloca i32
  store i32 %0, i32* %n.addr
  %retval = alloca i32
  %1 = load i32, i32* %n.addr
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %if.then44, label %if.end45
if.then44:
  %3 = load i32, i32* %n.addr
  %4 = sub i32 %3, 1
  %5 = call i32 @factorial(i32 %4)
  %6 = load i32, i32* %n.addr
  %7 = mul i32 %6, %5
  store i32 %7, i32* %retval
  br label %return43
if.end45:
  store i32 1, i32* %retval
  br label %return43
return43:
  %8 = load i32, i32* %retval
  ret i32 %8
}

define dso_local i32 @ft_isalpha(i32 %0) {
entry:
  %c.addr = alloca i32
  store i32 %0, i32* %c.addr
  %retval = alloca i32
  %1 = load i32, i32* %c.addr
  %2 = or i32 %1, 32
  store i32 %2, i32* %c.addr
  %3 = load i32, i32* %c.addr
  %4 = sext i8 97 to i32
  %5 = icmp sge i32 %3, %4
  %6 = zext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br label %land.lhs.eval49
land.lhs.eval49:
  br i1 %7, label %land.rhs47, label %land.end48
land.rhs47:
  %8 = load i32, i32* %c.addr
  %9 = sext i8 122 to i32
  %10 = icmp sle i32 %8, %9
  %11 = zext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br label %land.rhs.from50
land.rhs.from50:
  br label %land.end48
land.end48:
  %13 = phi i1 [ false, %land.lhs.eval49 ], [ %12, %land.rhs.from50 ]
  %14 = zext i1 %13 to i32
  store i32 %14, i32* %retval
  br label %return46
return46:
  %15 = load i32, i32* %retval
  ret i32 %15
}

define dso_local i8 @rot(i32 %0, i8 %1) {
entry:
  %n.addr = alloca i32
  store i32 %0, i32* %n.addr
  %c.addr = alloca i8
  store i8 %1, i8* %c.addr
  %retval = alloca i8
  %2 = load i8, i8* %c.addr
  %3 = call i32 @ft_isalpha(i8 %2)
  %4 = icmp eq i32 %3, 0
  %5 = zext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %if.then52, label %if.end53
if.then52:
  %7 = load i8, i8* %c.addr
  store i8 %7, i8* %retval
  br label %return51
if.end53:
  %8 = load i8, i8* %c.addr
  %9 = icmp sle i8 %8, 90
  %10 = zext i1 %9 to i32
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %if.then54, label %if.else55
if.then54:
  %12 = sext i8 65 to i32
  %13 = load i8, i8* %c.addr
  %14 = sext i8 %13 to i32
  %15 = sub i32 %14, %12
  %16 = load i32, i32* %n.addr
  %17 = srem i32 %16, 10
  %18 = call i32 @factorial(i32 %17)
  %19 = add i32 %15, %18
  %20 = load i32, i32* %n.addr
  %21 = sdiv i32 %20, 10
  %22 = add i32 %19, %21
  %23 = srem i32 %22, 26
  %24 = sext i8 65 to i32
  %25 = add i32 %24, %23
  %26 = trunc i32 %25 to i8
  store i8 %26, i8* %retval
  br label %return51
if.else55:
  %27 = sext i8 97 to i32
  %28 = load i8, i8* %c.addr
  %29 = sext i8 %28 to i32
  %30 = sub i32 %29, %27
  %31 = load i32, i32* %n.addr
  %32 = srem i32 %31, 10
  %33 = call i32 @factorial(i32 %32)
  %34 = add i32 %30, %33
  %35 = load i32, i32* %n.addr
  %36 = sdiv i32 %35, 10
  %37 = add i32 %34, %36
  %38 = srem i32 %37, 26
  %39 = sext i8 97 to i32
  %40 = add i32 %39, %38
  %41 = trunc i32 %40 to i8
  store i8 %41, i8* %retval
  br label %return51
if.end56:
  br label %return51
return51:
  %42 = load i8, i8* %retval
  ret i8 %42
}

define dso_local i32 @main(i32 %0, i8** %1) {
entry:
  %ac.addr = alloca i32
  store i32 %0, i32* %ac.addr
  %av.addr = alloca i8**
  store i8** %1, i8*** %av.addr
  %retval = alloca i32
  store i32 0, i32* %retval
  %i.addr2 = alloca i32
  store i32 1, i32* %i.addr2
  br label %while.cond58
while.cond58:
  %3 = load i32, i32* %i.addr2
  %4 = load i32, i32* %ac.addr
  %5 = icmp slt i32 %3, %4
  %6 = zext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %while.body59, label %while.end60
while.body59:
  %s.addr8 = alloca i8*
  %9 = load i8**, i8*** %av.addr
  %10 = load i32, i32* %i.addr2
  %11 = getelementptr inbounds i8*, i8** %9, i32 %10
  %12 = load i8*, i8** %11
  %13 = call i8* @ft_strmapi(i8* %12, i8* @rot)
  store i8* %13, i8** %s.addr8
  %14 = load i8*, i8** %s.addr8
  %15 = call i32 @puts(i8* %14)
  %16 = load i8*, i8** %s.addr8
  call void @free(i8* %16)
  %17 = load i32, i32* %i.addr2
  %18 = add i32 %17, 1
  store i32 %18, i32* %i.addr2
  br label %while.cond58
while.end60:
  br label %return57
return57:
  %19 = load i32, i32* %retval
  ret i32 %19
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

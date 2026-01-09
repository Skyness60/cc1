; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

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
return.dead4:
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
  br label %land.lhs.eval10
land.lhs.eval10:
  br i1 %7, label %land.end9, label %land.rhs8
land.rhs8:
  %8 = load i8*, i8** %src.addr
  %9 = icmp eq i8* %8, null
  %10 = zext i1 %9 to i32
  %11 = icmp ne i32 %10, 0
  br label %land.rhs.from11
land.rhs.from11:
  br label %land.end9
land.end9:
  %12 = phi i1 [ true, %land.lhs.eval10 ], [ %11, %land.rhs.from11 ]
  %13 = zext i1 %12 to i32
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %if.then6, label %if.end7
if.then6:
  %15 = inttoptr i32 0 to i8*
  store i8* %15, i8** %retval
  br label %return5
return.dead12:
  br label %if.end7
if.end7:
  store i32 0, i32* %i.addr3
  br label %for.cond13
for.cond13:
  %16 = load i32, i32* %i.addr3
  %17 = load i32, i32* %n.addr
  %18 = sdiv i32 %17, 4
  %19 = icmp slt i32 %16, %18
  %20 = zext i1 %19 to i32
  %21 = icmp ne i32 %20, 0
  br i1 %21, label %for.body14, label %for.end16
for.body14:
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
  br label %for.inc15
for.inc15:
  %31 = load i32, i32* %i.addr3
  %32 = add i32 %31, 1
  store i32 %32, i32* %i.addr3
  br label %for.cond13
for.end16:
  %33 = load i32, i32* %i.addr3
  %34 = mul i32 %33, 4
  store i32 %34, i32* %i.addr3
  br label %for.cond17
for.cond17:
  %35 = load i32, i32* %i.addr3
  %36 = load i32, i32* %n.addr
  %37 = icmp slt i32 %35, %36
  %38 = zext i1 %37 to i32
  %39 = icmp ne i32 %38, 0
  br i1 %39, label %for.body18, label %for.end20
for.body18:
  %40 = load i8*, i8** %src.addr
  %41 = load i32, i32* %i.addr3
  %42 = getelementptr inbounds i8, i8* %40, i32 %41
  %43 = load i8, i8* %42
  %44 = load i8*, i8** %dst.addr
  %45 = load i32, i32* %i.addr3
  %46 = getelementptr inbounds i8, i8* %44, i32 %45
  store i8 %43, i8* %46
  br label %for.inc19
for.inc19:
  %47 = load i32, i32* %i.addr3
  %48 = add i32 %47, 1
  store i32 %48, i32* %i.addr3
  br label %for.cond17
for.end20:
  %49 = load i8*, i8** %dst.addr
  store i8* %49, i8** %retval
  br label %return5
return.dead21:
  br label %return5
return5:
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
  br i1 %9, label %if.then23, label %if.end24
if.then23:
  %10 = load i8*, i8** %dest.addr1
  %11 = load i8*, i8** %s1.addr
  %12 = load i32, i32* %len.addr2
  %13 = add i32 %12, 1
  %14 = call i8* @ft_memcpy(i8* %10, i8* %11, i32 %13)
  store i8* %14, i8** %dest.addr1
  br label %if.end24
if.end24:
  %15 = load i8*, i8** %dest.addr1
  store i8* %15, i8** %retval
  br label %return22
return.dead25:
  br label %return22
return22:
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
  br label %land.lhs.eval31
land.lhs.eval31:
  br i1 %8, label %land.end30, label %land.rhs29
land.rhs29:
  %9 = load i8* (i32, i8)*, i8* (i32, i8)** %f.addr
  %10 = icmp eq i8* (i32, i8)* %9, null
  %11 = zext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br label %land.rhs.from32
land.rhs.from32:
  br label %land.end30
land.end30:
  %13 = phi i1 [ true, %land.lhs.eval31 ], [ %12, %land.rhs.from32 ]
  %14 = zext i1 %13 to i32
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %if.then27, label %if.end28
if.then27:
  br label %label.end33
goto.dead34:
  br label %if.end28
if.end28:
  %16 = sub i32 0, 1
  store i32 %16, i32* %i.addr2
  br label %label.iamalabel35
goto.dead36:
  br label %label.map37
label.map37:
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
  %27 = trunc i32 %23 to i8
  store i8 %27, i8* %26
  br label %label.loop38
goto.dead39:
  br label %label.iamalabel35
label.iamalabel35:
  %28 = load i8*, i8** %s.addr
  %29 = call i8* @ft_strdup(i8* %28)
  store i8* %29, i8** %dst.addr3
  %30 = load i8*, i8** %dst.addr3
  %31 = icmp eq i8* %30, null
  %32 = zext i1 %31 to i32
  %33 = icmp ne i32 %32, 0
  br i1 %33, label %if.then40, label %if.end41
if.then40:
  br label %label.end33
goto.dead42:
  br label %if.end41
if.end41:
  br label %for.cond43
for.cond43:
  br label %for.body44
for.body44:
  br label %label.cond47
goto.dead48:
  br label %label.loop38
label.loop38:
  br label %for.inc45
continue.dead49:
  br label %label.cond47
label.cond47:
  %34 = load i32, i32* %i.addr2
  %35 = add i32 %34, 1
  store i32 %35, i32* %i.addr2
  %36 = load i8*, i8** %dst.addr3
  %37 = getelementptr inbounds i8, i8* %36, i32 %35
  %38 = load i8, i8* %37
  %39 = icmp eq i8 %38, 0
  %40 = zext i1 %39 to i32
  %41 = icmp ne i32 %40, 0
  br i1 %41, label %if.then50, label %if.else51
if.then50:
  br label %for.end46
break.dead53:
  br label %if.end52
if.else51:
  br label %label.map37
goto.dead54:
  br label %if.end52
if.end52:
  br label %for.inc45
for.inc45:
  br label %for.cond43
for.end46:
  br label %label.end33
label.end33:
  %42 = load i8*, i8** %dst.addr3
  store i8* %42, i8** %retval
  br label %return26
return.dead55:
  br label %return26
return26:
  %43 = load i8*, i8** %retval
  ret i8* %43
}

define dso_local i32 @factorial(i32 %0) {
entry:
  %n.addr = alloca i32
  store i32 %0, i32* %n.addr
  %retval = alloca i32
  %1 = load i32, i32* %n.addr
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %if.then57, label %if.end58
if.then57:
  %3 = load i32, i32* %n.addr
  %4 = load i32, i32* %n.addr
  %5 = sub i32 %4, 1
  %6 = call i32 @factorial(i32 %5)
  %7 = mul i32 %3, %6
  store i32 %7, i32* %retval
  br label %return56
return.dead59:
  br label %if.end58
if.end58:
  store i32 1, i32* %retval
  br label %return56
return.dead60:
  br label %return56
return56:
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
  br label %land.lhs.eval64
land.lhs.eval64:
  br i1 %7, label %land.rhs62, label %land.end63
land.rhs62:
  %8 = load i32, i32* %c.addr
  %9 = sext i8 122 to i32
  %10 = icmp sle i32 %8, %9
  %11 = zext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br label %land.rhs.from65
land.rhs.from65:
  br label %land.end63
land.end63:
  %13 = phi i1 [ false, %land.lhs.eval64 ], [ %12, %land.rhs.from65 ]
  %14 = zext i1 %13 to i32
  store i32 %14, i32* %retval
  br label %return61
return.dead66:
  br label %return61
return61:
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
  br i1 %6, label %if.then68, label %if.end69
if.then68:
  %7 = load i8, i8* %c.addr
  store i8 %7, i8* %retval
  br label %return67
return.dead70:
  br label %if.end69
if.end69:
  %8 = load i8, i8* %c.addr
  %9 = icmp sle i8 %8, 90
  %10 = zext i1 %9 to i32
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %if.then71, label %if.else72
if.then71:
  %12 = load i8, i8* %c.addr
  %13 = sext i8 65 to i32
  %14 = sext i8 %12 to i32
  %15 = sub i32 %14, %13
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
  br label %return67
return.dead74:
  br label %if.end73
if.else72:
  %27 = load i8, i8* %c.addr
  %28 = sext i8 97 to i32
  %29 = sext i8 %27 to i32
  %30 = sub i32 %29, %28
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
  br label %return67
return.dead75:
  br label %if.end73
if.end73:
  br label %return67
return67:
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
  br label %while.cond77
while.cond77:
  %3 = load i32, i32* %i.addr2
  %4 = load i32, i32* %ac.addr
  %5 = icmp slt i32 %3, %4
  %6 = zext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %while.body78, label %while.end79
while.body78:
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
  br label %while.cond77
while.end79:
  br label %return76
return76:
  %19 = load i32, i32* %retval
  ret i32 %19
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

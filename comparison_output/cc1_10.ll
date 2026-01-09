; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

declare i8* @calloc(i32, i32)
declare void @free(i8*)
declare i32 @getchar()
declare i8* @memset(i8*, i32, i32)
declare i32 @puts(i8*)
declare i8* @realloc(i8*, i32)
declare i32 @strlen(i8*)

define dso_local i8* @add_char(i8* %0, i32* %1, i8 %2) {
entry:
  %str.addr = alloca i8*
  store i8* %0, i8** %str.addr
  %capacity.addr = alloca i32*
  store i32* %1, i32** %capacity.addr
  %c.addr = alloca i8
  store i8 %2, i8* %c.addr
  %retval = alloca i8*
  %3 = load i8*, i8** %str.addr
  %4 = icmp eq i8* %3, null
  %5 = zext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %if.then1, label %if.else2
if.then1:
  %7 = load i32*, i32** %capacity.addr
  store i32 1, i32* %7
  %8 = load i32*, i32** %capacity.addr
  %9 = load i32, i32* %8
  %10 = call i8* @calloc(i32 %9, i32 1)
  store i8* %10, i8** %str.addr
  %11 = load i8, i8* %c.addr
  %12 = load i8*, i8** %str.addr
  %13 = getelementptr inbounds i8, i8* %12, i32 0
  store i8 %11, i8* %13
  br label %if.end3
if.else2:
  %size.addr14 = alloca i32
  %15 = load i8*, i8** %str.addr
  %16 = call i32 @strlen(i8* %15)
  store i32 %16, i32* %size.addr14
  %17 = load i32, i32* %size.addr14
  %18 = load i32*, i32** %capacity.addr
  %19 = load i32, i32* %18
  %20 = icmp sge i32 %17, %19
  %21 = zext i1 %20 to i32
  %22 = icmp ne i32 %21, 0
  br i1 %22, label %if.then4, label %if.end5
if.then4:
  %23 = load i32*, i32** %capacity.addr
  %24 = load i32, i32* %23
  %25 = mul i32 %24, 2
  store i32 %25, i32* %23
  %26 = load i8*, i8** %str.addr
  %27 = load i32*, i32** %capacity.addr
  %28 = load i32, i32* %27
  %29 = call i8* @realloc(i8* %26, i32 %28)
  store i8* %29, i8** %str.addr
  %30 = load i8*, i8** %str.addr
  %31 = load i32, i32* %size.addr14
  %32 = getelementptr inbounds i8, i8* %30, i32 %31
  %33 = load i32*, i32** %capacity.addr
  %34 = load i32, i32* %33
  %35 = load i32, i32* %size.addr14
  %36 = sub i32 %34, %35
  %37 = call i8* @memset(i8* %32, i32 0, i32 %36)
  br label %if.end5
if.end5:
  %38 = load i8, i8* %c.addr
  %39 = load i8*, i8** %str.addr
  %40 = load i32, i32* %size.addr14
  %41 = getelementptr inbounds i8, i8* %39, i32 %40
  store i8 %38, i8* %41
  br label %if.end3
if.end3:
  %42 = load i8*, i8** %str.addr
  store i8* %42, i8** %retval
  br label %return0
return.dead6:
  br label %return0
return0:
  %43 = load i8*, i8** %retval
  ret i8* %43
}

define dso_local i8* @add_unicode(i8* %0, i32* %1, i16 %2) {
entry:
  %str.addr = alloca i8*
  store i8* %0, i8** %str.addr
  %capacity.addr = alloca i32*
  store i32* %1, i32** %capacity.addr
  %unicode.addr = alloca i16
  store i16 %2, i16* %unicode.addr
  %retval = alloca i8*
  %n.addr3 = alloca i32
  %4 = load i16, i16* %unicode.addr
  %5 = sext i16 %4 to i32
  %6 = icmp sle i32 %5, 127
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %if.then8, label %if.end9
if.then8:
  %9 = load i8*, i8** %str.addr
  %10 = load i32*, i32** %capacity.addr
  %11 = load i16, i16* %unicode.addr
  %12 = call i8* @add_char(i8* %9, i32* %10, i16 %11)
  store i8* %12, i8** %retval
  br label %return7
return.dead10:
  br label %if.end9
if.end9:
  store i32 1, i32* %n.addr3
  br label %for.cond11
for.cond11:
  %13 = load i16, i16* %unicode.addr
  %14 = load i32, i32* %n.addr3
  %15 = mul i32 6, %14
  %16 = load i32, i32* %n.addr3
  %17 = sub i32 %15, %16
  %18 = add i32 %17, 1
  %19 = sext i16 %13 to i32
  %20 = ashr i32 %19, %18
  %21 = icmp ne i32 %20, 0
  br i1 %21, label %for.body12, label %for.end14
for.body12:
  br label %for.inc13
for.inc13:
  %22 = load i32, i32* %n.addr3
  %23 = add i32 %22, 1
  store i32 %23, i32* %n.addr3
  br label %for.cond11
for.end14:
  %24 = load i8*, i8** %str.addr
  %25 = load i32*, i32** %capacity.addr
  %26 = load i32, i32* %n.addr3
  %27 = sub i32 8, %26
  %28 = shl i32 255, %27
  %29 = load i16, i16* %unicode.addr
  %30 = load i32, i32* %n.addr3
  %31 = sub i32 %30, 1
  %32 = mul i32 6, %31
  %33 = sext i16 %29 to i32
  %34 = ashr i32 %33, %32
  %35 = or i32 %28, %34
  %36 = call i8* @add_char(i8* %24, i32* %25, i32 %35)
  store i8* %36, i8** %str.addr
  %37 = load i32, i32* %n.addr3
  %38 = sub i32 %37, 1
  store i32 %38, i32* %n.addr3
  br label %for.cond15
for.cond15:
  %39 = load i32, i32* %n.addr3
  %40 = icmp sgt i32 %39, 0
  %41 = zext i1 %40 to i32
  %42 = icmp ne i32 %41, 0
  br i1 %42, label %for.body16, label %for.end18
for.body16:
  %43 = load i8*, i8** %str.addr
  %44 = load i32*, i32** %capacity.addr
  %45 = load i16, i16* %unicode.addr
  %46 = load i32, i32* %n.addr3
  %47 = sub i32 %46, 1
  %48 = mul i32 6, %47
  %49 = sext i16 %45 to i32
  %50 = ashr i32 %49, %48
  %51 = ashr i32 255, 2
  %52 = and i32 %50, %51
  %53 = shl i32 255, 7
  %54 = or i32 %52, %53
  %55 = call i8* @add_char(i8* %43, i32* %44, i32 %54)
  store i8* %55, i8** %str.addr
  br label %for.inc17
for.inc17:
  %56 = load i32, i32* %n.addr3
  %57 = sub i32 %56, 1
  store i32 %57, i32* %n.addr3
  br label %for.cond15
for.end18:
  %58 = load i8*, i8** %str.addr
  store i8* %58, i8** %retval
  br label %return7
return.dead19:
  br label %return7
return7:
  %59 = load i8*, i8** %retval
  ret i8* %59
}

define dso_local i8* @parse_str() {
entry:
  %retval = alloca i8*
  %str.addr0 = alloca i8*
  %1 = inttoptr i32 0 to i8*
  store i8* %1, i8** %str.addr0
  %c.addr2 = alloca i8
  %capacity.addr3 = alloca i32
  store i32 0, i32* %capacity.addr3
  br label %while.cond21
while.cond21:
  %4 = icmp ne i32 1, 0
  br i1 %4, label %while.body22, label %while.end23
while.body22:
  %5 = call i32 @getchar()
  %6 = trunc i32 %5 to i8
  store i8 %6, i8* %c.addr2
  switch i32 %5, label %sw.default27 [ i32 34, label %sw.case25 i32 92, label %sw.case26 i32 -1, label %sw.case28 ]
sw.after29:
  br label %sw.end24
sw.case25:
  %7 = load i8*, i8** %str.addr0
  store i8* %7, i8** %retval
  br label %return20
return.dead30:
  br label %sw.case26
sw.case26:
  %8 = call i32 @getchar()
  %9 = trunc i32 %8 to i8
  store i8 %9, i8* %c.addr2
  switch i32 %8, label %sw.default41 [ i32 98, label %sw.case32 i32 102, label %sw.case33 i32 110, label %sw.case34 i32 114, label %sw.case35 i32 116, label %sw.case36 i32 117, label %sw.case37 i32 34, label %sw.case38 i32 92, label %sw.case39 i32 47, label %sw.case40 i32 -1, label %sw.case42 ]
sw.after43:
  br label %sw.end31
sw.case32:
  %10 = load i8*, i8** %str.addr0
  %11 = call i8* @add_char(i8* %10, i32* %capacity.addr3, i8 8)
  store i8* %11, i8** %str.addr0
  br label %sw.end31
break.dead44:
  br label %sw.case33
sw.case33:
  %12 = load i8*, i8** %str.addr0
  %13 = call i8* @add_char(i8* %12, i32* %capacity.addr3, i8 12)
  store i8* %13, i8** %str.addr0
  br label %sw.end31
break.dead45:
  br label %sw.case34
sw.case34:
  %14 = load i8*, i8** %str.addr0
  %15 = call i8* @add_char(i8* %14, i32* %capacity.addr3, i8 10)
  store i8* %15, i8** %str.addr0
  br label %sw.end31
break.dead46:
  br label %sw.case35
sw.case35:
  %16 = load i8*, i8** %str.addr0
  %17 = call i8* @add_char(i8* %16, i32* %capacity.addr3, i8 13)
  store i8* %17, i8** %str.addr0
  br label %sw.end31
break.dead47:
  br label %sw.case36
sw.case36:
  %18 = load i8*, i8** %str.addr0
  %19 = call i8* @add_char(i8* %18, i32* %capacity.addr3, i8 9)
  store i8* %19, i8** %str.addr0
  br label %sw.end31
break.dead48:
  br label %sw.case37
sw.case37:
  %i.addr20 = alloca i32
  %hex.addr21 = alloca i16
  %22 = trunc i32 0 to i16
  store i16 %22, i16* %hex.addr21
  store i32 0, i32* %i.addr20
  br label %for.cond49
for.cond49:
  %23 = load i32, i32* %i.addr20
  %24 = icmp slt i32 %23, 4
  %25 = zext i1 %24 to i32
  %26 = icmp ne i32 %25, 0
  br i1 %26, label %for.body50, label %for.end52
for.body50:
  %27 = load i16, i16* %hex.addr21
  %28 = trunc i32 16 to i16
  %29 = mul i16 %27, %28
  store i16 %29, i16* %hex.addr21
  %30 = call i32 @getchar()
  %31 = trunc i32 %30 to i8
  store i8 %31, i8* %c.addr2
  switch i32 %30, label %sw.end53 [ i32 102, label %sw.case54 i32 70, label %sw.case55 i32 101, label %sw.case56 i32 69, label %sw.case57 i32 100, label %sw.case58 i32 68, label %sw.case59 i32 99, label %sw.case60 i32 67, label %sw.case61 i32 98, label %sw.case62 i32 66, label %sw.case63 i32 97, label %sw.case64 i32 65, label %sw.case65 i32 57, label %sw.case66 i32 56, label %sw.case67 i32 55, label %sw.case68 i32 54, label %sw.case69 i32 53, label %sw.case70 i32 52, label %sw.case71 i32 51, label %sw.case72 i32 50, label %sw.case73 i32 49, label %sw.case74 i32 48, label %sw.case75 i32 -1, label %sw.case76 ]
sw.after77:
  br label %sw.end53
sw.case54:
  br label %sw.case55
sw.case55:
  %32 = load i16, i16* %hex.addr21
  %33 = add i16 %32, 1
  store i16 %33, i16* %hex.addr21
  br label %sw.case56
sw.case56:
  br label %sw.case57
sw.case57:
  %34 = load i16, i16* %hex.addr21
  %35 = add i16 %34, 1
  store i16 %35, i16* %hex.addr21
  br label %sw.case58
sw.case58:
  br label %sw.case59
sw.case59:
  %36 = load i16, i16* %hex.addr21
  %37 = add i16 %36, 1
  store i16 %37, i16* %hex.addr21
  br label %sw.case60
sw.case60:
  br label %sw.case61
sw.case61:
  %38 = load i16, i16* %hex.addr21
  %39 = add i16 %38, 1
  store i16 %39, i16* %hex.addr21
  br label %sw.case62
sw.case62:
  br label %sw.case63
sw.case63:
  %40 = load i16, i16* %hex.addr21
  %41 = add i16 %40, 1
  store i16 %41, i16* %hex.addr21
  br label %sw.case64
sw.case64:
  br label %sw.case65
sw.case65:
  %42 = load i16, i16* %hex.addr21
  %43 = add i16 %42, 1
  store i16 %43, i16* %hex.addr21
  br label %sw.case66
sw.case66:
  %44 = load i16, i16* %hex.addr21
  %45 = add i16 %44, 1
  store i16 %45, i16* %hex.addr21
  br label %sw.case67
sw.case67:
  %46 = load i16, i16* %hex.addr21
  %47 = add i16 %46, 1
  store i16 %47, i16* %hex.addr21
  br label %sw.case68
sw.case68:
  %48 = load i16, i16* %hex.addr21
  %49 = add i16 %48, 1
  store i16 %49, i16* %hex.addr21
  br label %sw.case69
sw.case69:
  %50 = load i16, i16* %hex.addr21
  %51 = add i16 %50, 1
  store i16 %51, i16* %hex.addr21
  br label %sw.case70
sw.case70:
  %52 = load i16, i16* %hex.addr21
  %53 = add i16 %52, 1
  store i16 %53, i16* %hex.addr21
  br label %sw.case71
sw.case71:
  %54 = load i16, i16* %hex.addr21
  %55 = add i16 %54, 1
  store i16 %55, i16* %hex.addr21
  br label %sw.case72
sw.case72:
  %56 = load i16, i16* %hex.addr21
  %57 = add i16 %56, 1
  store i16 %57, i16* %hex.addr21
  br label %sw.case73
sw.case73:
  %58 = load i16, i16* %hex.addr21
  %59 = add i16 %58, 1
  store i16 %59, i16* %hex.addr21
  br label %sw.case74
sw.case74:
  %60 = load i16, i16* %hex.addr21
  %61 = add i16 %60, 1
  store i16 %61, i16* %hex.addr21
  br label %sw.case75
sw.case75:
  br label %sw.end53
break.dead78:
  br label %sw.case76
sw.case76:
  br label %label.error79
goto.dead80:
  br label %sw.end53
sw.end53:
  br label %for.inc51
for.inc51:
  %62 = load i32, i32* %i.addr20
  %63 = add i32 %62, 1
  store i32 %63, i32* %i.addr20
  br label %for.cond49
for.end52:
  %64 = load i8*, i8** %str.addr0
  %65 = load i16, i16* %hex.addr21
  %66 = call i8* @add_unicode(i8* %64, i32* %capacity.addr3, i16 %65)
  store i8* %66, i8** %str.addr0
  br label %sw.end31
break.dead81:
  br label %sw.case38
sw.case38:
  br label %sw.case39
sw.case39:
  br label %sw.case40
sw.case40:
  %67 = load i8*, i8** %str.addr0
  %68 = load i8, i8* %c.addr2
  %69 = call i8* @add_char(i8* %67, i32* %capacity.addr3, i8 %68)
  store i8* %69, i8** %str.addr0
  br label %sw.end31
break.dead82:
  br label %sw.default41
sw.default41:
  br label %sw.case42
sw.case42:
  br label %label.error79
goto.dead83:
  br label %sw.end31
sw.end31:
  br label %sw.end24
break.dead84:
  br label %sw.default27
sw.default27:
  %70 = load i8*, i8** %str.addr0
  %71 = load i8, i8* %c.addr2
  %72 = call i8* @add_char(i8* %70, i32* %capacity.addr3, i8 %71)
  store i8* %72, i8** %str.addr0
  br label %sw.end24
break.dead85:
  br label %sw.case28
sw.case28:
  br label %label.error79
goto.dead86:
  br label %sw.end24
sw.end24:
  br label %while.cond21
while.end23:
  br label %label.error79
label.error79:
  %73 = load i8*, i8** %str.addr0
  call void @free(i8* %73)
  %74 = inttoptr i32 0 to i8*
  store i8* %74, i8** %retval
  br label %return20
return.dead87:
  br label %return20
return20:
  %75 = load i8*, i8** %retval
  ret i8* %75
}

define dso_local i32 @main() {
entry:
  %retval = alloca i32
  store i32 0, i32* %retval
  %s.addr0 = alloca i8*
  %1 = call i32 @getchar()
  switch i32 %1, label %sw.default91 [ i32 34, label %sw.case90 ]
sw.after92:
  br label %sw.end89
sw.case90:
  %2 = call i8* @parse_str()
  store i8* %2, i8** %s.addr0
  %3 = load i8*, i8** %s.addr0
  %4 = icmp eq i8* %3, null
  %5 = zext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %if.then93, label %if.end94
if.then93:
  store i32 1, i32* %retval
  br label %return88
return.dead95:
  br label %if.end94
if.end94:
  %7 = load i8*, i8** %s.addr0
  %8 = call i32 @puts(i8* %7)
  %9 = load i8*, i8** %s.addr0
  call void @free(i8* %9)
  store i32 0, i32* %retval
  br label %return88
return.dead96:
  br label %sw.default91
sw.default91:
  store i32 1, i32* %retval
  br label %return88
return.dead97:
  br label %sw.end89
sw.end89:
  br label %return88
return88:
  %10 = load i32, i32* %retval
  ret i32 %10
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

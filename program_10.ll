; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@size_t = dso_local global i32 0
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
  %17 = load i32*, i32** %capacity.addr
  %18 = load i32, i32* %size.addr14
  %19 = load i32, i32* %17
  %20 = icmp sge i32 %18, %19
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
return0:
  %43 = load i8*, i8** %retval
  ret i8* %43
}

define dso_local i8* @add_unicode(i8* %0, i32* %1, i16 %2) {
entry:
if.then7:
  %9 = load i8*, i8** %str.addr
  %10 = load i32*, i32** %capacity.addr
  %11 = load i16, i16* %unicode.addr
  %12 = call i8* @add_char(i8* %9, i32* %10, i16 %11)
  store i8* %12, i8** %retval
  br label %return6
if.end8:
  store i32 1, i32* %n.addr3
  br label %for.cond9
for.cond9:
  %13 = load i32, i32* %n.addr3
  %14 = mul i32 6, %13
  %15 = load i32, i32* %n.addr3
  %16 = sub i32 %14, %15
  %17 = add i32 %16, 1
  %18 = load i16, i16* %unicode.addr
  %19 = sext i16 %18 to i32
  %20 = ashr i32 %19, %17
  %21 = icmp ne i32 %20, 0
  br i1 %21, label %for.body10, label %for.end12
for.body10:
  br label %for.inc11
for.inc11:
  %22 = load i32, i32* %n.addr3
  %23 = add i32 %22, 1
  store i32 %23, i32* %n.addr3
  br label %for.cond9
for.end12:
  %24 = load i8*, i8** %str.addr
  %25 = load i32*, i32** %capacity.addr
  %26 = load i32, i32* %n.addr3
  %27 = sub i32 8, %26
  %28 = shl i32 0, %27
  %29 = load i32, i32* %n.addr3
  %30 = sub i32 %29, 1
  %31 = mul i32 6, %30
  %32 = load i16, i16* %unicode.addr
  %33 = sext i16 %32 to i32
  %34 = ashr i32 %33, %31
  %35 = or i32 %28, %34
  %36 = call i8* @add_char(i8* %24, i32* %25, i32 %35)
  store i8* %36, i8** %str.addr
  %37 = load i32, i32* %n.addr3
  %38 = sub i32 %37, 1
  store i32 %38, i32* %n.addr3
  br label %for.cond13
for.cond13:
  %39 = load i32, i32* %n.addr3
  %40 = icmp sgt i32 %39, 0
  %41 = zext i1 %40 to i32
  %42 = icmp ne i32 %41, 0
  br i1 %42, label %for.body14, label %for.end16
for.body14:
  %43 = load i8*, i8** %str.addr
  %44 = load i32*, i32** %capacity.addr
  %45 = load i32, i32* %n.addr3
  %46 = sub i32 %45, 1
  %47 = mul i32 6, %46
  %48 = load i16, i16* %unicode.addr
  %49 = sext i16 %48 to i32
  %50 = ashr i32 %49, %47
  %51 = ashr i32 0, 2
  %52 = and i32 %50, %51
  %53 = shl i32 0, 7
  %54 = or i32 %52, %53
  %55 = call i8* @add_char(i8* %43, i32* %44, i32 %54)
  store i8* %55, i8** %str.addr
  br label %for.inc15
for.inc15:
  %56 = load i32, i32* %n.addr3
  %57 = sub i32 %56, 1
  store i32 %57, i32* %n.addr3
  br label %for.cond13
for.end16:
  %58 = load i8*, i8** %str.addr
  store i8* %58, i8** %retval
  br label %return6
return6:
  %59 = load i8*, i8** %retval
  ret i8* %59
}

define dso_local i8* @parse_str() {
entry:
while.cond18:
  %4 = icmp ne i32 1, 0
  br i1 %4, label %while.body19, label %while.end20
while.body19:
  %5 = call i32 @getchar()
  store i32 %5, i8* %c.addr2
  switch i32 %5, label %sw.default21 [
    i32 34, label %sw.case23
    i32 92, label %sw.case24
    i32 -1, label %sw.case25
  ]
sw.case23:
  %6 = load i8*, i8** %str.addr0
  store i8* %6, i8** %retval
  br label %return17
sw.case24:
  %7 = call i32 @getchar()
  store i32 %7, i8* %c.addr2
  switch i32 %7, label %sw.default26 [
    i32 98, label %sw.case28
    i32 102, label %sw.case29
    i32 110, label %sw.case30
    i32 114, label %sw.case31
    i32 116, label %sw.case32
    i32 117, label %sw.case33
    i32 47, label %sw.case34
    i32 92, label %sw.case34
    i32 34, label %sw.case34
    i32 -1, label %sw.case35
  ]
sw.case28:
  %8 = load i8*, i8** %str.addr0
  %9 = call i8* @add_char(i8* %8, i32* %capacity.addr3, i8 8)
  store i8* %9, i8** %str.addr0
  br label %case.next36
case.next36:
  br label %sw.end27
sw.case29:
  %10 = load i8*, i8** %str.addr0
  %11 = call i8* @add_char(i8* %10, i32* %capacity.addr3, i8 12)
  store i8* %11, i8** %str.addr0
  br label %case.next37
case.next37:
  br label %sw.end27
sw.case30:
  %12 = load i8*, i8** %str.addr0
  %13 = call i8* @add_char(i8* %12, i32* %capacity.addr3, i8 10)
  store i8* %13, i8** %str.addr0
  br label %case.next38
case.next38:
  br label %sw.end27
sw.case31:
  %14 = load i8*, i8** %str.addr0
  %15 = call i8* @add_char(i8* %14, i32* %capacity.addr3, i8 13)
  store i8* %15, i8** %str.addr0
  br label %case.next39
case.next39:
  br label %sw.end27
sw.case32:
  %16 = load i8*, i8** %str.addr0
  %17 = call i8* @add_char(i8* %16, i32* %capacity.addr3, i8 9)
  store i8* %17, i8** %str.addr0
  br label %case.next40
case.next40:
  br label %sw.end27
sw.case33:
  %i.addr18 = alloca i32
  %hex.addr19 = alloca i16
  %20 = trunc i32 0 to i16
  store i16 %20, i16* %hex.addr19
  store i32 0, i32* %i.addr18
  br label %for.cond41
for.cond41:
  %21 = load i32, i32* %i.addr18
  %22 = icmp slt i32 %21, 4
  %23 = zext i1 %22 to i32
  %24 = icmp ne i32 %23, 0
  br i1 %24, label %for.body42, label %for.end44
for.body42:
  %25 = load i16, i16* %hex.addr19
  %26 = mul i16 %25, 0
  store i16 %26, i16* %hex.addr19
  %27 = call i32 @getchar()
  store i32 %27, i8* %c.addr2
  switch i32 %27, label %sw.default45 [
    i32 70, label %sw.case47
    i32 102, label %sw.case47
    i32 69, label %sw.case48
    i32 101, label %sw.case48
    i32 68, label %sw.case49
    i32 100, label %sw.case49
    i32 67, label %sw.case50
    i32 99, label %sw.case50
    i32 66, label %sw.case51
    i32 98, label %sw.case51
    i32 65, label %sw.case52
    i32 97, label %sw.case52
    i32 57, label %sw.case53
    i32 56, label %sw.case54
    i32 55, label %sw.case55
    i32 54, label %sw.case56
    i32 53, label %sw.case57
    i32 52, label %sw.case58
    i32 51, label %sw.case59
    i32 50, label %sw.case60
    i32 49, label %sw.case61
    i32 48, label %sw.case62
    i32 -1, label %sw.case63
  ]
sw.case47:
  %28 = load i16, i16* %hex.addr19
  %29 = add i16 %28, 1
  store i16 %29, i16* %hex.addr19
  br label %case.next64
case.next64:
  br label %case.next65
case.next65:
  br label %sw.case48
sw.case48:
  %30 = load i16, i16* %hex.addr19
  %31 = add i16 %30, 1
  store i16 %31, i16* %hex.addr19
  br label %case.next66
case.next66:
  br label %case.next67
case.next67:
  br label %sw.case49
sw.case49:
  %32 = load i16, i16* %hex.addr19
  %33 = add i16 %32, 1
  store i16 %33, i16* %hex.addr19
  br label %case.next68
case.next68:
  br label %case.next69
case.next69:
  br label %sw.case50
sw.case50:
  %34 = load i16, i16* %hex.addr19
  %35 = add i16 %34, 1
  store i16 %35, i16* %hex.addr19
  br label %case.next70
case.next70:
  br label %case.next71
case.next71:
  br label %sw.case51
sw.case51:
  %36 = load i16, i16* %hex.addr19
  %37 = add i16 %36, 1
  store i16 %37, i16* %hex.addr19
  br label %case.next72
case.next72:
  br label %case.next73
case.next73:
  br label %sw.case52
sw.case52:
  %38 = load i16, i16* %hex.addr19
  %39 = add i16 %38, 1
  store i16 %39, i16* %hex.addr19
  br label %case.next74
case.next74:
  br label %case.next75
case.next75:
  br label %sw.case53
sw.case53:
  %40 = load i16, i16* %hex.addr19
  %41 = add i16 %40, 1
  store i16 %41, i16* %hex.addr19
  br label %case.next76
case.next76:
  br label %sw.case54
sw.case54:
  %42 = load i16, i16* %hex.addr19
  %43 = add i16 %42, 1
  store i16 %43, i16* %hex.addr19
  br label %case.next77
case.next77:
  br label %sw.case55
sw.case55:
  %44 = load i16, i16* %hex.addr19
  %45 = add i16 %44, 1
  store i16 %45, i16* %hex.addr19
  br label %case.next78
case.next78:
  br label %sw.case56
sw.case56:
  %46 = load i16, i16* %hex.addr19
  %47 = add i16 %46, 1
  store i16 %47, i16* %hex.addr19
  br label %case.next79
case.next79:
  br label %sw.case57
sw.case57:
  %48 = load i16, i16* %hex.addr19
  %49 = add i16 %48, 1
  store i16 %49, i16* %hex.addr19
  br label %case.next80
case.next80:
  br label %sw.case58
sw.case58:
  %50 = load i16, i16* %hex.addr19
  %51 = add i16 %50, 1
  store i16 %51, i16* %hex.addr19
  br label %case.next81
case.next81:
  br label %sw.case59
sw.case59:
  %52 = load i16, i16* %hex.addr19
  %53 = add i16 %52, 1
  store i16 %53, i16* %hex.addr19
  br label %case.next82
case.next82:
  br label %sw.case60
sw.case60:
  %54 = load i16, i16* %hex.addr19
  %55 = add i16 %54, 1
  store i16 %55, i16* %hex.addr19
  br label %case.next83
case.next83:
  br label %sw.case61
sw.case61:
  %56 = load i16, i16* %hex.addr19
  %57 = add i16 %56, 1
  store i16 %57, i16* %hex.addr19
  br label %case.next84
case.next84:
  br label %sw.case62
sw.case62:
  br label %sw.end46
sw.case63:
  br label %label.error85
sw.default45:
  br label %sw.end46
sw.end46:
  br label %for.inc43
for.inc43:
  %58 = load i32, i32* %i.addr18
  %59 = add i32 %58, 1
  store i32 %59, i32* %i.addr18
  br label %for.cond41
for.end44:
  %60 = load i8*, i8** %str.addr0
  %61 = load i16, i16* %hex.addr19
  %62 = call i8* @add_unicode(i8* %60, i32* %capacity.addr3, i16 %61)
  store i8* %62, i8** %str.addr0
  br label %sw.end27
sw.case34:
  %63 = load i8*, i8** %str.addr0
  %64 = load i8, i8* %c.addr2
  %65 = call i8* @add_char(i8* %63, i32* %capacity.addr3, i8 %64)
  store i8* %65, i8** %str.addr0
  br label %case.next86
case.next86:
  br label %case.next87
case.next87:
  br label %case.next88
case.next88:
  br label %sw.end27
sw.case35:
  br label %label.error85
sw.default26:
  br label %sw.end27
sw.end27:
  br label %case.next89
case.next89:
  br label %sw.end22
sw.case25:
  br label %label.error85
sw.default21:
  br label %sw.end22
sw.end22:
  br label %while.cond18
while.end20:
  br label %label.error85
label.error85:
  %69 = load i8*, i8** %str.addr0
  call void @free(i8* %69)
  %70 = inttoptr i32 0 to i8*
  store i8* %70, i8** %retval
  br label %return17
return17:
  %71 = load i8*, i8** %retval
  ret i8* %71
}

define dso_local i32 @main() {
entry:
sw.case93:
  %2 = call i8* @parse_str()
  store i8* %2, i8** %s.addr0
  %3 = load i8*, i8** %s.addr0
  %4 = icmp eq i8* %3, null
  %5 = zext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %if.then94, label %if.end95
if.then94:
  store i32 1, i32* %retval
  br label %return90
if.end95:
  %7 = load i8*, i8** %s.addr0
  %8 = call i32 @puts(i8* %7)
  %9 = load i8*, i8** %s.addr0
  call void @free(i8* %9)
  store i32 0, i32* %retval
  br label %return90
sw.default91:
  br label %sw.end92
sw.end92:
  br label %return90
return90:
  %10 = load i32, i32* %retval
  ret i32 %10
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

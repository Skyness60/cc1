; ModuleID = 'cc1'
source_filename = "cc1"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@static_test_signed_int_operations.val.0 = internal global i32 12
@static_test_signed_int_operations.val.1 = internal global i32 18
@static_test_signed_int_operations.val.2 = internal global i32 -45
@static_test_signed_int_operations.val.3 = internal global i32 2
@static_test_signed_int_operations.val.4 = internal global i32 -5
@static_test_signed_int_operations.val.5 = internal global i32 1
@static_test_signed_int_operations.val.6 = internal global i32 7
@static_test_signed_int_operations.val.7 = internal global i32 15
@static_test_signed_int_operations.val.8 = internal global i32 8
@static_test_signed_int_operations.val.9 = internal global i32 -16
@static_test_signed_int_operations.val.10 = internal global i32 30
@static_test_signed_int_operations.val.11 = internal global i32 7
@static_test_unsigned_int_operations.val.12 = internal global i32 18
@static_test_unsigned_int_operations.val.13 = internal global i32 12
@static_test_unsigned_int_operations.val.14 = internal global i32 45
@static_test_unsigned_int_operations.val.15 = internal global i32 2
@static_test_unsigned_int_operations.val.16 = internal global i32 1
@static_test_unsigned_int_operations.val.17 = internal global i32 7
@static_test_unsigned_int_operations.val.18 = internal global i32 15
@static_test_unsigned_int_operations.val.19 = internal global i32 8
@static_test_unsigned_int_operations.val.20 = internal global i32 -16
@static_test_unsigned_int_operations.val.21 = internal global i32 30
@static_test_unsigned_int_operations.val.22 = internal global i32 7
@static_test_float_operations.val.23 = internal global float 0x402b666660000000
@static_test_float_operations.val.24 = internal global float 0x401d333340000000
@static_test_float_operations.val.25 = internal global float 0x4040ccccc0000000
@static_test_float_operations.val.26 = internal global float 0x400a400000000000
@static_test_float_operations.val.27 = internal global float 0x4020333340000000
@static_test_float_operations.val.28 = internal global float 0x4029ccccc0000000
@static_test_float_operations.val.29 = internal global float 0xc039333340000000
@static_test_float_operations.val.30 = internal global float 0xc011800000000000
@static_test_float_operations.val.31 = internal global float 0xc025000000000000
@.str.0 = private unnamed_addr constant [46 x i8] c"Signed Integer Arithmetic and Bitwise Tests:\0A\00"
@.str.1 = private unnamed_addr constant [31 x i8] c"Addition:        %d + %d = %d\0A\00"
@.str.2 = private unnamed_addr constant [31 x i8] c"Subtraction:     %d - %d = %d\0A\00"
@.str.3 = private unnamed_addr constant [32 x i8] c"Multiplication:  %d * %d = %dd\0A\00"
@.str.4 = private unnamed_addr constant [31 x i8] c"Division:        %d / %d = %d\0A\00"
@.str.5 = private unnamed_addr constant [31 x i8] c"Division:        %d / %d = %d\0A\00"
@.str.6 = private unnamed_addr constant [30 x i8] c"Modulo:        %d %% %d = %d\0A\00"
@.str.7 = private unnamed_addr constant [31 x i8] c"Bitwise AND:     %d & %d = %d\0A\00"
@.str.8 = private unnamed_addr constant [31 x i8] c"Bitwise OR:      %d | %d = %d\0A\00"
@.str.9 = private unnamed_addr constant [31 x i8] c"Bitwise XOR:     %d ^ %d = %d\0A\00"
@.str.10 = private unnamed_addr constant [27 x i8] c"Bitwise NOT:     ~%d = %d\0A\00"
@.str.11 = private unnamed_addr constant [31 x i8] c"Shift Left:      %d << 1 = %d\0A\00"
@.str.12 = private unnamed_addr constant [31 x i8] c"Shift Right:     %d >> 1 = %d\0A\00"
@.str.13 = private unnamed_addr constant [48 x i8] c"Unsigned Integer Arithmetic and Bitwise Tests:\0A\00"
@.str.14 = private unnamed_addr constant [31 x i8] c"Addition:        %u + %u = %u\0A\00"
@.str.15 = private unnamed_addr constant [31 x i8] c"Subtraction:     %u - %u = %u\0A\00"
@.str.16 = private unnamed_addr constant [31 x i8] c"Multiplication:  %u * %u = %u\0A\00"
@.str.17 = private unnamed_addr constant [31 x i8] c"Division:        %u / %u = %u\0A\00"
@.str.18 = private unnamed_addr constant [32 x i8] c"Modulo:          %u %% %u = %u\0A\00"
@.str.19 = private unnamed_addr constant [31 x i8] c"Bitwise AND:     %u & %u = %u\0A\00"
@.str.20 = private unnamed_addr constant [31 x i8] c"Bitwise OR:      %u | %u = %u\0A\00"
@.str.21 = private unnamed_addr constant [31 x i8] c"Bitwise XOR:     %u ^ %u = %u\0A\00"
@.str.22 = private unnamed_addr constant [27 x i8] c"Bitwise NOT:     ~%u = %u\0A\00"
@.str.23 = private unnamed_addr constant [31 x i8] c"Shift Left:      %u << 1 = %u\0A\00"
@.str.24 = private unnamed_addr constant [31 x i8] c"Shift Right:     %u >> 1 = %u\0A\00"
@.str.25 = private unnamed_addr constant [63 x i8] c"Testing Logical Operators (&&, ||) and Ternary Operator (?:)\0A\0A\00"
@.str.26 = private unnamed_addr constant [27 x i8] c"Testing Logical AND (&&):\0A\00"
@.str.27 = private unnamed_addr constant [37 x i8] c"a && b: %d && %d = %d (Expected: 0)\0A\00"
@.str.28 = private unnamed_addr constant [36 x i8] c"a && 1: %d && 1 = %d (Expected: 1)\0A\00"
@.str.29 = private unnamed_addr constant [37 x i8] c"b && c: %d && %d = %d (Expected: 0)\0A\00"
@.str.30 = private unnamed_addr constant [37 x i8] c"c && d: %d && %d = %d (Expected: 1)\0A\00"
@.str.31 = private unnamed_addr constant [37 x i8] c"0 && 0: %d && %d = %d (Expected: 0)\0A\00"
@.str.32 = private unnamed_addr constant [37 x i8] c"1 && 0: %d && %d = %d (Expected: 0)\0A\00"
@.str.33 = private unnamed_addr constant [37 x i8] c"1 && 1: %d && %d = %d (Expected: 1)\0A\00"
@.str.34 = private unnamed_addr constant [27 x i8] c"\0ATesting Logical OR (||):\0A\00"
@.str.35 = private unnamed_addr constant [37 x i8] c"a || b: %d || %d = %d (Expected: 1)\0A\00"
@.str.36 = private unnamed_addr constant [36 x i8] c"b || 0: %d || 0 = %d (Expected: 0)\0A\00"
@.str.37 = private unnamed_addr constant [37 x i8] c"0 || c: %d || %d = %d (Expected: 1)\0A\00"
@.str.38 = private unnamed_addr constant [37 x i8] c"c || d: %d || %d = %d (Expected: 1)\0A\00"
@.str.39 = private unnamed_addr constant [37 x i8] c"0 || 0: %d || %d = %d (Expected: 0)\0A\00"
@.str.40 = private unnamed_addr constant [37 x i8] c"1 || 0: %d || %d = %d (Expected: 1)\0A\00"
@.str.41 = private unnamed_addr constant [37 x i8] c"1 || 1: %d || %d = %d (Expected: 1)\0A\00"
@.str.42 = private unnamed_addr constant [33 x i8] c"\0ATesting Ternary Operator (?:):\0A\00"
@.str.43 = private unnamed_addr constant [45 x i8] c"a ? c : d = %d ? %d : %d = %d (Expected: 5)\0A\00"
@.str.44 = private unnamed_addr constant [46 x i8] c"b ? c : d = %d ? %d : %d = %d (Expected: 10)\0A\00"
@.str.45 = private unnamed_addr constant [55 x i8] c"c > d ? c : d = %d > %d ? %d : %d = %d (Expected: 10)\0A\00"
@.str.46 = private unnamed_addr constant [54 x i8] c"c < d ? c : d = %d < %d ? %d : %d = %d (Expected: 5)\0A\00"
@.str.47 = private unnamed_addr constant [57 x i8] c"c == d ? c : d = %d == %d ? %d : %d = %d (Expected: 10)\0A\00"
@.str.48 = private unnamed_addr constant [26 x i8] c"Float Arithmetic Tests:\0A\0A\00"
@.str.49 = private unnamed_addr constant [41 x i8] c"a + b:               %.2f + %.2f = %.2f\0A\00"
@.str.50 = private unnamed_addr constant [41 x i8] c"a - b:               %.2f - %.2f = %.2f\0A\00"
@.str.51 = private unnamed_addr constant [41 x i8] c"a * b:               %.2f * %.2f = %.2f\0A\00"
@.str.52 = private unnamed_addr constant [41 x i8] c"a / b:               %.2f / %.2f = %.2f\0A\00"
@.str.53 = private unnamed_addr constant [41 x i8] c"a + c:               %.2f + %.2f = %.2f\0A\00"
@.str.54 = private unnamed_addr constant [41 x i8] c"a - c:               %.2f - %.2f = %.2f\0A\00"
@.str.55 = private unnamed_addr constant [41 x i8] c"a * c:               %.2f * %.2f = %.2f\0A\00"
@.str.56 = private unnamed_addr constant [41 x i8] c"a / c:               %.2f / %.2f = %.2f\0A\00"
@.str.57 = private unnamed_addr constant [35 x i8] c"-a:                  -%.2f = %.2f\0A\00"
@.str.58 = private unnamed_addr constant [29 x i8] c"Operator Precedence Tests:\0A\0A\00"
@.str.59 = private unnamed_addr constant [43 x i8] c"a + b * c:              %d + %d * %d = %d\0A\00"
@.str.60 = private unnamed_addr constant [43 x i8] c"a - b / c:              %d - %d / %d = %d\0A\00"
@.str.61 = private unnamed_addr constant [45 x i8] c"(a + b) * c:            (%d + %d) * %d = %d\0A\00"
@.str.62 = private unnamed_addr constant [43 x i8] c"a + b & c:              %d + %d & %d = %d\0A\00"
@.str.63 = private unnamed_addr constant [43 x i8] c"a | b * c:              %d | %d * %d = %d\0A\00"
@.str.64 = private unnamed_addr constant [45 x i8] c"(a | b) * c:            (%d | %d) * %d = %d\0A\00"
@.str.65 = private unnamed_addr constant [48 x i8] c"a << 1 + b >> 1:        %d << 1 + %d >> 1 = %d\0A\00"
@.str.66 = private unnamed_addr constant [52 x i8] c"(a << 1) + (b >> 1):    (%d << 1) + (%d >> 1) = %d\0A\00"
@.str.67 = private unnamed_addr constant [49 x i8] c"a < b || b & c:         %d < %d || %d & %d = %d\0A\00"
@.str.68 = private unnamed_addr constant [50 x i8] c"a < b || b && c:        %d < %d || %d && %d = %d\0A\00"
@.str.69 = private unnamed_addr constant [50 x i8] c"a + (b * c - d):        %d + (%d * %d - %d) = %d\0A\00"
@.str.70 = private unnamed_addr constant [52 x i8] c"(a + b) * (c + d):      (%d + %d) * (%d + %d) = %d\0A\00"
@.str.71 = private unnamed_addr constant [35 x i8] c"\0AFloat Operator Precedence Tests:\0A\00"
@.str.72 = private unnamed_addr constant [51 x i8] c"x + y * z:              %.2f + %.2f * %.2f = %.2f\0A\00"
@.str.73 = private unnamed_addr constant [51 x i8] c"x - y / z:              %.2f - %.2f / %.2f = %.2f\0A\00"
@.str.74 = private unnamed_addr constant [53 x i8] c"(x + y) * z:            (%.2f + %.2f) * %.2f = %.2f\0A\00"
@.str.75 = private unnamed_addr constant [35 x i8] c"\0AEdge Cases with Negative Values:\0A\00"
@.str.76 = private unnamed_addr constant [43 x i8] c"d * b / c:              %d * %d / %d = %d\0A\00"
@.str.77 = private unnamed_addr constant [44 x i8] c"d + a * -b:             %d + %d * -%d = %d\0A\00"
@.str.78 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.79 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.80 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.81 = private unnamed_addr constant [46 x i8] c"Signed Integer Arithmetic and Bitwise Tests:\0A\00"
@.str.82 = private unnamed_addr constant [31 x i8] c"Addition:        %d + %d = %d\0A\00"
@.str.83 = private unnamed_addr constant [31 x i8] c"Subtraction:     %d - %d = %d\0A\00"
@.str.84 = private unnamed_addr constant [32 x i8] c"Multiplication:  %d * %d = %dd\0A\00"
@.str.85 = private unnamed_addr constant [31 x i8] c"Division:        %d / %d = %d\0A\00"
@.str.86 = private unnamed_addr constant [31 x i8] c"Division:        %d / %d = %d\0A\00"
@.str.87 = private unnamed_addr constant [30 x i8] c"Modulo:        %d %% %d = %d\0A\00"
@.str.88 = private unnamed_addr constant [31 x i8] c"Bitwise AND:     %d & %d = %d\0A\00"
@.str.89 = private unnamed_addr constant [31 x i8] c"Bitwise OR:      %d | %d = %d\0A\00"
@.str.90 = private unnamed_addr constant [31 x i8] c"Bitwise XOR:     %d ^ %d = %d\0A\00"
@.str.91 = private unnamed_addr constant [27 x i8] c"Bitwise NOT:     ~%d = %d\0A\00"
@.str.92 = private unnamed_addr constant [31 x i8] c"Shift Left:      %d << 1 = %d\0A\00"
@.str.93 = private unnamed_addr constant [31 x i8] c"Shift Right:     %d >> 1 = %d\0A\00"
@.str.94 = private unnamed_addr constant [48 x i8] c"Unsigned Integer Arithmetic and Bitwise Tests:\0A\00"
@.str.95 = private unnamed_addr constant [31 x i8] c"Addition:        %u + %u = %u\0A\00"
@.str.96 = private unnamed_addr constant [31 x i8] c"Subtraction:     %u - %u = %u\0A\00"
@.str.97 = private unnamed_addr constant [31 x i8] c"Multiplication:  %u * %u = %u\0A\00"
@.str.98 = private unnamed_addr constant [31 x i8] c"Division:        %u / %u = %u\0A\00"
@.str.99 = private unnamed_addr constant [32 x i8] c"Modulo:          %u %% %u = %u\0A\00"
@.str.100 = private unnamed_addr constant [31 x i8] c"Bitwise AND:     %u & %u = %u\0A\00"
@.str.101 = private unnamed_addr constant [31 x i8] c"Bitwise OR:      %u | %u = %u\0A\00"
@.str.102 = private unnamed_addr constant [31 x i8] c"Bitwise XOR:     %u ^ %u = %u\0A\00"
@.str.103 = private unnamed_addr constant [27 x i8] c"Bitwise NOT:     ~%u = %u\0A\00"
@.str.104 = private unnamed_addr constant [31 x i8] c"Shift Left:      %u << 1 = %u\0A\00"
@.str.105 = private unnamed_addr constant [31 x i8] c"Shift Right:     %u >> 1 = %u\0A\00"
@.str.106 = private unnamed_addr constant [63 x i8] c"Testing Logical Operators (&&, ||) and Ternary Operator (?:)\0A\0A\00"
@.str.107 = private unnamed_addr constant [27 x i8] c"Testing Logical AND (&&):\0A\00"
@.str.108 = private unnamed_addr constant [37 x i8] c"a && b: %d && %d = %d (Expected: 0)\0A\00"
@.str.109 = private unnamed_addr constant [36 x i8] c"a && 1: %d && 1 = %d (Expected: 1)\0A\00"
@.str.110 = private unnamed_addr constant [37 x i8] c"b && c: %d && %d = %d (Expected: 0)\0A\00"
@.str.111 = private unnamed_addr constant [37 x i8] c"c && d: %d && %d = %d (Expected: 1)\0A\00"
@.str.112 = private unnamed_addr constant [37 x i8] c"0 && 0: %d && %d = %d (Expected: 0)\0A\00"
@.str.113 = private unnamed_addr constant [37 x i8] c"1 && 0: %d && %d = %d (Expected: 0)\0A\00"
@.str.114 = private unnamed_addr constant [37 x i8] c"1 && 1: %d && %d = %d (Expected: 1)\0A\00"
@.str.115 = private unnamed_addr constant [86 x i8] c"Short-circuit test (b && ++c): %d && ++%d = %d (Expected: 0, c should not increment)\0A\00"
@.str.116 = private unnamed_addr constant [55 x i8] c"Value of c after short-circuit test: %d (Expected: 5)\0A\00"
@.str.117 = private unnamed_addr constant [27 x i8] c"\0ATesting Logical OR (||):\0A\00"
@.str.118 = private unnamed_addr constant [37 x i8] c"a || b: %d || %d = %d (Expected: 1)\0A\00"
@.str.119 = private unnamed_addr constant [36 x i8] c"b || 0: %d || 0 = %d (Expected: 0)\0A\00"
@.str.120 = private unnamed_addr constant [37 x i8] c"0 || c: %d || %d = %d (Expected: 1)\0A\00"
@.str.121 = private unnamed_addr constant [37 x i8] c"c || d: %d || %d = %d (Expected: 1)\0A\00"
@.str.122 = private unnamed_addr constant [37 x i8] c"0 || 0: %d || %d = %d (Expected: 0)\0A\00"
@.str.123 = private unnamed_addr constant [37 x i8] c"1 || 0: %d || %d = %d (Expected: 1)\0A\00"
@.str.124 = private unnamed_addr constant [37 x i8] c"1 || 1: %d || %d = %d (Expected: 1)\0A\00"
@.str.125 = private unnamed_addr constant [86 x i8] c"Short-circuit test (a || ++d): %d || ++%d = %d (Expected: 1, d should not increment)\0A\00"
@.str.126 = private unnamed_addr constant [56 x i8] c"Value of d after short-circuit test: %d (Expected: 10)\0A\00"
@.str.127 = private unnamed_addr constant [33 x i8] c"\0ATesting Ternary Operator (?:):\0A\00"
@.str.128 = private unnamed_addr constant [45 x i8] c"a ? c : d = %d ? %d : %d = %d (Expected: 5)\0A\00"
@.str.129 = private unnamed_addr constant [46 x i8] c"b ? c : d = %d ? %d : %d = %d (Expected: 10)\0A\00"
@.str.130 = private unnamed_addr constant [55 x i8] c"c > d ? c : d = %d > %d ? %d : %d = %d (Expected: 10)\0A\00"
@.str.131 = private unnamed_addr constant [54 x i8] c"c < d ? c : d = %d < %d ? %d : %d = %d (Expected: 5)\0A\00"
@.str.132 = private unnamed_addr constant [57 x i8] c"c == d ? c : d = %d == %d ? %d : %d = %d (Expected: 10)\0A\00"
@.str.133 = private unnamed_addr constant [41 x i8] c"Short-circuit test in ternary operator:\0A\00"
@.str.134 = private unnamed_addr constant [77 x i8] c"result = a ? (c += 2) : d; with a = %d, c = %d => result = %d (Expected: 7)\0A\00"
@.str.135 = private unnamed_addr constant [44 x i8] c"Value of c after ternary: %d (Expected: 7)\0A\00"
@.str.136 = private unnamed_addr constant [78 x i8] c"result = b ? c : (d += 3); with b = %d, d = %d => result = %d (Expected: 13)\0A\00"
@.str.137 = private unnamed_addr constant [45 x i8] c"Value of d after ternary: %d (Expected: 13)\0A\00"
@.str.138 = private unnamed_addr constant [26 x i8] c"Float Arithmetic Tests:\0A\0A\00"
@.str.139 = private unnamed_addr constant [41 x i8] c"a + b:               %.2f + %.2f = %.2f\0A\00"
@.str.140 = private unnamed_addr constant [41 x i8] c"a - b:               %.2f - %.2f = %.2f\0A\00"
@.str.141 = private unnamed_addr constant [41 x i8] c"a * b:               %.2f * %.2f = %.2f\0A\00"
@.str.142 = private unnamed_addr constant [41 x i8] c"a / b:               %.2f / %.2f = %.2f\0A\00"
@.str.143 = private unnamed_addr constant [41 x i8] c"a + c:               %.2f + %.2f = %.2f\0A\00"
@.str.144 = private unnamed_addr constant [41 x i8] c"a - c:               %.2f - %.2f = %.2f\0A\00"
@.str.145 = private unnamed_addr constant [41 x i8] c"a * c:               %.2f * %.2f = %.2f\0A\00"
@.str.146 = private unnamed_addr constant [41 x i8] c"a / c:               %.2f / %.2f = %.2f\0A\00"
@.str.147 = private unnamed_addr constant [35 x i8] c"-a:                  -%.2f = %.2f\0A\00"
@.str.148 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.149 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.150 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.151 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.152 = private unnamed_addr constant [33 x i8] c"Test with constant expressions:\0A\00"
@.str.153 = private unnamed_addr constant [2 x i8] c"\0A\00"
declare i32 @printf(i8*, ...)

define dso_local void @static_test_signed_int_operations() {
entry:
  %0 = getelementptr inbounds [46 x i8], [46 x i8]* @.str.0, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  br label %do.body1
do.body1:
  %2 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.1, i32 0, i32 0
  %3 = sub i32 0, 3
  %4 = load i32, i32* @static_test_signed_int_operations.val.0
  %5 = call i32 @printf(i8* %2, i32 15, i32 %3, i32 %4)
  br label %do.cond2
do.cond2:
  %6 = icmp ne i32 0, 0
  br i1 %6, label %do.body1, label %do.end3
do.end3:
  br label %do.body4
do.body4:
  %7 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.2, i32 0, i32 0
  %8 = sub i32 0, 3
  %9 = load i32, i32* @static_test_signed_int_operations.val.1
  %10 = call i32 @printf(i8* %7, i32 15, i32 %8, i32 %9)
  br label %do.cond5
do.cond5:
  %11 = icmp ne i32 0, 0
  br i1 %11, label %do.body4, label %do.end6
do.end6:
  br label %do.body7
do.body7:
  %12 = getelementptr inbounds [32 x i8], [32 x i8]* @.str.3, i32 0, i32 0
  %13 = sub i32 0, 3
  %14 = load i32, i32* @static_test_signed_int_operations.val.2
  %15 = call i32 @printf(i8* %12, i32 15, i32 %13, i32 %14)
  br label %do.cond8
do.cond8:
  %16 = icmp ne i32 0, 0
  br i1 %16, label %do.body7, label %do.end9
do.end9:
  br label %do.body10
do.body10:
  %17 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.4, i32 0, i32 0
  %18 = load i32, i32* @static_test_signed_int_operations.val.3
  %19 = call i32 @printf(i8* %17, i32 15, i32 7, i32 %18)
  br label %do.cond11
do.cond11:
  %20 = icmp ne i32 0, 0
  br i1 %20, label %do.body10, label %do.end12
do.end12:
  br label %do.body13
do.body13:
  %21 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.5, i32 0, i32 0
  %22 = sub i32 0, 3
  %23 = load i32, i32* @static_test_signed_int_operations.val.4
  %24 = call i32 @printf(i8* %21, i32 15, i32 %22, i32 %23)
  br label %do.cond14
do.cond14:
  %25 = icmp ne i32 0, 0
  br i1 %25, label %do.body13, label %do.end15
do.end15:
  br label %do.body16
do.body16:
  %26 = getelementptr inbounds [30 x i8], [30 x i8]* @.str.6, i32 0, i32 0
  %27 = load i32, i32* @static_test_signed_int_operations.val.5
  %28 = call i32 @printf(i8* %26, i32 15, i32 7, i32 %27)
  br label %do.cond17
do.cond17:
  %29 = icmp ne i32 0, 0
  br i1 %29, label %do.body16, label %do.end18
do.end18:
  br label %do.body19
do.body19:
  %30 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.7, i32 0, i32 0
  %31 = load i32, i32* @static_test_signed_int_operations.val.6
  %32 = call i32 @printf(i8* %30, i32 15, i32 7, i32 %31)
  br label %do.cond20
do.cond20:
  %33 = icmp ne i32 0, 0
  br i1 %33, label %do.body19, label %do.end21
do.end21:
  br label %do.body22
do.body22:
  %34 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.8, i32 0, i32 0
  %35 = load i32, i32* @static_test_signed_int_operations.val.7
  %36 = call i32 @printf(i8* %34, i32 15, i32 7, i32 %35)
  br label %do.cond23
do.cond23:
  %37 = icmp ne i32 0, 0
  br i1 %37, label %do.body22, label %do.end24
do.end24:
  br label %do.body25
do.body25:
  %38 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.9, i32 0, i32 0
  %39 = load i32, i32* @static_test_signed_int_operations.val.8
  %40 = call i32 @printf(i8* %38, i32 15, i32 7, i32 %39)
  br label %do.cond26
do.cond26:
  %41 = icmp ne i32 0, 0
  br i1 %41, label %do.body25, label %do.end27
do.end27:
  br label %do.body28
do.body28:
  %42 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.10, i32 0, i32 0
  %43 = load i32, i32* @static_test_signed_int_operations.val.9
  %44 = call i32 @printf(i8* %42, i32 15, i32 %43)
  br label %do.cond29
do.cond29:
  %45 = icmp ne i32 0, 0
  br i1 %45, label %do.body28, label %do.end30
do.end30:
  br label %do.body31
do.body31:
  %46 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.11, i32 0, i32 0
  %47 = load i32, i32* @static_test_signed_int_operations.val.10
  %48 = call i32 @printf(i8* %46, i32 15, i32 %47)
  br label %do.cond32
do.cond32:
  %49 = icmp ne i32 0, 0
  br i1 %49, label %do.body31, label %do.end33
do.end33:
  br label %do.body34
do.body34:
  %50 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.12, i32 0, i32 0
  %51 = load i32, i32* @static_test_signed_int_operations.val.11
  %52 = call i32 @printf(i8* %50, i32 15, i32 %51)
  br label %do.cond35
do.cond35:
  %53 = icmp ne i32 0, 0
  br i1 %53, label %do.body34, label %do.end36
do.end36:
  br label %return0
return0:
  ret void
}

define dso_local void @static_test_unsigned_int_operations() {
entry:
  %0 = getelementptr inbounds [48 x i8], [48 x i8]* @.str.13, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  br label %do.body38
do.body38:
  %2 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.14, i32 0, i32 0
  %3 = load i32, i32* @static_test_unsigned_int_operations.val.12
  %4 = call i32 @printf(i8* %2, i32 15, i32 3, i32 %3)
  br label %do.cond39
do.cond39:
  %5 = icmp ne i32 0, 0
  br i1 %5, label %do.body38, label %do.end40
do.end40:
  br label %do.body41
do.body41:
  %6 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.15, i32 0, i32 0
  %7 = load i32, i32* @static_test_unsigned_int_operations.val.13
  %8 = call i32 @printf(i8* %6, i32 15, i32 3, i32 %7)
  br label %do.cond42
do.cond42:
  %9 = icmp ne i32 0, 0
  br i1 %9, label %do.body41, label %do.end43
do.end43:
  br label %do.body44
do.body44:
  %10 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.16, i32 0, i32 0
  %11 = load i32, i32* @static_test_unsigned_int_operations.val.14
  %12 = call i32 @printf(i8* %10, i32 15, i32 3, i32 %11)
  br label %do.cond45
do.cond45:
  %13 = icmp ne i32 0, 0
  br i1 %13, label %do.body44, label %do.end46
do.end46:
  br label %do.body47
do.body47:
  %14 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.17, i32 0, i32 0
  %15 = load i32, i32* @static_test_unsigned_int_operations.val.15
  %16 = call i32 @printf(i8* %14, i32 15, i32 7, i32 %15)
  br label %do.cond48
do.cond48:
  %17 = icmp ne i32 0, 0
  br i1 %17, label %do.body47, label %do.end49
do.end49:
  br label %do.body50
do.body50:
  %18 = getelementptr inbounds [32 x i8], [32 x i8]* @.str.18, i32 0, i32 0
  %19 = load i32, i32* @static_test_unsigned_int_operations.val.16
  %20 = call i32 @printf(i8* %18, i32 15, i32 7, i32 %19)
  br label %do.cond51
do.cond51:
  %21 = icmp ne i32 0, 0
  br i1 %21, label %do.body50, label %do.end52
do.end52:
  br label %do.body53
do.body53:
  %22 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.19, i32 0, i32 0
  %23 = load i32, i32* @static_test_unsigned_int_operations.val.17
  %24 = call i32 @printf(i8* %22, i32 15, i32 7, i32 %23)
  br label %do.cond54
do.cond54:
  %25 = icmp ne i32 0, 0
  br i1 %25, label %do.body53, label %do.end55
do.end55:
  br label %do.body56
do.body56:
  %26 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.20, i32 0, i32 0
  %27 = load i32, i32* @static_test_unsigned_int_operations.val.18
  %28 = call i32 @printf(i8* %26, i32 15, i32 7, i32 %27)
  br label %do.cond57
do.cond57:
  %29 = icmp ne i32 0, 0
  br i1 %29, label %do.body56, label %do.end58
do.end58:
  br label %do.body59
do.body59:
  %30 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.21, i32 0, i32 0
  %31 = load i32, i32* @static_test_unsigned_int_operations.val.19
  %32 = call i32 @printf(i8* %30, i32 15, i32 7, i32 %31)
  br label %do.cond60
do.cond60:
  %33 = icmp ne i32 0, 0
  br i1 %33, label %do.body59, label %do.end61
do.end61:
  br label %do.body62
do.body62:
  %34 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.22, i32 0, i32 0
  %35 = load i32, i32* @static_test_unsigned_int_operations.val.20
  %36 = call i32 @printf(i8* %34, i32 15, i32 %35)
  br label %do.cond63
do.cond63:
  %37 = icmp ne i32 0, 0
  br i1 %37, label %do.body62, label %do.end64
do.end64:
  br label %do.body65
do.body65:
  %38 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.23, i32 0, i32 0
  %39 = load i32, i32* @static_test_unsigned_int_operations.val.21
  %40 = call i32 @printf(i8* %38, i32 15, i32 %39)
  br label %do.cond66
do.cond66:
  %41 = icmp ne i32 0, 0
  br i1 %41, label %do.body65, label %do.end67
do.end67:
  br label %do.body68
do.body68:
  %42 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.24, i32 0, i32 0
  %43 = load i32, i32* @static_test_unsigned_int_operations.val.22
  %44 = call i32 @printf(i8* %42, i32 15, i32 %43)
  br label %do.cond69
do.cond69:
  %45 = icmp ne i32 0, 0
  br i1 %45, label %do.body68, label %do.end70
do.end70:
  br label %return37
return37:
  ret void
}

define dso_local void @static_test_logical_operators() {
entry:
  %0 = getelementptr inbounds [63 x i8], [63 x i8]* @.str.25, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  %2 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.26, i32 0, i32 0
  %3 = call i32 @printf(i8* %2)
  %4 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.27, i32 0, i32 0
  %5 = icmp ne i32 1, 0
  br label %land.lhs.eval74
land.lhs.eval74:
  br i1 %5, label %land.rhs72, label %land.end73
land.rhs72:
  %6 = icmp ne i32 0, 0
  br label %land.rhs.from75
land.rhs.from75:
  br label %land.end73
land.end73:
  %7 = phi i1 [ false, %land.lhs.eval74 ], [ %6, %land.rhs.from75 ]
  %8 = zext i1 %7 to i32
  %9 = call i32 @printf(i8* %4, i32 1, i32 0, i32 %8)
  %10 = getelementptr inbounds [36 x i8], [36 x i8]* @.str.28, i32 0, i32 0
  %11 = icmp ne i32 1, 0
  br label %land.lhs.eval78
land.lhs.eval78:
  br i1 %11, label %land.rhs76, label %land.end77
land.rhs76:
  %12 = icmp ne i32 1, 0
  br label %land.rhs.from79
land.rhs.from79:
  br label %land.end77
land.end77:
  %13 = phi i1 [ false, %land.lhs.eval78 ], [ %12, %land.rhs.from79 ]
  %14 = zext i1 %13 to i32
  %15 = call i32 @printf(i8* %10, i32 1, i32 %14)
  %16 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.29, i32 0, i32 0
  %17 = icmp ne i32 0, 0
  br label %land.lhs.eval82
land.lhs.eval82:
  br i1 %17, label %land.rhs80, label %land.end81
land.rhs80:
  %18 = icmp ne i32 5, 0
  br label %land.rhs.from83
land.rhs.from83:
  br label %land.end81
land.end81:
  %19 = phi i1 [ false, %land.lhs.eval82 ], [ %18, %land.rhs.from83 ]
  %20 = zext i1 %19 to i32
  %21 = call i32 @printf(i8* %16, i32 0, i32 5, i32 %20)
  %22 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.30, i32 0, i32 0
  %23 = icmp ne i32 5, 0
  br label %land.lhs.eval86
land.lhs.eval86:
  br i1 %23, label %land.rhs84, label %land.end85
land.rhs84:
  %24 = icmp ne i32 10, 0
  br label %land.rhs.from87
land.rhs.from87:
  br label %land.end85
land.end85:
  %25 = phi i1 [ false, %land.lhs.eval86 ], [ %24, %land.rhs.from87 ]
  %26 = zext i1 %25 to i32
  %27 = call i32 @printf(i8* %22, i32 5, i32 10, i32 %26)
  %28 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.31, i32 0, i32 0
  %29 = icmp ne i32 0, 0
  br label %land.lhs.eval90
land.lhs.eval90:
  br i1 %29, label %land.rhs88, label %land.end89
land.rhs88:
  %30 = icmp ne i32 0, 0
  br label %land.rhs.from91
land.rhs.from91:
  br label %land.end89
land.end89:
  %31 = phi i1 [ false, %land.lhs.eval90 ], [ %30, %land.rhs.from91 ]
  %32 = zext i1 %31 to i32
  %33 = call i32 @printf(i8* %28, i32 0, i32 0, i32 %32)
  %34 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.32, i32 0, i32 0
  %35 = icmp ne i32 1, 0
  br label %land.lhs.eval94
land.lhs.eval94:
  br i1 %35, label %land.rhs92, label %land.end93
land.rhs92:
  %36 = icmp ne i32 0, 0
  br label %land.rhs.from95
land.rhs.from95:
  br label %land.end93
land.end93:
  %37 = phi i1 [ false, %land.lhs.eval94 ], [ %36, %land.rhs.from95 ]
  %38 = zext i1 %37 to i32
  %39 = call i32 @printf(i8* %34, i32 1, i32 0, i32 %38)
  %40 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.33, i32 0, i32 0
  %41 = icmp ne i32 1, 0
  br label %land.lhs.eval98
land.lhs.eval98:
  br i1 %41, label %land.rhs96, label %land.end97
land.rhs96:
  %42 = icmp ne i32 1, 0
  br label %land.rhs.from99
land.rhs.from99:
  br label %land.end97
land.end97:
  %43 = phi i1 [ false, %land.lhs.eval98 ], [ %42, %land.rhs.from99 ]
  %44 = zext i1 %43 to i32
  %45 = call i32 @printf(i8* %40, i32 1, i32 1, i32 %44)
  %46 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.34, i32 0, i32 0
  %47 = call i32 @printf(i8* %46)
  %48 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.35, i32 0, i32 0
  %49 = icmp ne i32 1, 0
  br label %land.lhs.eval102
land.lhs.eval102:
  br i1 %49, label %land.end101, label %land.rhs100
land.rhs100:
  %50 = icmp ne i32 0, 0
  br label %land.rhs.from103
land.rhs.from103:
  br label %land.end101
land.end101:
  %51 = phi i1 [ true, %land.lhs.eval102 ], [ %50, %land.rhs.from103 ]
  %52 = zext i1 %51 to i32
  %53 = call i32 @printf(i8* %48, i32 1, i32 0, i32 %52)
  %54 = getelementptr inbounds [36 x i8], [36 x i8]* @.str.36, i32 0, i32 0
  %55 = icmp ne i32 0, 0
  br label %land.lhs.eval106
land.lhs.eval106:
  br i1 %55, label %land.end105, label %land.rhs104
land.rhs104:
  %56 = icmp ne i32 0, 0
  br label %land.rhs.from107
land.rhs.from107:
  br label %land.end105
land.end105:
  %57 = phi i1 [ true, %land.lhs.eval106 ], [ %56, %land.rhs.from107 ]
  %58 = zext i1 %57 to i32
  %59 = call i32 @printf(i8* %54, i32 0, i32 %58)
  %60 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.37, i32 0, i32 0
  %61 = icmp ne i32 0, 0
  br label %land.lhs.eval110
land.lhs.eval110:
  br i1 %61, label %land.end109, label %land.rhs108
land.rhs108:
  %62 = icmp ne i32 5, 0
  br label %land.rhs.from111
land.rhs.from111:
  br label %land.end109
land.end109:
  %63 = phi i1 [ true, %land.lhs.eval110 ], [ %62, %land.rhs.from111 ]
  %64 = zext i1 %63 to i32
  %65 = call i32 @printf(i8* %60, i32 0, i32 5, i32 %64)
  %66 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.38, i32 0, i32 0
  %67 = icmp ne i32 5, 0
  br label %land.lhs.eval114
land.lhs.eval114:
  br i1 %67, label %land.end113, label %land.rhs112
land.rhs112:
  %68 = icmp ne i32 10, 0
  br label %land.rhs.from115
land.rhs.from115:
  br label %land.end113
land.end113:
  %69 = phi i1 [ true, %land.lhs.eval114 ], [ %68, %land.rhs.from115 ]
  %70 = zext i1 %69 to i32
  %71 = call i32 @printf(i8* %66, i32 5, i32 10, i32 %70)
  %72 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.39, i32 0, i32 0
  %73 = icmp ne i32 0, 0
  br label %land.lhs.eval118
land.lhs.eval118:
  br i1 %73, label %land.end117, label %land.rhs116
land.rhs116:
  %74 = icmp ne i32 0, 0
  br label %land.rhs.from119
land.rhs.from119:
  br label %land.end117
land.end117:
  %75 = phi i1 [ true, %land.lhs.eval118 ], [ %74, %land.rhs.from119 ]
  %76 = zext i1 %75 to i32
  %77 = call i32 @printf(i8* %72, i32 0, i32 0, i32 %76)
  %78 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.40, i32 0, i32 0
  %79 = icmp ne i32 1, 0
  br label %land.lhs.eval122
land.lhs.eval122:
  br i1 %79, label %land.end121, label %land.rhs120
land.rhs120:
  %80 = icmp ne i32 0, 0
  br label %land.rhs.from123
land.rhs.from123:
  br label %land.end121
land.end121:
  %81 = phi i1 [ true, %land.lhs.eval122 ], [ %80, %land.rhs.from123 ]
  %82 = zext i1 %81 to i32
  %83 = call i32 @printf(i8* %78, i32 1, i32 0, i32 %82)
  %84 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.41, i32 0, i32 0
  %85 = icmp ne i32 1, 0
  br label %land.lhs.eval126
land.lhs.eval126:
  br i1 %85, label %land.end125, label %land.rhs124
land.rhs124:
  %86 = icmp ne i32 1, 0
  br label %land.rhs.from127
land.rhs.from127:
  br label %land.end125
land.end125:
  %87 = phi i1 [ true, %land.lhs.eval126 ], [ %86, %land.rhs.from127 ]
  %88 = zext i1 %87 to i32
  %89 = call i32 @printf(i8* %84, i32 1, i32 1, i32 %88)
  %90 = getelementptr inbounds [33 x i8], [33 x i8]* @.str.42, i32 0, i32 0
  %91 = call i32 @printf(i8* %90)
  %92 = getelementptr inbounds [45 x i8], [45 x i8]* @.str.43, i32 0, i32 0
  %93 = icmp ne i32 1, 0
  br i1 %93, label %cond.true128, label %cond.false129
cond.true128:
  br label %then.from131
then.from131:
  br label %cond.end130
cond.false129:
  br label %else.from132
else.from132:
  br label %cond.end130
cond.end130:
  %94 = phi i32 [ 5, %then.from131 ], [ 10, %else.from132 ]
  %95 = call i32 @printf(i8* %92, i32 1, i32 5, i32 10, i32 %94)
  %96 = getelementptr inbounds [46 x i8], [46 x i8]* @.str.44, i32 0, i32 0
  %97 = icmp ne i32 0, 0
  br i1 %97, label %cond.true133, label %cond.false134
cond.true133:
  br label %then.from136
then.from136:
  br label %cond.end135
cond.false134:
  br label %else.from137
else.from137:
  br label %cond.end135
cond.end135:
  %98 = phi i32 [ 5, %then.from136 ], [ 10, %else.from137 ]
  %99 = call i32 @printf(i8* %96, i32 0, i32 5, i32 10, i32 %98)
  %100 = getelementptr inbounds [55 x i8], [55 x i8]* @.str.45, i32 0, i32 0
  %101 = icmp sgt i32 5, 10
  %102 = zext i1 %101 to i32
  %103 = icmp ne i32 %102, 0
  br i1 %103, label %cond.true138, label %cond.false139
cond.true138:
  br label %then.from141
then.from141:
  br label %cond.end140
cond.false139:
  br label %else.from142
else.from142:
  br label %cond.end140
cond.end140:
  %104 = phi i32 [ 5, %then.from141 ], [ 10, %else.from142 ]
  %105 = call i32 @printf(i8* %100, i32 5, i32 10, i32 5, i32 10, i32 %104)
  %106 = getelementptr inbounds [54 x i8], [54 x i8]* @.str.46, i32 0, i32 0
  %107 = icmp slt i32 5, 10
  %108 = zext i1 %107 to i32
  %109 = icmp ne i32 %108, 0
  br i1 %109, label %cond.true143, label %cond.false144
cond.true143:
  br label %then.from146
then.from146:
  br label %cond.end145
cond.false144:
  br label %else.from147
else.from147:
  br label %cond.end145
cond.end145:
  %110 = phi i32 [ 5, %then.from146 ], [ 10, %else.from147 ]
  %111 = call i32 @printf(i8* %106, i32 5, i32 10, i32 5, i32 10, i32 %110)
  %112 = getelementptr inbounds [57 x i8], [57 x i8]* @.str.47, i32 0, i32 0
  %113 = icmp eq i32 5, 10
  %114 = zext i1 %113 to i32
  %115 = icmp ne i32 %114, 0
  br i1 %115, label %cond.true148, label %cond.false149
cond.true148:
  br label %then.from151
then.from151:
  br label %cond.end150
cond.false149:
  br label %else.from152
else.from152:
  br label %cond.end150
cond.end150:
  %116 = phi i32 [ 5, %then.from151 ], [ 10, %else.from152 ]
  %117 = call i32 @printf(i8* %112, i32 5, i32 10, i32 5, i32 10, i32 %116)
  br label %return71
return71:
  ret void
}

define dso_local void @static_test_float_operations() {
entry:
  %0 = getelementptr inbounds [26 x i8], [26 x i8]* @.str.48, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  br label %do.body154
do.body154:
  %2 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.49, i32 0, i32 0
  %3 = load float, float* @static_test_float_operations.val.23
  %4 = fpext float %3 to double
  %5 = call i32 @printf(i8* %2, double 10.500000, double 3.200000, double %4)
  br label %do.cond155
do.cond155:
  %6 = icmp ne i32 0, 0
  br i1 %6, label %do.body154, label %do.end156
do.end156:
  br label %do.body157
do.body157:
  %7 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.50, i32 0, i32 0
  %8 = load float, float* @static_test_float_operations.val.24
  %9 = fpext float %8 to double
  %10 = call i32 @printf(i8* %7, double 10.500000, double 3.200000, double %9)
  br label %do.cond158
do.cond158:
  %11 = icmp ne i32 0, 0
  br i1 %11, label %do.body157, label %do.end159
do.end159:
  br label %do.body160
do.body160:
  %12 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.51, i32 0, i32 0
  %13 = load float, float* @static_test_float_operations.val.25
  %14 = fpext float %13 to double
  %15 = call i32 @printf(i8* %12, double 10.500000, double 3.200000, double %14)
  br label %do.cond161
do.cond161:
  %16 = icmp ne i32 0, 0
  br i1 %16, label %do.body160, label %do.end162
do.end162:
  br label %do.body163
do.body163:
  %17 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.52, i32 0, i32 0
  %18 = load float, float* @static_test_float_operations.val.26
  %19 = fpext float %18 to double
  %20 = call i32 @printf(i8* %17, double 10.500000, double 3.200000, double %19)
  br label %do.cond164
do.cond164:
  %21 = icmp ne i32 0, 0
  br i1 %21, label %do.body163, label %do.end165
do.end165:
  br label %do.body166
do.body166:
  %22 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.53, i32 0, i32 0
  %23 = fneg double 2.400000
  %24 = load float, float* @static_test_float_operations.val.27
  %25 = fpext float %24 to double
  %26 = call i32 @printf(i8* %22, double 10.500000, double %23, double %25)
  br label %do.cond167
do.cond167:
  %27 = icmp ne i32 0, 0
  br i1 %27, label %do.body166, label %do.end168
do.end168:
  br label %do.body169
do.body169:
  %28 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.54, i32 0, i32 0
  %29 = fneg double 2.400000
  %30 = load float, float* @static_test_float_operations.val.28
  %31 = fpext float %30 to double
  %32 = call i32 @printf(i8* %28, double 10.500000, double %29, double %31)
  br label %do.cond170
do.cond170:
  %33 = icmp ne i32 0, 0
  br i1 %33, label %do.body169, label %do.end171
do.end171:
  br label %do.body172
do.body172:
  %34 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.55, i32 0, i32 0
  %35 = fneg double 2.400000
  %36 = load float, float* @static_test_float_operations.val.29
  %37 = fpext float %36 to double
  %38 = call i32 @printf(i8* %34, double 10.500000, double %35, double %37)
  br label %do.cond173
do.cond173:
  %39 = icmp ne i32 0, 0
  br i1 %39, label %do.body172, label %do.end174
do.end174:
  br label %do.body175
do.body175:
  %40 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.56, i32 0, i32 0
  %41 = fneg double 2.400000
  %42 = load float, float* @static_test_float_operations.val.30
  %43 = fpext float %42 to double
  %44 = call i32 @printf(i8* %40, double 10.500000, double %41, double %43)
  br label %do.cond176
do.cond176:
  %45 = icmp ne i32 0, 0
  br i1 %45, label %do.body175, label %do.end177
do.end177:
  br label %do.body178
do.body178:
  %46 = getelementptr inbounds [35 x i8], [35 x i8]* @.str.57, i32 0, i32 0
  %47 = load float, float* @static_test_float_operations.val.31
  %48 = fpext float %47 to double
  %49 = call i32 @printf(i8* %46, double 10.500000, double %48)
  br label %do.cond179
do.cond179:
  %50 = icmp ne i32 0, 0
  br i1 %50, label %do.body178, label %do.end180
do.end180:
  br label %return153
return153:
  ret void
}

define dso_local void @test_operator_precedence() {
entry:
  %a.addr0 = alloca i32
  store i32 10, i32* %a.addr0
  %d.addr1 = alloca i32
  %2 = sub i32 0, 3
  store i32 %2, i32* %d.addr1
  %c.addr3 = alloca i32
  store i32 2, i32* %c.addr3
  %b.addr4 = alloca i32
  store i32 5, i32* %b.addr4
  %x.addr5 = alloca float
  %6 = fptrunc double 10.500000 to float
  store float %6, float* %x.addr5
  %z.addr7 = alloca float
  %8 = fptrunc double 2.000000 to float
  store float %8, float* %z.addr7
  %y.addr9 = alloca float
  %10 = fptrunc double 5.200000 to float
  store float %10, float* %y.addr9
  %11 = getelementptr inbounds [29 x i8], [29 x i8]* @.str.58, i32 0, i32 0
  %12 = call i32 @printf(i8* %11)
  %13 = getelementptr inbounds [43 x i8], [43 x i8]* @.str.59, i32 0, i32 0
  %14 = load i32, i32* %a.addr0
  %15 = load i32, i32* %b.addr4
  %16 = load i32, i32* %c.addr3
  %17 = load i32, i32* %a.addr0
  %18 = load i32, i32* %b.addr4
  %19 = load i32, i32* %c.addr3
  %20 = mul i32 %18, %19
  %21 = add i32 %17, %20
  %22 = call i32 @printf(i8* %13, i32 %14, i32 %15, i32 %16, i32 %21)
  %23 = getelementptr inbounds [43 x i8], [43 x i8]* @.str.60, i32 0, i32 0
  %24 = load i32, i32* %a.addr0
  %25 = load i32, i32* %b.addr4
  %26 = load i32, i32* %c.addr3
  %27 = load i32, i32* %a.addr0
  %28 = load i32, i32* %b.addr4
  %29 = load i32, i32* %c.addr3
  %30 = sdiv i32 %28, %29
  %31 = sub i32 %27, %30
  %32 = call i32 @printf(i8* %23, i32 %24, i32 %25, i32 %26, i32 %31)
  %33 = getelementptr inbounds [45 x i8], [45 x i8]* @.str.61, i32 0, i32 0
  %34 = load i32, i32* %a.addr0
  %35 = load i32, i32* %b.addr4
  %36 = load i32, i32* %c.addr3
  %37 = load i32, i32* %a.addr0
  %38 = load i32, i32* %b.addr4
  %39 = add i32 %37, %38
  %40 = load i32, i32* %c.addr3
  %41 = mul i32 %39, %40
  %42 = call i32 @printf(i8* %33, i32 %34, i32 %35, i32 %36, i32 %41)
  %43 = getelementptr inbounds [43 x i8], [43 x i8]* @.str.62, i32 0, i32 0
  %44 = load i32, i32* %a.addr0
  %45 = load i32, i32* %b.addr4
  %46 = load i32, i32* %c.addr3
  %47 = load i32, i32* %a.addr0
  %48 = load i32, i32* %b.addr4
  %49 = load i32, i32* %c.addr3
  %50 = and i32 %48, %49
  %51 = add i32 %47, %50
  %52 = call i32 @printf(i8* %43, i32 %44, i32 %45, i32 %46, i32 %51)
  %53 = getelementptr inbounds [43 x i8], [43 x i8]* @.str.63, i32 0, i32 0
  %54 = load i32, i32* %a.addr0
  %55 = load i32, i32* %b.addr4
  %56 = load i32, i32* %c.addr3
  %57 = load i32, i32* %a.addr0
  %58 = load i32, i32* %b.addr4
  %59 = load i32, i32* %c.addr3
  %60 = mul i32 %58, %59
  %61 = or i32 %57, %60
  %62 = call i32 @printf(i8* %53, i32 %54, i32 %55, i32 %56, i32 %61)
  %63 = getelementptr inbounds [45 x i8], [45 x i8]* @.str.64, i32 0, i32 0
  %64 = load i32, i32* %a.addr0
  %65 = load i32, i32* %b.addr4
  %66 = load i32, i32* %c.addr3
  %67 = load i32, i32* %a.addr0
  %68 = load i32, i32* %b.addr4
  %69 = or i32 %67, %68
  %70 = load i32, i32* %c.addr3
  %71 = mul i32 %69, %70
  %72 = call i32 @printf(i8* %63, i32 %64, i32 %65, i32 %66, i32 %71)
  %73 = getelementptr inbounds [48 x i8], [48 x i8]* @.str.65, i32 0, i32 0
  %74 = load i32, i32* %a.addr0
  %75 = load i32, i32* %b.addr4
  %76 = load i32, i32* %a.addr0
  %77 = shl i32 %76, 1
  %78 = load i32, i32* %b.addr4
  %79 = ashr i32 %78, 1
  %80 = add i32 %77, %79
  %81 = call i32 @printf(i8* %73, i32 %74, i32 %75, i32 %80)
  %82 = getelementptr inbounds [52 x i8], [52 x i8]* @.str.66, i32 0, i32 0
  %83 = load i32, i32* %a.addr0
  %84 = load i32, i32* %b.addr4
  %85 = load i32, i32* %a.addr0
  %86 = shl i32 %85, 1
  %87 = load i32, i32* %b.addr4
  %88 = ashr i32 %87, 1
  %89 = add i32 %86, %88
  %90 = call i32 @printf(i8* %82, i32 %83, i32 %84, i32 %89)
  %91 = getelementptr inbounds [49 x i8], [49 x i8]* @.str.67, i32 0, i32 0
  %92 = load i32, i32* %a.addr0
  %93 = load i32, i32* %b.addr4
  %94 = load i32, i32* %b.addr4
  %95 = load i32, i32* %c.addr3
  %96 = load i32, i32* %a.addr0
  %97 = load i32, i32* %b.addr4
  %98 = icmp slt i32 %96, %97
  %99 = zext i1 %98 to i32
  %100 = icmp ne i32 %99, 0
  br label %land.lhs.eval184
land.lhs.eval184:
  br i1 %100, label %land.end183, label %land.rhs182
land.rhs182:
  %101 = load i32, i32* %b.addr4
  %102 = load i32, i32* %c.addr3
  %103 = and i32 %101, %102
  %104 = icmp ne i32 %103, 0
  br label %land.rhs.from185
land.rhs.from185:
  br label %land.end183
land.end183:
  %105 = phi i1 [ true, %land.lhs.eval184 ], [ %104, %land.rhs.from185 ]
  %106 = zext i1 %105 to i32
  %107 = call i32 @printf(i8* %91, i32 %92, i32 %93, i32 %94, i32 %95, i32 %106)
  %108 = getelementptr inbounds [50 x i8], [50 x i8]* @.str.68, i32 0, i32 0
  %109 = load i32, i32* %a.addr0
  %110 = load i32, i32* %b.addr4
  %111 = load i32, i32* %b.addr4
  %112 = load i32, i32* %c.addr3
  %113 = load i32, i32* %a.addr0
  %114 = load i32, i32* %b.addr4
  %115 = icmp slt i32 %113, %114
  %116 = zext i1 %115 to i32
  %117 = icmp ne i32 %116, 0
  br label %land.lhs.eval188
land.lhs.eval188:
  br i1 %117, label %land.end187, label %land.rhs186
land.rhs186:
  %118 = load i32, i32* %b.addr4
  %119 = icmp ne i32 %118, 0
  br label %land.lhs.eval191
land.lhs.eval191:
  br i1 %119, label %land.rhs189, label %land.end190
land.rhs189:
  %120 = load i32, i32* %c.addr3
  %121 = icmp ne i32 %120, 0
  br label %land.rhs.from192
land.rhs.from192:
  br label %land.end190
land.end190:
  %122 = phi i1 [ false, %land.lhs.eval191 ], [ %121, %land.rhs.from192 ]
  %123 = zext i1 %122 to i32
  %124 = icmp ne i32 %123, 0
  br label %land.rhs.from193
land.rhs.from193:
  br label %land.end187
land.end187:
  %125 = phi i1 [ true, %land.lhs.eval188 ], [ %124, %land.rhs.from193 ]
  %126 = zext i1 %125 to i32
  %127 = call i32 @printf(i8* %108, i32 %109, i32 %110, i32 %111, i32 %112, i32 %126)
  %128 = getelementptr inbounds [50 x i8], [50 x i8]* @.str.69, i32 0, i32 0
  %129 = load i32, i32* %a.addr0
  %130 = load i32, i32* %b.addr4
  %131 = load i32, i32* %c.addr3
  %132 = load i32, i32* %d.addr1
  %133 = load i32, i32* %a.addr0
  %134 = load i32, i32* %b.addr4
  %135 = load i32, i32* %c.addr3
  %136 = mul i32 %134, %135
  %137 = load i32, i32* %d.addr1
  %138 = sub i32 %136, %137
  %139 = add i32 %133, %138
  %140 = call i32 @printf(i8* %128, i32 %129, i32 %130, i32 %131, i32 %132, i32 %139)
  %141 = getelementptr inbounds [52 x i8], [52 x i8]* @.str.70, i32 0, i32 0
  %142 = load i32, i32* %a.addr0
  %143 = load i32, i32* %b.addr4
  %144 = load i32, i32* %c.addr3
  %145 = load i32, i32* %d.addr1
  %146 = load i32, i32* %a.addr0
  %147 = load i32, i32* %b.addr4
  %148 = add i32 %146, %147
  %149 = load i32, i32* %c.addr3
  %150 = load i32, i32* %d.addr1
  %151 = add i32 %149, %150
  %152 = mul i32 %148, %151
  %153 = call i32 @printf(i8* %141, i32 %142, i32 %143, i32 %144, i32 %145, i32 %152)
  %154 = getelementptr inbounds [35 x i8], [35 x i8]* @.str.71, i32 0, i32 0
  %155 = call i32 @printf(i8* %154)
  %156 = getelementptr inbounds [51 x i8], [51 x i8]* @.str.72, i32 0, i32 0
  %157 = load float, float* %x.addr5
  %158 = fpext float %157 to double
  %159 = load float, float* %y.addr9
  %160 = fpext float %159 to double
  %161 = load float, float* %z.addr7
  %162 = fpext float %161 to double
  %163 = load float, float* %x.addr5
  %164 = load float, float* %y.addr9
  %165 = load float, float* %z.addr7
  %166 = fmul float %164, %165
  %167 = fadd float %163, %166
  %168 = fpext float %167 to double
  %169 = call i32 @printf(i8* %156, double %158, double %160, double %162, double %168)
  %170 = getelementptr inbounds [51 x i8], [51 x i8]* @.str.73, i32 0, i32 0
  %171 = load float, float* %x.addr5
  %172 = fpext float %171 to double
  %173 = load float, float* %y.addr9
  %174 = fpext float %173 to double
  %175 = load float, float* %z.addr7
  %176 = fpext float %175 to double
  %177 = load float, float* %x.addr5
  %178 = load float, float* %y.addr9
  %179 = load float, float* %z.addr7
  %180 = fdiv float %178, %179
  %181 = fsub float %177, %180
  %182 = fpext float %181 to double
  %183 = call i32 @printf(i8* %170, double %172, double %174, double %176, double %182)
  %184 = getelementptr inbounds [53 x i8], [53 x i8]* @.str.74, i32 0, i32 0
  %185 = load float, float* %x.addr5
  %186 = fpext float %185 to double
  %187 = load float, float* %y.addr9
  %188 = fpext float %187 to double
  %189 = load float, float* %z.addr7
  %190 = fpext float %189 to double
  %191 = load float, float* %x.addr5
  %192 = load float, float* %y.addr9
  %193 = fadd float %191, %192
  %194 = load float, float* %z.addr7
  %195 = fmul float %193, %194
  %196 = fpext float %195 to double
  %197 = call i32 @printf(i8* %184, double %186, double %188, double %190, double %196)
  %198 = getelementptr inbounds [35 x i8], [35 x i8]* @.str.75, i32 0, i32 0
  %199 = call i32 @printf(i8* %198)
  %200 = getelementptr inbounds [43 x i8], [43 x i8]* @.str.76, i32 0, i32 0
  %201 = load i32, i32* %d.addr1
  %202 = load i32, i32* %b.addr4
  %203 = load i32, i32* %c.addr3
  %204 = load i32, i32* %d.addr1
  %205 = load i32, i32* %b.addr4
  %206 = mul i32 %204, %205
  %207 = load i32, i32* %c.addr3
  %208 = sdiv i32 %206, %207
  %209 = call i32 @printf(i8* %200, i32 %201, i32 %202, i32 %203, i32 %208)
  %210 = getelementptr inbounds [44 x i8], [44 x i8]* @.str.77, i32 0, i32 0
  %211 = load i32, i32* %d.addr1
  %212 = load i32, i32* %a.addr0
  %213 = load i32, i32* %b.addr4
  %214 = load i32, i32* %d.addr1
  %215 = load i32, i32* %a.addr0
  %216 = load i32, i32* %b.addr4
  %217 = sub i32 0, %216
  %218 = mul i32 %215, %217
  %219 = add i32 %214, %218
  %220 = call i32 @printf(i8* %210, i32 %211, i32 %212, i32 %213, i32 %219)
  br label %return181
return181:
  ret void
}

define dso_local i32 @static_main() {
entry:
  %retval = alloca i32
  call void @static_test_signed_int_operations()
  %0 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.78, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  call void @static_test_unsigned_int_operations()
  %2 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.79, i32 0, i32 0
  %3 = call i32 @printf(i8* %2)
  call void @static_test_float_operations()
  %4 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.80, i32 0, i32 0
  %5 = call i32 @printf(i8* %4)
  call void @static_test_logical_operators()
  store i32 0, i32* %retval
  br label %return194
return.dead195:
  br label %return194
return194:
  %6 = load i32, i32* %retval
  ret i32 %6
}

define dso_local void @test_signed_int_operations() {
entry:
  %a.addr0 = alloca i32
  store i32 15, i32* %a.addr0
  %c.addr1 = alloca i32
  store i32 7, i32* %c.addr1
  %b.addr2 = alloca i32
  %3 = sub i32 0, 3
  store i32 %3, i32* %b.addr2
  %4 = getelementptr inbounds [46 x i8], [46 x i8]* @.str.81, i32 0, i32 0
  %5 = call i32 @printf(i8* %4)
  br label %do.body197
do.body197:
  %val.addr6 = alloca i32
  %7 = load i32, i32* %a.addr0
  %8 = load i32, i32* %b.addr2
  %9 = add i32 %7, %8
  store i32 %9, i32* %val.addr6
  %10 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.82, i32 0, i32 0
  %11 = load i32, i32* %a.addr0
  %12 = load i32, i32* %b.addr2
  %13 = load i32, i32* %val.addr6
  %14 = call i32 @printf(i8* %10, i32 %11, i32 %12, i32 %13)
  br label %do.cond198
do.cond198:
  %15 = icmp ne i32 0, 0
  br i1 %15, label %do.body197, label %do.end199
do.end199:
  br label %do.body200
do.body200:
  %val.addr16 = alloca i32
  %17 = load i32, i32* %a.addr0
  %18 = load i32, i32* %b.addr2
  %19 = sub i32 %17, %18
  store i32 %19, i32* %val.addr16
  %20 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.83, i32 0, i32 0
  %21 = load i32, i32* %a.addr0
  %22 = load i32, i32* %b.addr2
  %23 = load i32, i32* %val.addr16
  %24 = call i32 @printf(i8* %20, i32 %21, i32 %22, i32 %23)
  br label %do.cond201
do.cond201:
  %25 = icmp ne i32 0, 0
  br i1 %25, label %do.body200, label %do.end202
do.end202:
  br label %do.body203
do.body203:
  %val.addr26 = alloca i32
  %27 = load i32, i32* %a.addr0
  %28 = load i32, i32* %b.addr2
  %29 = mul i32 %27, %28
  store i32 %29, i32* %val.addr26
  %30 = getelementptr inbounds [32 x i8], [32 x i8]* @.str.84, i32 0, i32 0
  %31 = load i32, i32* %a.addr0
  %32 = load i32, i32* %b.addr2
  %33 = load i32, i32* %val.addr26
  %34 = call i32 @printf(i8* %30, i32 %31, i32 %32, i32 %33)
  br label %do.cond204
do.cond204:
  %35 = icmp ne i32 0, 0
  br i1 %35, label %do.body203, label %do.end205
do.end205:
  br label %do.body206
do.body206:
  %val.addr36 = alloca i32
  %37 = load i32, i32* %a.addr0
  %38 = load i32, i32* %c.addr1
  %39 = sdiv i32 %37, %38
  store i32 %39, i32* %val.addr36
  %40 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.85, i32 0, i32 0
  %41 = load i32, i32* %a.addr0
  %42 = load i32, i32* %c.addr1
  %43 = load i32, i32* %val.addr36
  %44 = call i32 @printf(i8* %40, i32 %41, i32 %42, i32 %43)
  br label %do.cond207
do.cond207:
  %45 = icmp ne i32 0, 0
  br i1 %45, label %do.body206, label %do.end208
do.end208:
  br label %do.body209
do.body209:
  %val.addr46 = alloca i32
  %47 = load i32, i32* %a.addr0
  %48 = load i32, i32* %b.addr2
  %49 = sdiv i32 %47, %48
  store i32 %49, i32* %val.addr46
  %50 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.86, i32 0, i32 0
  %51 = load i32, i32* %a.addr0
  %52 = load i32, i32* %b.addr2
  %53 = load i32, i32* %val.addr46
  %54 = call i32 @printf(i8* %50, i32 %51, i32 %52, i32 %53)
  br label %do.cond210
do.cond210:
  %55 = icmp ne i32 0, 0
  br i1 %55, label %do.body209, label %do.end211
do.end211:
  br label %do.body212
do.body212:
  %val.addr56 = alloca i32
  %57 = load i32, i32* %a.addr0
  %58 = load i32, i32* %c.addr1
  %59 = srem i32 %57, %58
  store i32 %59, i32* %val.addr56
  %60 = getelementptr inbounds [30 x i8], [30 x i8]* @.str.87, i32 0, i32 0
  %61 = load i32, i32* %a.addr0
  %62 = load i32, i32* %c.addr1
  %63 = load i32, i32* %val.addr56
  %64 = call i32 @printf(i8* %60, i32 %61, i32 %62, i32 %63)
  br label %do.cond213
do.cond213:
  %65 = icmp ne i32 0, 0
  br i1 %65, label %do.body212, label %do.end214
do.end214:
  br label %do.body215
do.body215:
  %val.addr66 = alloca i32
  %67 = load i32, i32* %a.addr0
  %68 = load i32, i32* %c.addr1
  %69 = and i32 %67, %68
  store i32 %69, i32* %val.addr66
  %70 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.88, i32 0, i32 0
  %71 = load i32, i32* %a.addr0
  %72 = load i32, i32* %c.addr1
  %73 = load i32, i32* %val.addr66
  %74 = call i32 @printf(i8* %70, i32 %71, i32 %72, i32 %73)
  br label %do.cond216
do.cond216:
  %75 = icmp ne i32 0, 0
  br i1 %75, label %do.body215, label %do.end217
do.end217:
  br label %do.body218
do.body218:
  %val.addr76 = alloca i32
  %77 = load i32, i32* %a.addr0
  %78 = load i32, i32* %c.addr1
  %79 = or i32 %77, %78
  store i32 %79, i32* %val.addr76
  %80 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.89, i32 0, i32 0
  %81 = load i32, i32* %a.addr0
  %82 = load i32, i32* %c.addr1
  %83 = load i32, i32* %val.addr76
  %84 = call i32 @printf(i8* %80, i32 %81, i32 %82, i32 %83)
  br label %do.cond219
do.cond219:
  %85 = icmp ne i32 0, 0
  br i1 %85, label %do.body218, label %do.end220
do.end220:
  br label %do.body221
do.body221:
  %val.addr86 = alloca i32
  %87 = load i32, i32* %a.addr0
  %88 = load i32, i32* %c.addr1
  %89 = xor i32 %87, %88
  store i32 %89, i32* %val.addr86
  %90 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.90, i32 0, i32 0
  %91 = load i32, i32* %a.addr0
  %92 = load i32, i32* %c.addr1
  %93 = load i32, i32* %val.addr86
  %94 = call i32 @printf(i8* %90, i32 %91, i32 %92, i32 %93)
  br label %do.cond222
do.cond222:
  %95 = icmp ne i32 0, 0
  br i1 %95, label %do.body221, label %do.end223
do.end223:
  br label %do.body224
do.body224:
  %val.addr96 = alloca i32
  %97 = load i32, i32* %a.addr0
  %98 = xor i32 %97, -1
  store i32 %98, i32* %val.addr96
  %99 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.91, i32 0, i32 0
  %100 = load i32, i32* %a.addr0
  %101 = load i32, i32* %val.addr96
  %102 = call i32 @printf(i8* %99, i32 %100, i32 %101)
  br label %do.cond225
do.cond225:
  %103 = icmp ne i32 0, 0
  br i1 %103, label %do.body224, label %do.end226
do.end226:
  br label %do.body227
do.body227:
  %val.addr104 = alloca i32
  %105 = load i32, i32* %a.addr0
  %106 = shl i32 %105, 1
  store i32 %106, i32* %val.addr104
  %107 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.92, i32 0, i32 0
  %108 = load i32, i32* %a.addr0
  %109 = load i32, i32* %val.addr104
  %110 = call i32 @printf(i8* %107, i32 %108, i32 %109)
  br label %do.cond228
do.cond228:
  %111 = icmp ne i32 0, 0
  br i1 %111, label %do.body227, label %do.end229
do.end229:
  br label %do.body230
do.body230:
  %val.addr112 = alloca i32
  %113 = load i32, i32* %a.addr0
  %114 = ashr i32 %113, 1
  store i32 %114, i32* %val.addr112
  %115 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.93, i32 0, i32 0
  %116 = load i32, i32* %a.addr0
  %117 = load i32, i32* %val.addr112
  %118 = call i32 @printf(i8* %115, i32 %116, i32 %117)
  br label %do.cond231
do.cond231:
  %119 = icmp ne i32 0, 0
  br i1 %119, label %do.body230, label %do.end232
do.end232:
  br label %return196
return196:
  ret void
}

define dso_local void @test_unsigned_int_operations() {
entry:
  %a.addr0 = alloca i32
  store i32 15, i32* %a.addr0
  %c.addr1 = alloca i32
  store i32 7, i32* %c.addr1
  %b.addr2 = alloca i32
  store i32 3, i32* %b.addr2
  %3 = getelementptr inbounds [48 x i8], [48 x i8]* @.str.94, i32 0, i32 0
  %4 = call i32 @printf(i8* %3)
  br label %do.body234
do.body234:
  %val.addr5 = alloca i32
  %6 = load i32, i32* %a.addr0
  %7 = load i32, i32* %b.addr2
  %8 = add i32 %6, %7
  store i32 %8, i32* %val.addr5
  %9 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.95, i32 0, i32 0
  %10 = load i32, i32* %a.addr0
  %11 = load i32, i32* %b.addr2
  %12 = load i32, i32* %val.addr5
  %13 = call i32 @printf(i8* %9, i32 %10, i32 %11, i32 %12)
  br label %do.cond235
do.cond235:
  %14 = icmp ne i32 0, 0
  br i1 %14, label %do.body234, label %do.end236
do.end236:
  br label %do.body237
do.body237:
  %val.addr15 = alloca i32
  %16 = load i32, i32* %a.addr0
  %17 = load i32, i32* %b.addr2
  %18 = sub i32 %16, %17
  store i32 %18, i32* %val.addr15
  %19 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.96, i32 0, i32 0
  %20 = load i32, i32* %a.addr0
  %21 = load i32, i32* %b.addr2
  %22 = load i32, i32* %val.addr15
  %23 = call i32 @printf(i8* %19, i32 %20, i32 %21, i32 %22)
  br label %do.cond238
do.cond238:
  %24 = icmp ne i32 0, 0
  br i1 %24, label %do.body237, label %do.end239
do.end239:
  br label %do.body240
do.body240:
  %val.addr25 = alloca i32
  %26 = load i32, i32* %a.addr0
  %27 = load i32, i32* %b.addr2
  %28 = mul i32 %26, %27
  store i32 %28, i32* %val.addr25
  %29 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.97, i32 0, i32 0
  %30 = load i32, i32* %a.addr0
  %31 = load i32, i32* %b.addr2
  %32 = load i32, i32* %val.addr25
  %33 = call i32 @printf(i8* %29, i32 %30, i32 %31, i32 %32)
  br label %do.cond241
do.cond241:
  %34 = icmp ne i32 0, 0
  br i1 %34, label %do.body240, label %do.end242
do.end242:
  br label %do.body243
do.body243:
  %val.addr35 = alloca i32
  %36 = load i32, i32* %a.addr0
  %37 = load i32, i32* %c.addr1
  %38 = sdiv i32 %36, %37
  store i32 %38, i32* %val.addr35
  %39 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.98, i32 0, i32 0
  %40 = load i32, i32* %a.addr0
  %41 = load i32, i32* %c.addr1
  %42 = load i32, i32* %val.addr35
  %43 = call i32 @printf(i8* %39, i32 %40, i32 %41, i32 %42)
  br label %do.cond244
do.cond244:
  %44 = icmp ne i32 0, 0
  br i1 %44, label %do.body243, label %do.end245
do.end245:
  br label %do.body246
do.body246:
  %val.addr45 = alloca i32
  %46 = load i32, i32* %a.addr0
  %47 = load i32, i32* %c.addr1
  %48 = srem i32 %46, %47
  store i32 %48, i32* %val.addr45
  %49 = getelementptr inbounds [32 x i8], [32 x i8]* @.str.99, i32 0, i32 0
  %50 = load i32, i32* %a.addr0
  %51 = load i32, i32* %c.addr1
  %52 = load i32, i32* %val.addr45
  %53 = call i32 @printf(i8* %49, i32 %50, i32 %51, i32 %52)
  br label %do.cond247
do.cond247:
  %54 = icmp ne i32 0, 0
  br i1 %54, label %do.body246, label %do.end248
do.end248:
  br label %do.body249
do.body249:
  %val.addr55 = alloca i32
  %56 = load i32, i32* %a.addr0
  %57 = load i32, i32* %c.addr1
  %58 = and i32 %56, %57
  store i32 %58, i32* %val.addr55
  %59 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.100, i32 0, i32 0
  %60 = load i32, i32* %a.addr0
  %61 = load i32, i32* %c.addr1
  %62 = load i32, i32* %val.addr55
  %63 = call i32 @printf(i8* %59, i32 %60, i32 %61, i32 %62)
  br label %do.cond250
do.cond250:
  %64 = icmp ne i32 0, 0
  br i1 %64, label %do.body249, label %do.end251
do.end251:
  br label %do.body252
do.body252:
  %val.addr65 = alloca i32
  %66 = load i32, i32* %a.addr0
  %67 = load i32, i32* %c.addr1
  %68 = or i32 %66, %67
  store i32 %68, i32* %val.addr65
  %69 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.101, i32 0, i32 0
  %70 = load i32, i32* %a.addr0
  %71 = load i32, i32* %c.addr1
  %72 = load i32, i32* %val.addr65
  %73 = call i32 @printf(i8* %69, i32 %70, i32 %71, i32 %72)
  br label %do.cond253
do.cond253:
  %74 = icmp ne i32 0, 0
  br i1 %74, label %do.body252, label %do.end254
do.end254:
  br label %do.body255
do.body255:
  %val.addr75 = alloca i32
  %76 = load i32, i32* %a.addr0
  %77 = load i32, i32* %c.addr1
  %78 = xor i32 %76, %77
  store i32 %78, i32* %val.addr75
  %79 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.102, i32 0, i32 0
  %80 = load i32, i32* %a.addr0
  %81 = load i32, i32* %c.addr1
  %82 = load i32, i32* %val.addr75
  %83 = call i32 @printf(i8* %79, i32 %80, i32 %81, i32 %82)
  br label %do.cond256
do.cond256:
  %84 = icmp ne i32 0, 0
  br i1 %84, label %do.body255, label %do.end257
do.end257:
  br label %do.body258
do.body258:
  %val.addr85 = alloca i32
  %86 = load i32, i32* %a.addr0
  %87 = xor i32 %86, -1
  store i32 %87, i32* %val.addr85
  %88 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.103, i32 0, i32 0
  %89 = load i32, i32* %a.addr0
  %90 = load i32, i32* %val.addr85
  %91 = call i32 @printf(i8* %88, i32 %89, i32 %90)
  br label %do.cond259
do.cond259:
  %92 = icmp ne i32 0, 0
  br i1 %92, label %do.body258, label %do.end260
do.end260:
  br label %do.body261
do.body261:
  %val.addr93 = alloca i32
  %94 = load i32, i32* %a.addr0
  %95 = shl i32 %94, 1
  store i32 %95, i32* %val.addr93
  %96 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.104, i32 0, i32 0
  %97 = load i32, i32* %a.addr0
  %98 = load i32, i32* %val.addr93
  %99 = call i32 @printf(i8* %96, i32 %97, i32 %98)
  br label %do.cond262
do.cond262:
  %100 = icmp ne i32 0, 0
  br i1 %100, label %do.body261, label %do.end263
do.end263:
  br label %do.body264
do.body264:
  %val.addr101 = alloca i32
  %102 = load i32, i32* %a.addr0
  %103 = ashr i32 %102, 1
  store i32 %103, i32* %val.addr101
  %104 = getelementptr inbounds [31 x i8], [31 x i8]* @.str.105, i32 0, i32 0
  %105 = load i32, i32* %a.addr0
  %106 = load i32, i32* %val.addr101
  %107 = call i32 @printf(i8* %104, i32 %105, i32 %106)
  br label %do.cond265
do.cond265:
  %108 = icmp ne i32 0, 0
  br i1 %108, label %do.body264, label %do.end266
do.end266:
  br label %return233
return233:
  ret void
}

define dso_local void @test_logical_operators() {
entry:
  %a.addr0 = alloca i32
  store i32 1, i32* %a.addr0
  %d.addr1 = alloca i32
  store i32 10, i32* %d.addr1
  %c.addr2 = alloca i32
  store i32 5, i32* %c.addr2
  %b.addr3 = alloca i32
  store i32 0, i32* %b.addr3
  %result.addr4 = alloca i32
  %5 = getelementptr inbounds [63 x i8], [63 x i8]* @.str.106, i32 0, i32 0
  %6 = call i32 @printf(i8* %5)
  %7 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.107, i32 0, i32 0
  %8 = call i32 @printf(i8* %7)
  %9 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.108, i32 0, i32 0
  %10 = load i32, i32* %a.addr0
  %11 = load i32, i32* %b.addr3
  %12 = load i32, i32* %a.addr0
  %13 = icmp ne i32 %12, 0
  br label %land.lhs.eval270
land.lhs.eval270:
  br i1 %13, label %land.rhs268, label %land.end269
land.rhs268:
  %14 = load i32, i32* %b.addr3
  %15 = icmp ne i32 %14, 0
  br label %land.rhs.from271
land.rhs.from271:
  br label %land.end269
land.end269:
  %16 = phi i1 [ false, %land.lhs.eval270 ], [ %15, %land.rhs.from271 ]
  %17 = zext i1 %16 to i32
  %18 = call i32 @printf(i8* %9, i32 %10, i32 %11, i32 %17)
  %19 = getelementptr inbounds [36 x i8], [36 x i8]* @.str.109, i32 0, i32 0
  %20 = load i32, i32* %a.addr0
  %21 = load i32, i32* %a.addr0
  %22 = icmp ne i32 %21, 0
  br label %land.lhs.eval274
land.lhs.eval274:
  br i1 %22, label %land.rhs272, label %land.end273
land.rhs272:
  %23 = icmp ne i32 1, 0
  br label %land.rhs.from275
land.rhs.from275:
  br label %land.end273
land.end273:
  %24 = phi i1 [ false, %land.lhs.eval274 ], [ %23, %land.rhs.from275 ]
  %25 = zext i1 %24 to i32
  %26 = call i32 @printf(i8* %19, i32 %20, i32 %25)
  %27 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.110, i32 0, i32 0
  %28 = load i32, i32* %b.addr3
  %29 = load i32, i32* %c.addr2
  %30 = load i32, i32* %b.addr3
  %31 = icmp ne i32 %30, 0
  br label %land.lhs.eval278
land.lhs.eval278:
  br i1 %31, label %land.rhs276, label %land.end277
land.rhs276:
  %32 = load i32, i32* %c.addr2
  %33 = icmp ne i32 %32, 0
  br label %land.rhs.from279
land.rhs.from279:
  br label %land.end277
land.end277:
  %34 = phi i1 [ false, %land.lhs.eval278 ], [ %33, %land.rhs.from279 ]
  %35 = zext i1 %34 to i32
  %36 = call i32 @printf(i8* %27, i32 %28, i32 %29, i32 %35)
  %37 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.111, i32 0, i32 0
  %38 = load i32, i32* %c.addr2
  %39 = load i32, i32* %d.addr1
  %40 = load i32, i32* %c.addr2
  %41 = icmp ne i32 %40, 0
  br label %land.lhs.eval282
land.lhs.eval282:
  br i1 %41, label %land.rhs280, label %land.end281
land.rhs280:
  %42 = load i32, i32* %d.addr1
  %43 = icmp ne i32 %42, 0
  br label %land.rhs.from283
land.rhs.from283:
  br label %land.end281
land.end281:
  %44 = phi i1 [ false, %land.lhs.eval282 ], [ %43, %land.rhs.from283 ]
  %45 = zext i1 %44 to i32
  %46 = call i32 @printf(i8* %37, i32 %38, i32 %39, i32 %45)
  %47 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.112, i32 0, i32 0
  %48 = icmp ne i32 0, 0
  br label %land.lhs.eval286
land.lhs.eval286:
  br i1 %48, label %land.rhs284, label %land.end285
land.rhs284:
  %49 = icmp ne i32 0, 0
  br label %land.rhs.from287
land.rhs.from287:
  br label %land.end285
land.end285:
  %50 = phi i1 [ false, %land.lhs.eval286 ], [ %49, %land.rhs.from287 ]
  %51 = zext i1 %50 to i32
  %52 = call i32 @printf(i8* %47, i32 0, i32 0, i32 %51)
  %53 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.113, i32 0, i32 0
  %54 = icmp ne i32 1, 0
  br label %land.lhs.eval290
land.lhs.eval290:
  br i1 %54, label %land.rhs288, label %land.end289
land.rhs288:
  %55 = icmp ne i32 0, 0
  br label %land.rhs.from291
land.rhs.from291:
  br label %land.end289
land.end289:
  %56 = phi i1 [ false, %land.lhs.eval290 ], [ %55, %land.rhs.from291 ]
  %57 = zext i1 %56 to i32
  %58 = call i32 @printf(i8* %53, i32 1, i32 0, i32 %57)
  %59 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.114, i32 0, i32 0
  %60 = icmp ne i32 1, 0
  br label %land.lhs.eval294
land.lhs.eval294:
  br i1 %60, label %land.rhs292, label %land.end293
land.rhs292:
  %61 = icmp ne i32 1, 0
  br label %land.rhs.from295
land.rhs.from295:
  br label %land.end293
land.end293:
  %62 = phi i1 [ false, %land.lhs.eval294 ], [ %61, %land.rhs.from295 ]
  %63 = zext i1 %62 to i32
  %64 = call i32 @printf(i8* %59, i32 1, i32 1, i32 %63)
  %65 = getelementptr inbounds [86 x i8], [86 x i8]* @.str.115, i32 0, i32 0
  %66 = load i32, i32* %b.addr3
  %67 = load i32, i32* %c.addr2
  %68 = load i32, i32* %b.addr3
  %69 = icmp ne i32 %68, 0
  br label %land.lhs.eval298
land.lhs.eval298:
  br i1 %69, label %land.rhs296, label %land.end297
land.rhs296:
  %70 = load i32, i32* %c.addr2
  %71 = add i32 %70, 1
  store i32 %71, i32* %c.addr2
  %72 = icmp ne i32 %71, 0
  br label %land.rhs.from299
land.rhs.from299:
  br label %land.end297
land.end297:
  %73 = phi i1 [ false, %land.lhs.eval298 ], [ %72, %land.rhs.from299 ]
  %74 = zext i1 %73 to i32
  %75 = call i32 @printf(i8* %65, i32 %66, i32 %67, i32 %74)
  %76 = getelementptr inbounds [55 x i8], [55 x i8]* @.str.116, i32 0, i32 0
  %77 = load i32, i32* %c.addr2
  %78 = call i32 @printf(i8* %76, i32 %77)
  %79 = getelementptr inbounds [27 x i8], [27 x i8]* @.str.117, i32 0, i32 0
  %80 = call i32 @printf(i8* %79)
  %81 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.118, i32 0, i32 0
  %82 = load i32, i32* %a.addr0
  %83 = load i32, i32* %b.addr3
  %84 = load i32, i32* %a.addr0
  %85 = icmp ne i32 %84, 0
  br label %land.lhs.eval302
land.lhs.eval302:
  br i1 %85, label %land.end301, label %land.rhs300
land.rhs300:
  %86 = load i32, i32* %b.addr3
  %87 = icmp ne i32 %86, 0
  br label %land.rhs.from303
land.rhs.from303:
  br label %land.end301
land.end301:
  %88 = phi i1 [ true, %land.lhs.eval302 ], [ %87, %land.rhs.from303 ]
  %89 = zext i1 %88 to i32
  %90 = call i32 @printf(i8* %81, i32 %82, i32 %83, i32 %89)
  %91 = getelementptr inbounds [36 x i8], [36 x i8]* @.str.119, i32 0, i32 0
  %92 = load i32, i32* %b.addr3
  %93 = load i32, i32* %b.addr3
  %94 = icmp ne i32 %93, 0
  br label %land.lhs.eval306
land.lhs.eval306:
  br i1 %94, label %land.end305, label %land.rhs304
land.rhs304:
  %95 = icmp ne i32 0, 0
  br label %land.rhs.from307
land.rhs.from307:
  br label %land.end305
land.end305:
  %96 = phi i1 [ true, %land.lhs.eval306 ], [ %95, %land.rhs.from307 ]
  %97 = zext i1 %96 to i32
  %98 = call i32 @printf(i8* %91, i32 %92, i32 %97)
  %99 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.120, i32 0, i32 0
  %100 = load i32, i32* %c.addr2
  %101 = icmp ne i32 0, 0
  br label %land.lhs.eval310
land.lhs.eval310:
  br i1 %101, label %land.end309, label %land.rhs308
land.rhs308:
  %102 = load i32, i32* %c.addr2
  %103 = icmp ne i32 %102, 0
  br label %land.rhs.from311
land.rhs.from311:
  br label %land.end309
land.end309:
  %104 = phi i1 [ true, %land.lhs.eval310 ], [ %103, %land.rhs.from311 ]
  %105 = zext i1 %104 to i32
  %106 = call i32 @printf(i8* %99, i32 0, i32 %100, i32 %105)
  %107 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.121, i32 0, i32 0
  %108 = load i32, i32* %c.addr2
  %109 = load i32, i32* %d.addr1
  %110 = load i32, i32* %c.addr2
  %111 = icmp ne i32 %110, 0
  br label %land.lhs.eval314
land.lhs.eval314:
  br i1 %111, label %land.end313, label %land.rhs312
land.rhs312:
  %112 = load i32, i32* %d.addr1
  %113 = icmp ne i32 %112, 0
  br label %land.rhs.from315
land.rhs.from315:
  br label %land.end313
land.end313:
  %114 = phi i1 [ true, %land.lhs.eval314 ], [ %113, %land.rhs.from315 ]
  %115 = zext i1 %114 to i32
  %116 = call i32 @printf(i8* %107, i32 %108, i32 %109, i32 %115)
  %117 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.122, i32 0, i32 0
  %118 = icmp ne i32 0, 0
  br label %land.lhs.eval318
land.lhs.eval318:
  br i1 %118, label %land.end317, label %land.rhs316
land.rhs316:
  %119 = icmp ne i32 0, 0
  br label %land.rhs.from319
land.rhs.from319:
  br label %land.end317
land.end317:
  %120 = phi i1 [ true, %land.lhs.eval318 ], [ %119, %land.rhs.from319 ]
  %121 = zext i1 %120 to i32
  %122 = call i32 @printf(i8* %117, i32 0, i32 0, i32 %121)
  %123 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.123, i32 0, i32 0
  %124 = icmp ne i32 1, 0
  br label %land.lhs.eval322
land.lhs.eval322:
  br i1 %124, label %land.end321, label %land.rhs320
land.rhs320:
  %125 = icmp ne i32 0, 0
  br label %land.rhs.from323
land.rhs.from323:
  br label %land.end321
land.end321:
  %126 = phi i1 [ true, %land.lhs.eval322 ], [ %125, %land.rhs.from323 ]
  %127 = zext i1 %126 to i32
  %128 = call i32 @printf(i8* %123, i32 1, i32 0, i32 %127)
  %129 = getelementptr inbounds [37 x i8], [37 x i8]* @.str.124, i32 0, i32 0
  %130 = icmp ne i32 1, 0
  br label %land.lhs.eval326
land.lhs.eval326:
  br i1 %130, label %land.end325, label %land.rhs324
land.rhs324:
  %131 = icmp ne i32 1, 0
  br label %land.rhs.from327
land.rhs.from327:
  br label %land.end325
land.end325:
  %132 = phi i1 [ true, %land.lhs.eval326 ], [ %131, %land.rhs.from327 ]
  %133 = zext i1 %132 to i32
  %134 = call i32 @printf(i8* %129, i32 1, i32 1, i32 %133)
  %135 = getelementptr inbounds [86 x i8], [86 x i8]* @.str.125, i32 0, i32 0
  %136 = load i32, i32* %a.addr0
  %137 = load i32, i32* %d.addr1
  %138 = load i32, i32* %a.addr0
  %139 = icmp ne i32 %138, 0
  br label %land.lhs.eval330
land.lhs.eval330:
  br i1 %139, label %land.end329, label %land.rhs328
land.rhs328:
  %140 = load i32, i32* %d.addr1
  %141 = add i32 %140, 1
  store i32 %141, i32* %d.addr1
  %142 = icmp ne i32 %141, 0
  br label %land.rhs.from331
land.rhs.from331:
  br label %land.end329
land.end329:
  %143 = phi i1 [ true, %land.lhs.eval330 ], [ %142, %land.rhs.from331 ]
  %144 = zext i1 %143 to i32
  %145 = call i32 @printf(i8* %135, i32 %136, i32 %137, i32 %144)
  %146 = getelementptr inbounds [56 x i8], [56 x i8]* @.str.126, i32 0, i32 0
  %147 = load i32, i32* %d.addr1
  %148 = call i32 @printf(i8* %146, i32 %147)
  %149 = getelementptr inbounds [33 x i8], [33 x i8]* @.str.127, i32 0, i32 0
  %150 = call i32 @printf(i8* %149)
  %151 = getelementptr inbounds [45 x i8], [45 x i8]* @.str.128, i32 0, i32 0
  %152 = load i32, i32* %a.addr0
  %153 = load i32, i32* %c.addr2
  %154 = load i32, i32* %d.addr1
  %155 = load i32, i32* %a.addr0
  %156 = icmp ne i32 %155, 0
  br i1 %156, label %cond.true332, label %cond.false333
cond.true332:
  %157 = load i32, i32* %c.addr2
  br label %then.from335
then.from335:
  br label %cond.end334
cond.false333:
  %158 = load i32, i32* %d.addr1
  br label %else.from336
else.from336:
  br label %cond.end334
cond.end334:
  %159 = phi i32 [ %157, %then.from335 ], [ %158, %else.from336 ]
  %160 = call i32 @printf(i8* %151, i32 %152, i32 %153, i32 %154, i32 %159)
  %161 = getelementptr inbounds [46 x i8], [46 x i8]* @.str.129, i32 0, i32 0
  %162 = load i32, i32* %b.addr3
  %163 = load i32, i32* %c.addr2
  %164 = load i32, i32* %d.addr1
  %165 = load i32, i32* %b.addr3
  %166 = icmp ne i32 %165, 0
  br i1 %166, label %cond.true337, label %cond.false338
cond.true337:
  %167 = load i32, i32* %c.addr2
  br label %then.from340
then.from340:
  br label %cond.end339
cond.false338:
  %168 = load i32, i32* %d.addr1
  br label %else.from341
else.from341:
  br label %cond.end339
cond.end339:
  %169 = phi i32 [ %167, %then.from340 ], [ %168, %else.from341 ]
  %170 = call i32 @printf(i8* %161, i32 %162, i32 %163, i32 %164, i32 %169)
  %171 = getelementptr inbounds [55 x i8], [55 x i8]* @.str.130, i32 0, i32 0
  %172 = load i32, i32* %c.addr2
  %173 = load i32, i32* %d.addr1
  %174 = load i32, i32* %c.addr2
  %175 = load i32, i32* %d.addr1
  %176 = load i32, i32* %c.addr2
  %177 = load i32, i32* %d.addr1
  %178 = icmp sgt i32 %176, %177
  %179 = zext i1 %178 to i32
  %180 = icmp ne i32 %179, 0
  br i1 %180, label %cond.true342, label %cond.false343
cond.true342:
  %181 = load i32, i32* %c.addr2
  br label %then.from345
then.from345:
  br label %cond.end344
cond.false343:
  %182 = load i32, i32* %d.addr1
  br label %else.from346
else.from346:
  br label %cond.end344
cond.end344:
  %183 = phi i32 [ %181, %then.from345 ], [ %182, %else.from346 ]
  %184 = call i32 @printf(i8* %171, i32 %172, i32 %173, i32 %174, i32 %175, i32 %183)
  %185 = getelementptr inbounds [54 x i8], [54 x i8]* @.str.131, i32 0, i32 0
  %186 = load i32, i32* %c.addr2
  %187 = load i32, i32* %d.addr1
  %188 = load i32, i32* %c.addr2
  %189 = load i32, i32* %d.addr1
  %190 = load i32, i32* %c.addr2
  %191 = load i32, i32* %d.addr1
  %192 = icmp slt i32 %190, %191
  %193 = zext i1 %192 to i32
  %194 = icmp ne i32 %193, 0
  br i1 %194, label %cond.true347, label %cond.false348
cond.true347:
  %195 = load i32, i32* %c.addr2
  br label %then.from350
then.from350:
  br label %cond.end349
cond.false348:
  %196 = load i32, i32* %d.addr1
  br label %else.from351
else.from351:
  br label %cond.end349
cond.end349:
  %197 = phi i32 [ %195, %then.from350 ], [ %196, %else.from351 ]
  %198 = call i32 @printf(i8* %185, i32 %186, i32 %187, i32 %188, i32 %189, i32 %197)
  %199 = getelementptr inbounds [57 x i8], [57 x i8]* @.str.132, i32 0, i32 0
  %200 = load i32, i32* %c.addr2
  %201 = load i32, i32* %d.addr1
  %202 = load i32, i32* %c.addr2
  %203 = load i32, i32* %d.addr1
  %204 = load i32, i32* %c.addr2
  %205 = load i32, i32* %d.addr1
  %206 = icmp eq i32 %204, %205
  %207 = zext i1 %206 to i32
  %208 = icmp ne i32 %207, 0
  br i1 %208, label %cond.true352, label %cond.false353
cond.true352:
  %209 = load i32, i32* %c.addr2
  br label %then.from355
then.from355:
  br label %cond.end354
cond.false353:
  %210 = load i32, i32* %d.addr1
  br label %else.from356
else.from356:
  br label %cond.end354
cond.end354:
  %211 = phi i32 [ %209, %then.from355 ], [ %210, %else.from356 ]
  %212 = call i32 @printf(i8* %199, i32 %200, i32 %201, i32 %202, i32 %203, i32 %211)
  %213 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.133, i32 0, i32 0
  %214 = call i32 @printf(i8* %213)
  %215 = load i32, i32* %a.addr0
  %216 = icmp ne i32 %215, 0
  br i1 %216, label %cond.true357, label %cond.false358
cond.true357:
  %217 = load i32, i32* %c.addr2
  %218 = add i32 %217, 2
  store i32 %218, i32* %c.addr2
  br label %then.from360
then.from360:
  br label %cond.end359
cond.false358:
  %219 = load i32, i32* %d.addr1
  br label %else.from361
else.from361:
  br label %cond.end359
cond.end359:
  %220 = phi i32 [ %218, %then.from360 ], [ %219, %else.from361 ]
  store i32 %220, i32* %result.addr4
  %221 = getelementptr inbounds [77 x i8], [77 x i8]* @.str.134, i32 0, i32 0
  %222 = load i32, i32* %a.addr0
  %223 = load i32, i32* %c.addr2
  %224 = load i32, i32* %result.addr4
  %225 = call i32 @printf(i8* %221, i32 %222, i32 %223, i32 %224)
  %226 = getelementptr inbounds [44 x i8], [44 x i8]* @.str.135, i32 0, i32 0
  %227 = load i32, i32* %c.addr2
  %228 = call i32 @printf(i8* %226, i32 %227)
  store i32 5, i32* %c.addr2
  %229 = load i32, i32* %b.addr3
  %230 = icmp ne i32 %229, 0
  br i1 %230, label %cond.true362, label %cond.false363
cond.true362:
  %231 = load i32, i32* %c.addr2
  br label %then.from365
then.from365:
  br label %cond.end364
cond.false363:
  %232 = load i32, i32* %d.addr1
  %233 = add i32 %232, 3
  store i32 %233, i32* %d.addr1
  br label %else.from366
else.from366:
  br label %cond.end364
cond.end364:
  %234 = phi i32 [ %231, %then.from365 ], [ %233, %else.from366 ]
  store i32 %234, i32* %result.addr4
  %235 = getelementptr inbounds [78 x i8], [78 x i8]* @.str.136, i32 0, i32 0
  %236 = load i32, i32* %b.addr3
  %237 = load i32, i32* %d.addr1
  %238 = load i32, i32* %result.addr4
  %239 = call i32 @printf(i8* %235, i32 %236, i32 %237, i32 %238)
  %240 = getelementptr inbounds [45 x i8], [45 x i8]* @.str.137, i32 0, i32 0
  %241 = load i32, i32* %d.addr1
  %242 = call i32 @printf(i8* %240, i32 %241)
  br label %return267
return267:
  ret void
}

define dso_local void @test_float_operations() {
entry:
  %a.addr0 = alloca float
  %1 = fptrunc double 10.500000 to float
  store float %1, float* %a.addr0
  %c.addr2 = alloca float
  %3 = fneg double 2.400000
  %4 = fptrunc double %3 to float
  store float %4, float* %c.addr2
  %b.addr5 = alloca float
  %6 = fptrunc double 3.200000 to float
  store float %6, float* %b.addr5
  %7 = getelementptr inbounds [26 x i8], [26 x i8]* @.str.138, i32 0, i32 0
  %8 = call i32 @printf(i8* %7)
  br label %do.body368
do.body368:
  %val.addr9 = alloca float
  %10 = load float, float* %a.addr0
  %11 = load float, float* %b.addr5
  %12 = fadd float %10, %11
  store float %12, float* %val.addr9
  %13 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.139, i32 0, i32 0
  %14 = load float, float* %a.addr0
  %15 = fpext float %14 to double
  %16 = load float, float* %b.addr5
  %17 = fpext float %16 to double
  %18 = load float, float* %val.addr9
  %19 = fpext float %18 to double
  %20 = call i32 @printf(i8* %13, double %15, double %17, double %19)
  br label %do.cond369
do.cond369:
  %21 = icmp ne i32 0, 0
  br i1 %21, label %do.body368, label %do.end370
do.end370:
  br label %do.body371
do.body371:
  %val.addr22 = alloca float
  %23 = load float, float* %a.addr0
  %24 = load float, float* %b.addr5
  %25 = fsub float %23, %24
  store float %25, float* %val.addr22
  %26 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.140, i32 0, i32 0
  %27 = load float, float* %a.addr0
  %28 = fpext float %27 to double
  %29 = load float, float* %b.addr5
  %30 = fpext float %29 to double
  %31 = load float, float* %val.addr22
  %32 = fpext float %31 to double
  %33 = call i32 @printf(i8* %26, double %28, double %30, double %32)
  br label %do.cond372
do.cond372:
  %34 = icmp ne i32 0, 0
  br i1 %34, label %do.body371, label %do.end373
do.end373:
  br label %do.body374
do.body374:
  %val.addr35 = alloca float
  %36 = load float, float* %a.addr0
  %37 = load float, float* %b.addr5
  %38 = fmul float %36, %37
  store float %38, float* %val.addr35
  %39 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.141, i32 0, i32 0
  %40 = load float, float* %a.addr0
  %41 = fpext float %40 to double
  %42 = load float, float* %b.addr5
  %43 = fpext float %42 to double
  %44 = load float, float* %val.addr35
  %45 = fpext float %44 to double
  %46 = call i32 @printf(i8* %39, double %41, double %43, double %45)
  br label %do.cond375
do.cond375:
  %47 = icmp ne i32 0, 0
  br i1 %47, label %do.body374, label %do.end376
do.end376:
  br label %do.body377
do.body377:
  %val.addr48 = alloca float
  %49 = load float, float* %a.addr0
  %50 = load float, float* %b.addr5
  %51 = fdiv float %49, %50
  store float %51, float* %val.addr48
  %52 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.142, i32 0, i32 0
  %53 = load float, float* %a.addr0
  %54 = fpext float %53 to double
  %55 = load float, float* %b.addr5
  %56 = fpext float %55 to double
  %57 = load float, float* %val.addr48
  %58 = fpext float %57 to double
  %59 = call i32 @printf(i8* %52, double %54, double %56, double %58)
  br label %do.cond378
do.cond378:
  %60 = icmp ne i32 0, 0
  br i1 %60, label %do.body377, label %do.end379
do.end379:
  br label %do.body380
do.body380:
  %val.addr61 = alloca float
  %62 = load float, float* %a.addr0
  %63 = load float, float* %c.addr2
  %64 = fadd float %62, %63
  store float %64, float* %val.addr61
  %65 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.143, i32 0, i32 0
  %66 = load float, float* %a.addr0
  %67 = fpext float %66 to double
  %68 = load float, float* %c.addr2
  %69 = fpext float %68 to double
  %70 = load float, float* %val.addr61
  %71 = fpext float %70 to double
  %72 = call i32 @printf(i8* %65, double %67, double %69, double %71)
  br label %do.cond381
do.cond381:
  %73 = icmp ne i32 0, 0
  br i1 %73, label %do.body380, label %do.end382
do.end382:
  br label %do.body383
do.body383:
  %val.addr74 = alloca float
  %75 = load float, float* %a.addr0
  %76 = load float, float* %c.addr2
  %77 = fsub float %75, %76
  store float %77, float* %val.addr74
  %78 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.144, i32 0, i32 0
  %79 = load float, float* %a.addr0
  %80 = fpext float %79 to double
  %81 = load float, float* %c.addr2
  %82 = fpext float %81 to double
  %83 = load float, float* %val.addr74
  %84 = fpext float %83 to double
  %85 = call i32 @printf(i8* %78, double %80, double %82, double %84)
  br label %do.cond384
do.cond384:
  %86 = icmp ne i32 0, 0
  br i1 %86, label %do.body383, label %do.end385
do.end385:
  br label %do.body386
do.body386:
  %val.addr87 = alloca float
  %88 = load float, float* %a.addr0
  %89 = load float, float* %c.addr2
  %90 = fmul float %88, %89
  store float %90, float* %val.addr87
  %91 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.145, i32 0, i32 0
  %92 = load float, float* %a.addr0
  %93 = fpext float %92 to double
  %94 = load float, float* %c.addr2
  %95 = fpext float %94 to double
  %96 = load float, float* %val.addr87
  %97 = fpext float %96 to double
  %98 = call i32 @printf(i8* %91, double %93, double %95, double %97)
  br label %do.cond387
do.cond387:
  %99 = icmp ne i32 0, 0
  br i1 %99, label %do.body386, label %do.end388
do.end388:
  br label %do.body389
do.body389:
  %val.addr100 = alloca float
  %101 = load float, float* %a.addr0
  %102 = load float, float* %c.addr2
  %103 = fdiv float %101, %102
  store float %103, float* %val.addr100
  %104 = getelementptr inbounds [41 x i8], [41 x i8]* @.str.146, i32 0, i32 0
  %105 = load float, float* %a.addr0
  %106 = fpext float %105 to double
  %107 = load float, float* %c.addr2
  %108 = fpext float %107 to double
  %109 = load float, float* %val.addr100
  %110 = fpext float %109 to double
  %111 = call i32 @printf(i8* %104, double %106, double %108, double %110)
  br label %do.cond390
do.cond390:
  %112 = icmp ne i32 0, 0
  br i1 %112, label %do.body389, label %do.end391
do.end391:
  br label %do.body392
do.body392:
  %val.addr113 = alloca float
  %114 = load float, float* %a.addr0
  %115 = fneg float %114
  store float %115, float* %val.addr113
  %116 = getelementptr inbounds [35 x i8], [35 x i8]* @.str.147, i32 0, i32 0
  %117 = load float, float* %a.addr0
  %118 = fpext float %117 to double
  %119 = load float, float* %val.addr113
  %120 = fpext float %119 to double
  %121 = call i32 @printf(i8* %116, double %118, double %120)
  br label %do.cond393
do.cond393:
  %122 = icmp ne i32 0, 0
  br i1 %122, label %do.body392, label %do.end394
do.end394:
  br label %return367
return367:
  ret void
}

define dso_local i32 @main() {
entry:
  %retval = alloca i32
  store i32 0, i32* %retval
  call void @test_signed_int_operations()
  %0 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.148, i32 0, i32 0
  %1 = call i32 @printf(i8* %0)
  call void @test_unsigned_int_operations()
  %2 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.149, i32 0, i32 0
  %3 = call i32 @printf(i8* %2)
  call void @test_float_operations()
  %4 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.150, i32 0, i32 0
  %5 = call i32 @printf(i8* %4)
  call void @test_logical_operators()
  %6 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.151, i32 0, i32 0
  %7 = call i32 @printf(i8* %6)
  %8 = getelementptr inbounds [33 x i8], [33 x i8]* @.str.152, i32 0, i32 0
  %9 = call i32 @printf(i8* %8)
  %10 = call i32 @static_main()
  %11 = getelementptr inbounds [2 x i8], [2 x i8]* @.str.153, i32 0, i32 0
  %12 = call i32 @printf(i8* %11)
  call void @test_operator_precedence()
  store i32 0, i32* %retval
  br label %return395
return.dead396:
  br label %return395
return395:
  %13 = load i32, i32* %retval
  ret i32 %13
}
!llvm.module.flags = !{!0, !1}
!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}

	.file	"cc1"
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	subl	$8, %esp
	.cfi_def_cfa_offset 16
	.cfi_offset %ebx, -8
	calll	.L0$pb
	.cfi_adjust_cfa_offset 4
.L0$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp0:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp0-.L0$pb), %ebx
	movl	$0, 4(%esp)
	leal	.L.str.0@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.1@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$2
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.2@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$4
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.3@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$4
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.4@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$4
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.5@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$8
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.6@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$12
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.7@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$12
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.8@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$8
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.9@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$1337                           # imm = 0x539
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.10@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$42
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.11@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$43
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.12@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$44
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.13@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$42
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.14@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$43
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	leal	.L.str.15@GOTOFF(%ebx), %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	$2
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	4(%esp), %eax
	addl	$8, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	s1,@object                      # @s1
	.bss
	.globl	s1
	.p2align	4, 0x0
s1:
	.zero	16044
	.size	s1, 16044

	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.0:
	.asciz	"size of char: %zu\n"
	.size	.L.str.0, 19

	.type	.L.str.1,@object                # @.str.1
	.p2align	4, 0x0
.L.str.1:
	.asciz	"size of short int: %zu\n"
	.size	.L.str.1, 24

	.type	.L.str.2,@object                # @.str.2
	.p2align	4, 0x0
.L.str.2:
	.asciz	"size of int: %zu\n"
	.size	.L.str.2, 18

	.type	.L.str.3,@object                # @.str.3
	.p2align	4, 0x0
.L.str.3:
	.asciz	"size of long int: %zu\n"
	.size	.L.str.3, 23

	.type	.L.str.4,@object                # @.str.4
	.p2align	4, 0x0
.L.str.4:
	.asciz	"size of float: %zu\n"
	.size	.L.str.4, 20

	.type	.L.str.5,@object                # @.str.5
	.p2align	4, 0x0
.L.str.5:
	.asciz	"size of double: %zu\n"
	.size	.L.str.5, 21

	.type	.L.str.6,@object                # @.str.6
	.p2align	4, 0x0
.L.str.6:
	.asciz	"size of long double: %zu\n"
	.size	.L.str.6, 26

	.type	.L.str.7,@object                # @.str.7
	.p2align	4, 0x0
.L.str.7:
	.asciz	"size of struct s: %zu\n"
	.size	.L.str.7, 23

	.type	.L.str.8,@object                # @.str.8
	.p2align	4, 0x0
.L.str.8:
	.asciz	"size of union u: %zu\n"
	.size	.L.str.8, 22

	.type	.L.str.9,@object                # @.str.9
	.p2align	4, 0x0
.L.str.9:
	.asciz	"enum constant A: %d\n"
	.size	.L.str.9, 21

	.type	.L.str.10,@object               # @.str.10
	.p2align	4, 0x0
.L.str.10:
	.asciz	"enum constant B: %d\n"
	.size	.L.str.10, 21

	.type	.L.str.11,@object               # @.str.11
	.p2align	4, 0x0
.L.str.11:
	.asciz	"enum constant C: %d\n"
	.size	.L.str.11, 21

	.type	.L.str.12,@object               # @.str.12
	.p2align	4, 0x0
.L.str.12:
	.asciz	"enum constant D: %d\n"
	.size	.L.str.12, 21

	.type	.L.str.13,@object               # @.str.13
	.p2align	4, 0x0
.L.str.13:
	.asciz	"enum constant E: %d\n"
	.size	.L.str.13, 21

	.type	.L.str.14,@object               # @.str.14
	.p2align	4, 0x0
.L.str.14:
	.asciz	"enum constant F: %d\n"
	.size	.L.str.14, 21

	.type	.L.str.15,@object               # @.str.15
	.p2align	4, 0x0
.L.str.15:
	.asciz	"enum constant G: %d\n"
	.size	.L.str.15, 21

	.section	".note.GNU-stack","",@progbits

	.file	"cc1"
	.text
	.globl	print_array                     # -- Begin function print_array
	.p2align	4
	.type	print_array,@function
print_array:                            # @print_array
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebp
	.cfi_def_cfa_offset 8
	pushl	%ebx
	.cfi_def_cfa_offset 12
	pushl	%edi
	.cfi_def_cfa_offset 16
	pushl	%esi
	.cfi_def_cfa_offset 20
	subl	$76, %esp
	.cfi_def_cfa_offset 96
	.cfi_offset %esi, -20
	.cfi_offset %edi, -16
	.cfi_offset %ebx, -12
	.cfi_offset %ebp, -8
	calll	.L0$pb
	.cfi_adjust_cfa_offset 4
.L0$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp0:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp0-.L0$pb), %ebx
	leal	.L.str.0@GOTOFF(%ebx), %eax
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	$0, 20(%esp)
	leal	.L.str.1@GOTOFF(%ebx), %eax
	movl	%eax, 72(%esp)                  # 4-byte Spill
	leal	.L.str.2@GOTOFF(%ebx), %eax
	movl	%eax, 68(%esp)                  # 4-byte Spill
	leal	.L.str.3@GOTOFF(%ebx), %eax
	movl	%eax, 64(%esp)                  # 4-byte Spill
	leal	.L.str.4@GOTOFF(%ebx), %eax
	movl	%eax, 60(%esp)                  # 4-byte Spill
	leal	.L.str.5@GOTOFF(%ebx), %eax
	movl	%eax, 56(%esp)                  # 4-byte Spill
	leal	.L.str.6@GOTOFF(%ebx), %eax
	movl	%eax, 52(%esp)                  # 4-byte Spill
	leal	.L.str.7@GOTOFF(%ebx), %eax
	movl	%eax, 48(%esp)                  # 4-byte Spill
	leal	.L.str.8@GOTOFF(%ebx), %eax
	movl	%eax, 44(%esp)                  # 4-byte Spill
	leal	.L.str.9@GOTOFF(%ebx), %eax
	movl	%eax, 40(%esp)                  # 4-byte Spill
	leal	.L.str.10@GOTOFF(%ebx), %eax
	movl	%eax, 36(%esp)                  # 4-byte Spill
	leal	.L.str.11@GOTOFF(%ebx), %eax
	movl	%eax, 32(%esp)                  # 4-byte Spill
	leal	.L.str.12@GOTOFF(%ebx), %eax
	movl	%eax, 28(%esp)                  # 4-byte Spill
	leal	.L.str.13@GOTOFF(%ebx), %eax
	movl	%eax, 24(%esp)                  # 4-byte Spill
	leal	.L.str.14@GOTOFF(%ebx), %edi
	leal	.L.str.15@GOTOFF(%ebx), %ebp
	leal	.L.str.16@GOTOFF(%ebx), %esi
	cmpl	$3, 20(%esp)
	jg	.LBB0_3
	.p2align	4
.LBB0_2:                                # %for.body2
                                        # =>This Inner Loop Header: Depth=1
	movl	20(%esp), %eax
	movl	%eax, 4(%esp)
	movl	72(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	68(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movss	(%eax,%edx), %xmm0              # xmm0 = mem[0],zero,zero,zero
	movss	%xmm0, 4(%esp)
	movl	64(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	60(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movl	4(%eax,%edx), %eax
	movl	%eax, 4(%esp)
	movl	56(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movl	8(%eax,%edx), %eax
	movl	%eax, 4(%esp)
	movl	52(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	48(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movl	12(%eax,%edx), %eax
	movl	%eax, 4(%esp)
	movl	44(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	40(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movzbl	16(%eax,%edx), %eax
	movl	%eax, 4(%esp)
	movl	36(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movzbl	17(%eax,%edx), %eax
	movl	%eax, 4(%esp)
	movl	32(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	28(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movl	20(%eax,%edx), %eax
	movl	%eax, 4(%esp)
	movl	24(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	96(%esp), %eax
	movl	20(%esp), %ecx
	leal	(%ecx,%ecx,8), %edx
	leal	(%edx,%edx,2), %edx
	addl	%ecx, %edx
	movl	24(%eax,%edx), %eax
	movl	%eax, 4(%esp)
	movl	%edi, (%esp)
	calll	printf@PLT
	movl	%ebp, (%esp)
	calll	printf@PLT
	movl	%esi, (%esp)
	calll	printf@PLT
	incl	20(%esp)
	cmpl	$3, 20(%esp)
	jle	.LBB0_2
.LBB0_3:                                # %for.end4
	leal	.L.str.17@GOTOFF(%ebx), %eax
	movl	%eax, (%esp)
	calll	printf@PLT
	addl	$76, %esp
	.cfi_def_cfa_offset 20
	popl	%esi
	.cfi_def_cfa_offset 16
	popl	%edi
	.cfi_def_cfa_offset 12
	popl	%ebx
	.cfi_def_cfa_offset 8
	popl	%ebp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	print_array, .Lfunc_end0-print_array
	.cfi_endproc
                                        # -- End function
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
	calll	.L1$pb
	.cfi_adjust_cfa_offset 4
.L1$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp1:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp1-.L1$pb), %ebx
	movl	$0, 4(%esp)
	leal	.L.str.18@GOTOFF(%ebx), %eax
	movl	%eax, (%esp)
	calll	printf@PLT
	leal	array1@GOTOFF(%ebx), %eax
	movl	%eax, (%esp)
	calll	print_array
	leal	.L.str.19@GOTOFF(%ebx), %eax
	movl	%eax, (%esp)
	calll	printf@PLT
	leal	array2@GOTOFF(%ebx), %eax
	movl	%eax, (%esp)
	calll	print_array
	movl	array1@GOTOFF(%ebx), %eax
	cmpl	array2@GOTOFF(%ebx), %eax
	je	.LBB1_2
# %bb.1:                                # %if.then6
	leal	.L.str.20@GOTOFF(%ebx), %eax
	jmp	.LBB1_3
.LBB1_2:                                # %if.else7
	leal	.L.str.21@GOTOFF(%ebx), %eax
.LBB1_3:                                # %if.end8
	movl	%eax, (%esp)
	calll	printf@PLT
	movl	4(%esp), %eax
	addl	$8, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	size_t,@object                  # @size_t
	.bss
	.globl	size_t
	.p2align	2, 0x0
size_t:
	.long	0                               # 0x0
	.size	size_t, 4

	.type	T,@object                       # @T
	.globl	T
	.p2align	4, 0x0
T:
	.zero	28
	.size	T, 28

	.type	array1,@object                  # @array1
	.globl	array1
	.p2align	4, 0x0
array1:
	.zero	112
	.size	array1, 112

	.type	array2,@object                  # @array2
	.globl	array2
	.p2align	4, 0x0
array2:
	.zero	112
	.size	array2, 112

	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.0:
	.asciz	"{\n"
	.size	.L.str.0, 3

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"    .[%d] = {\n"
	.size	.L.str.1, 15

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"        .a = {\n"
	.size	.L.str.2, 16

	.type	.L.str.3,@object                # @.str.3
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.3:
	.asciz	"            .a = %f,\n"
	.size	.L.str.3, 22

	.type	.L.str.4,@object                # @.str.4
	.p2align	4, 0x0
.L.str.4:
	.asciz	"            .b = {\n"
	.size	.L.str.4, 20

	.type	.L.str.5,@object                # @.str.5
	.p2align	4, 0x0
.L.str.5:
	.asciz	"                .[0] = %d,\n"
	.size	.L.str.5, 28

	.type	.L.str.6,@object                # @.str.6
	.p2align	4, 0x0
.L.str.6:
	.asciz	"                .[1] = %d,\n"
	.size	.L.str.6, 28

	.type	.L.str.7,@object                # @.str.7
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.7:
	.asciz	"            },\n"
	.size	.L.str.7, 16

	.type	.L.str.8,@object                # @.str.8
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.8:
	.asciz	"            .c = {.a = %d},\n"
	.size	.L.str.8, 29

	.type	.L.str.9,@object                # @.str.9
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.9:
	.asciz	"        },\n"
	.size	.L.str.9, 12

	.type	.L.str.10,@object               # @.str.10
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.10:
	.asciz	"        .b1 = %d,\n"
	.size	.L.str.10, 19

	.type	.L.str.11,@object               # @.str.11
	.p2align	4, 0x0
.L.str.11:
	.asciz	"        .b2 = %d,\n"
	.size	.L.str.11, 19

	.type	.L.str.12,@object               # @.str.12
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.12:
	.asciz	"        .c = {\n"
	.size	.L.str.12, 16

	.type	.L.str.13,@object               # @.str.13
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.13:
	.asciz	"            .[0] = %d,\n"
	.size	.L.str.13, 24

	.type	.L.str.14,@object               # @.str.14
	.p2align	4, 0x0
.L.str.14:
	.asciz	"            .[1] = %d,\n"
	.size	.L.str.14, 24

	.type	.L.str.15,@object               # @.str.15
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.15:
	.asciz	"        },\n"
	.size	.L.str.15, 12

	.type	.L.str.16,@object               # @.str.16
.L.str.16:
	.asciz	"    },\n"
	.size	.L.str.16, 8

	.type	.L.str.17,@object               # @.str.17
.L.str.17:
	.asciz	"}\n"
	.size	.L.str.17, 3

	.type	.L.str.18,@object               # @.str.18
.L.str.18:
	.asciz	"array1:\n"
	.size	.L.str.18, 9

	.type	.L.str.19,@object               # @.str.19
.L.str.19:
	.asciz	"array2:\n"
	.size	.L.str.19, 9

	.type	.L.str.20,@object               # @.str.20
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.20:
	.asciz	"error: struct isn't correctly initialized\n"
	.size	.L.str.20, 43

	.type	.L.str.21,@object               # @.str.21
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.21:
	.asciz	"OK\n"
	.size	.L.str.21, 4

	.section	".note.GNU-stack","",@progbits

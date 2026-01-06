	.file	"cc1"
	.text
	.globl	f                               # -- Begin function f
	.p2align	4
	.type	f,@function
f:                                      # @f
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%eax
	.cfi_def_cfa_offset 8
	incl	b
	movl	f.a.0, %eax
	leal	1(%eax), %ecx
	movl	%ecx, f.a.0
	movl	%eax, (%esp)
	movl	(%esp), %eax
	popl	%ecx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	f, .Lfunc_end0-f
	.cfi_endproc
                                        # -- End function
	.globl	g                               # -- Begin function g
	.p2align	4
	.type	g,@function
g:                                      # @g
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%eax
	.cfi_def_cfa_offset 8
	incl	b
	movl	g.a.1, %eax
	leal	1(%eax), %ecx
	movl	%ecx, g.a.1
	movl	%eax, (%esp)
	movl	(%esp), %eax
	popl	%ecx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end1:
	.size	g, .Lfunc_end1-g
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subl	$12, %esp
	.cfi_def_cfa_offset 16
	movl	$0, 8(%esp)
	calll	f
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.0
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	f
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.1
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	f
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.2
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	g
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.3
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	g
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.4
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	g
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.5
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	h
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.6
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	h
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.7
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	calll	h
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.8
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$12, %esp
	.cfi_adjust_cfa_offset -12
	pushl	b
	.cfi_adjust_cfa_offset 4
	pushl	a
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.9
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	8(%esp), %eax
	addl	$12, %esp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc
                                        # -- End function
	.globl	h                               # -- Begin function h
	.p2align	4
	.type	h,@function
h:                                      # @h
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%eax
	.cfi_def_cfa_offset 8
	incl	b
	movl	h.a.2, %eax
	leal	1(%eax), %ecx
	movl	%ecx, h.a.2
	movl	%eax, (%esp)
	movl	(%esp), %eax
	popl	%ecx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end3:
	.size	h, .Lfunc_end3-h
	.cfi_endproc
                                        # -- End function
	.type	a,@object                       # @a
	.data
	.globl	a
	.p2align	2, 0x0
a:
	.long	17                              # 0x11
	.size	a, 4

	.type	b,@object                       # @b
	.bss
	.globl	b
	.p2align	2, 0x0
b:
	.long	0                               # 0x0
	.size	b, 4

	.type	f.a.0,@object                   # @f.a.0
	.data
	.p2align	2, 0x0
f.a.0:
	.long	42                              # 0x2a
	.size	f.a.0, 4

	.type	g.a.1,@object                   # @g.a.1
	.p2align	2, 0x0
g.a.1:
	.long	84                              # 0x54
	.size	g.a.1, 4

	.type	h.a.2,@object                   # @h.a.2
	.p2align	2, 0x0
h.a.2:
	.long	168                             # 0xa8
	.size	h.a.2, 4

	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.0:
	.asciz	"%d "
	.size	.L.str.0, 4

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"%d "
	.size	.L.str.1, 4

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"%d\n"
	.size	.L.str.2, 4

	.type	.L.str.3,@object                # @.str.3
.L.str.3:
	.asciz	"%d "
	.size	.L.str.3, 4

	.type	.L.str.4,@object                # @.str.4
.L.str.4:
	.asciz	"%d "
	.size	.L.str.4, 4

	.type	.L.str.5,@object                # @.str.5
.L.str.5:
	.asciz	"%d\n"
	.size	.L.str.5, 4

	.type	.L.str.6,@object                # @.str.6
.L.str.6:
	.asciz	"%d "
	.size	.L.str.6, 4

	.type	.L.str.7,@object                # @.str.7
.L.str.7:
	.asciz	"%d "
	.size	.L.str.7, 4

	.type	.L.str.8,@object                # @.str.8
.L.str.8:
	.asciz	"%d\n"
	.size	.L.str.8, 4

	.type	.L.str.9,@object                # @.str.9
.L.str.9:
	.asciz	"%d %d\n"
	.size	.L.str.9, 7

	.section	".note.GNU-stack","",@progbits

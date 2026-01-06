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
	subl	$24, %esp
	.cfi_def_cfa_offset 32
	.cfi_offset %ebx, -8
	calll	.L0$pb
	.cfi_adjust_cfa_offset 4
.L0$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp0:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp0-.L0$pb), %ebx
	movl	$0, 20(%esp)
	movl	$0, 16(%esp)
	.p2align	4
.LBB0_1:                                # %while.cond1
                                        # =>This Inner Loop Header: Depth=1
	movl	32(%esp), %eax
	cmpl	16(%esp), %eax
	je	.LBB0_3
# %bb.2:                                # %while.body2
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	36(%esp), %eax
	movl	16(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	%eax, (%esp)
	calll	puts@PLT
	incl	16(%esp)
	jmp	.LBB0_1
.LBB0_3:                                # %while.end3
	movl	$1, 20(%esp)
	movl	20(%esp), %eax
	addl	$24, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits

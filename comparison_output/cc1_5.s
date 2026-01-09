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
	movl	$0, 8(%esp)
	movl	$101, (%esp)
	calll	malloc@PLT
	movl	%eax, 12(%esp)
	testl	%eax, %eax
	jne	.LBB0_2
# %bb.1:                                # %if.then1
	movl	$1, 20(%esp)
	jmp	.LBB0_12
.LBB0_2:                                # %if.end2
	movl	12(%esp), %eax
	movb	$0, 100(%eax)
	cmpl	$99, 8(%esp)
	jg	.LBB0_5
	.p2align	4
.LBB0_4:                                # %while.body5
                                        # =>This Inner Loop Header: Depth=1
	movl	8(%esp), %eax
	leal	1(%eax), %ecx
	movl	%ecx, 8(%esp)
	movl	12(%esp), %ecx
	movb	$48, (%ecx,%eax)
	cmpl	$99, 8(%esp)
	jle	.LBB0_4
.LBB0_5:                                # %while.end6
	movl	$0, 8(%esp)
	cmpl	$49, 8(%esp)
	jg	.LBB0_8
	.p2align	4
.LBB0_7:                                # %while.body8
                                        # =>This Inner Loop Header: Depth=1
	movl	12(%esp), %eax
	movl	8(%esp), %ecx
	leal	1(%ecx), %edx
	movl	%edx, 8(%esp)
	incw	(%eax,%ecx,2)
	cmpl	$49, 8(%esp)
	jle	.LBB0_7
.LBB0_8:                                # %while.end9
	movl	12(%esp), %eax
	movl	%eax, 16(%esp)
	.p2align	4
.LBB0_9:                                # %while.cond10
                                        # =>This Inner Loop Header: Depth=1
	movl	12(%esp), %eax
	addl	$100, %eax
	cmpl	%eax, 16(%esp)
	jae	.LBB0_11
# %bb.10:                               # %while.body11
                                        #   in Loop: Header=BB0_9 Depth=1
	movl	16(%esp), %eax
	leal	4(%eax), %ecx
	movl	%ecx, 16(%esp)
	incl	(%eax)
	jmp	.LBB0_9
.LBB0_11:                               # %while.end12
	movl	12(%esp), %eax
	movb	$42, 5(%eax)
	movl	12(%esp), %eax
	movl	%eax, (%esp)
	calll	puts@PLT
.LBB0_12:                               # %return0
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

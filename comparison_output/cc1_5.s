	.file	"cc1"
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subl	$28, %esp
	.cfi_def_cfa_offset 32
	movl	$0, 24(%esp)
	movl	$0, 12(%esp)
	movl	$101, (%esp)
	calll	malloc@PLT
	movl	%eax, 16(%esp)
	testl	%eax, %eax
	jne	.LBB0_2
# %bb.1:                                # %if.then1
	movl	$1, 24(%esp)
	jmp	.LBB0_12
.LBB0_2:                                # %if.end2
	movl	16(%esp), %eax
	movl	$0, 100(%eax)
	cmpl	$99, 12(%esp)
	jg	.LBB0_5
	.p2align	4
.LBB0_4:                                # %while.body4
                                        # =>This Inner Loop Header: Depth=1
	movl	12(%esp), %eax
	leal	1(%eax), %ecx
	movl	%ecx, 12(%esp)
	movl	16(%esp), %ecx
	movb	$48, (%ecx,%eax)
	cmpl	$99, 12(%esp)
	jle	.LBB0_4
.LBB0_5:                                # %while.end5
	movl	$0, 12(%esp)
	cmpl	$49, 12(%esp)
	jg	.LBB0_8
	.p2align	4
.LBB0_7:                                # %while.body7
                                        # =>This Inner Loop Header: Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	leal	1(%ecx), %edx
	movl	%edx, 12(%esp)
	incw	(%eax,%ecx,2)
	cmpl	$49, 12(%esp)
	jle	.LBB0_7
.LBB0_8:                                # %while.end8
	movl	16(%esp), %eax
	movl	%eax, 20(%esp)
	.p2align	4
.LBB0_9:                                # %while.cond9
                                        # =>This Inner Loop Header: Depth=1
	movl	16(%esp), %eax
	addl	$100, %eax
	cmpl	%eax, 20(%esp)
	jae	.LBB0_11
# %bb.10:                               # %while.body10
                                        #   in Loop: Header=BB0_9 Depth=1
	movl	20(%esp), %eax
	leal	4(%eax), %ecx
	movl	%ecx, 20(%esp)
	incl	(%eax)
	jmp	.LBB0_9
.LBB0_11:                               # %while.end11
	movl	16(%esp), %eax
	movb	$42, 5(%eax)
	movl	16(%esp), %eax
	movl	%eax, (%esp)
	calll	puts@PLT
.LBB0_12:                               # %return0
	movl	24(%esp), %eax
	addl	$28, %esp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits

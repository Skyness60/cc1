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
	cmpl	$2, 16(%esp)
	movb	$1, %al
	movb	$1, %cl
	je	.LBB0_4
# %bb.1:                                # %land.end6
	testb	%cl, %cl
	je	.LBB0_5
.LBB0_2:                                # %land.end4
	testb	%al, %al
	jne	.LBB0_3
	jmp	.LBB0_6
.LBB0_4:                                # %land.rhs5
	movl	20(%esp), %ecx
	movl	4(%ecx), %ecx
	cmpb	$0, (%ecx)
	sete	%cl
	testb	%cl, %cl
	jne	.LBB0_2
.LBB0_5:                                # %land.rhs3
	movl	20(%esp), %eax
	movl	4(%eax), %eax
	cmpb	$0, 1(%eax)
	setne	%al
	testb	%al, %al
	jne	.LBB0_3
.LBB0_6:                                # %if.end2
	movl	20(%esp), %eax
	movl	4(%eax), %eax
	movsbl	(%eax), %eax
	movb	%al, (%esp)
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	isalnum@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testl	%eax, %eax
	jne	.LBB0_8
.LBB0_3:                                # %if.then1
	movl	$1, 4(%esp)
.LBB0_15:                               # %return0
	movl	4(%esp), %eax
	addl	$8, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.LBB0_8:                                # %if.end13
	.cfi_def_cfa_offset 16
	btl	$6, (%esp)
	jae	.LBB0_11
# %bb.9:                                # %if.then15
	leal	.L.str.0@GOTOFF(%ebx), %eax
	movzbl	(%esp), %edx
	movl	%edx, %ecx
	andl	$31, %ecx
	decl	%ecx
	btl	$5, %edx
	jae	.LBB0_12
# %bb.10:                               # %cond.true18
	leal	.L.str.1@GOTOFF(%ebx), %edx
	jmp	.LBB0_13
.LBB0_11:                               # %if.else16
	leal	.L.str.3@GOTOFF(%ebx), %eax
	movzbl	(%esp), %ecx
	andl	$15, %ecx
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	jmp	.LBB0_14
.LBB0_12:                               # %cond.false19
	.cfi_def_cfa_offset 16
	leal	.L.str.2@GOTOFF(%ebx), %edx
.LBB0_13:                               # %cond.end20
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
.LBB0_14:                               # %return0
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	jmp	.LBB0_15
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.0:
	.asciz	"alpha:\nposition: %d\n%s\n"
	.size	.L.str.0, 24

	.type	.L.str.1,@object                # @.str.1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.1:
	.asciz	"lowercase"
	.size	.L.str.1, 10

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"uppercase"
	.size	.L.str.2, 10

	.type	.L.str.3,@object                # @.str.3
.L.str.3:
	.asciz	"digit:\nn: %d\n"
	.size	.L.str.3, 14

	.section	".note.GNU-stack","",@progbits

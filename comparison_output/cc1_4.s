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
	movl	36(%esp), %eax
	movl	4(%eax), %eax
	movl	%eax, 20(%esp)
	movl	$0, 16(%esp)
	cmpl	$2, 32(%esp)
	je	.LBB0_3
# %bb.1:                                # %if.then1
	movl	$1, 24(%esp)
.LBB0_2:                                # %return0
	movl	24(%esp), %eax
	addl	$28, %esp
	.cfi_def_cfa_offset 4
	retl
	.p2align	4
.LBB0_5:                                # %if.then6
                                        #   in Loop: Header=BB0_3 Depth=1
	.cfi_def_cfa_offset 32
	movl	20(%esp), %eax
	movl	16(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	$.L.str.1, (%esp)
.LBB0_6:                                # %while.cond3
                                        #   in Loop: Header=BB0_3 Depth=1
	calll	printf@PLT
	incl	16(%esp)
.LBB0_3:                                # %while.cond3
                                        # =>This Inner Loop Header: Depth=1
	movl	20(%esp), %eax
	movl	16(%esp), %ecx
	cmpb	$0, (%eax,%ecx)
	je	.LBB0_2
# %bb.4:                                # %while.body4
                                        #   in Loop: Header=BB0_3 Depth=1
	movl	20(%esp), %eax
	movl	16(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	$.L.str.0, (%esp)
	calll	strchr@PLT
	testl	%eax, %eax
	jne	.LBB0_5
# %bb.7:                                # %if.else7
                                        #   in Loop: Header=BB0_3 Depth=1
	movl	20(%esp), %eax
	movl	16(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	%eax, (%esp)
	calll	isalpha@PLT
	testl	%eax, %eax
	je	.LBB0_9
# %bb.8:                                # %if.then9
                                        #   in Loop: Header=BB0_3 Depth=1
	movl	20(%esp), %eax
	movl	16(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	$.L.str.2, (%esp)
	jmp	.LBB0_6
.LBB0_9:                                # %if.else10
                                        #   in Loop: Header=BB0_3 Depth=1
	movl	20(%esp), %eax
	movl	16(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	$.L.str.3, (%esp)
	jmp	.LBB0_6
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.0:
	.asciz	"aeiouyAEIOUY"
	.size	.L.str.0, 13

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"%c: vowel\n"
	.size	.L.str.1, 11

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"%c: consonant\n"
	.size	.L.str.2, 15

	.type	.L.str.3,@object                # @.str.3
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.3:
	.asciz	"%c: not a letter\n"
	.size	.L.str.3, 18

	.section	".note.GNU-stack","",@progbits

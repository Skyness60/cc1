	.file	"cc1"
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
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
	subl	$28, %esp
	.cfi_def_cfa_offset 48
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
	movl	$0, 20(%esp)
	movl	52(%esp), %eax
	movl	4(%eax), %eax
	movl	%eax, 16(%esp)
	movl	$0, 12(%esp)
	cmpl	$2, 48(%esp)
	je	.LBB0_3
# %bb.1:                                # %if.then1
	movl	$1, 20(%esp)
.LBB0_2:                                # %return0
	movl	20(%esp), %eax
	addl	$28, %esp
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
.LBB0_3:                                # %if.end2
	.cfi_def_cfa_offset 48
	leal	.L.str.0@GOTOFF(%ebx), %esi
	leal	.L.str.1@GOTOFF(%ebx), %edi
	leal	.L.str.2@GOTOFF(%ebx), %ebp
	leal	.L.str.3@GOTOFF(%ebx), %eax
	movl	%eax, 24(%esp)                  # 4-byte Spill
	jmp	.LBB0_4
	.p2align	4
.LBB0_6:                                # %if.then7
                                        #   in Loop: Header=BB0_4 Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	movsbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	%edi, (%esp)
.LBB0_7:                                # %while.cond4
                                        #   in Loop: Header=BB0_4 Depth=1
	calll	printf@PLT
	incl	12(%esp)
.LBB0_4:                                # %while.cond4
                                        # =>This Inner Loop Header: Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	cmpb	$0, (%eax,%ecx)
	je	.LBB0_2
# %bb.5:                                # %while.body5
                                        #   in Loop: Header=BB0_4 Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	movsbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	%esi, (%esp)
	calll	strchr@PLT
	testl	%eax, %eax
	jne	.LBB0_6
# %bb.8:                                # %if.else8
                                        #   in Loop: Header=BB0_4 Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	movsbl	(%eax,%ecx), %eax
	movl	%eax, (%esp)
	calll	isalpha@PLT
	testl	%eax, %eax
	je	.LBB0_10
# %bb.9:                                # %if.then10
                                        #   in Loop: Header=BB0_4 Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	movsbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	%ebp, (%esp)
	jmp	.LBB0_7
.LBB0_10:                               # %if.else11
                                        #   in Loop: Header=BB0_4 Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	movsbl	(%eax,%ecx), %eax
	movl	%eax, 4(%esp)
	movl	24(%esp), %eax                  # 4-byte Reload
	movl	%eax, (%esp)
	jmp	.LBB0_7
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

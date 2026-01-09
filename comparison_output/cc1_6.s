	.file	"cc1"
	.text
	.globl	ft_strlen                       # -- Begin function ft_strlen
	.p2align	4
	.type	ft_strlen,@function
ft_strlen:                              # @ft_strlen
	.cfi_startproc
# %bb.0:                                # %entry
	subl	$8, %esp
	.cfi_def_cfa_offset 12
	movl	$0, (%esp)
	.p2align	4
.LBB0_1:                                # %while.cond1
                                        # =>This Inner Loop Header: Depth=1
	movl	12(%esp), %eax
	movl	(%esp), %ecx
	cmpb	$0, (%eax,%ecx)
	je	.LBB0_3
# %bb.2:                                # %while.body2
                                        #   in Loop: Header=BB0_1 Depth=1
	incl	(%esp)
	jmp	.LBB0_1
.LBB0_3:                                # %while.end3
	movl	(%esp), %eax
	movl	%eax, 4(%esp)
	movl	4(%esp), %eax
	addl	$8, %esp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	ft_strlen, .Lfunc_end0-ft_strlen
	.cfi_endproc
                                        # -- End function
	.globl	ft_memcpy                       # -- Begin function ft_memcpy
	.p2align	4
	.type	ft_memcpy,@function
ft_memcpy:                              # @ft_memcpy
	.cfi_startproc
# %bb.0:                                # %entry
	subl	$8, %esp
	.cfi_def_cfa_offset 12
	cmpl	$0, 12(%esp)
	movb	$1, %al
	jne	.LBB1_1
# %bb.2:                                # %land.end9
	testb	%al, %al
	je	.LBB1_4
.LBB1_3:                                # %if.then6
	movl	$0, 4(%esp)
	jmp	.LBB1_11
.LBB1_1:                                # %land.rhs8
	cmpl	$0, 16(%esp)
	sete	%al
	testb	%al, %al
	jne	.LBB1_3
.LBB1_4:                                # %if.end7
	movl	$0, (%esp)
	.p2align	4
.LBB1_5:                                # %for.cond13
                                        # =>This Inner Loop Header: Depth=1
	movl	20(%esp), %eax
	leal	3(%eax), %ecx
	testl	%eax, %eax
	cmovnsl	%eax, %ecx
	sarl	$2, %ecx
	cmpl	%ecx, (%esp)
	jge	.LBB1_7
# %bb.6:                                # %for.body14
                                        #   in Loop: Header=BB1_5 Depth=1
	movl	16(%esp), %eax
	movl	(%esp), %ecx
	movl	(%eax,%ecx,4), %eax
	movl	12(%esp), %edx
	movl	%eax, (%edx,%ecx,4)
	incl	(%esp)
	jmp	.LBB1_5
.LBB1_7:                                # %for.end16
	shll	$2, (%esp)
	.p2align	4
.LBB1_8:                                # %for.cond17
                                        # =>This Inner Loop Header: Depth=1
	movl	(%esp), %eax
	cmpl	20(%esp), %eax
	jge	.LBB1_10
# %bb.9:                                # %for.body18
                                        #   in Loop: Header=BB1_8 Depth=1
	movl	16(%esp), %eax
	movl	(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	12(%esp), %edx
	movb	%al, (%edx,%ecx)
	incl	(%esp)
	jmp	.LBB1_8
.LBB1_10:                               # %for.end20
	movl	12(%esp), %eax
	movl	%eax, 4(%esp)
.LBB1_11:                               # %return5
	movl	4(%esp), %eax
	addl	$8, %esp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end1:
	.size	ft_memcpy, .Lfunc_end1-ft_memcpy
	.cfi_endproc
                                        # -- End function
	.globl	ft_strdup                       # -- Begin function ft_strdup
	.p2align	4
	.type	ft_strdup,@function
ft_strdup:                              # @ft_strdup
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	subl	$24, %esp
	.cfi_def_cfa_offset 32
	.cfi_offset %ebx, -8
	calll	.L2$pb
	.cfi_adjust_cfa_offset 4
.L2$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp0:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp0-.L2$pb), %ebx
	movl	32(%esp), %eax
	movl	%eax, (%esp)
	calll	ft_strlen
	movl	%eax, 16(%esp)
	incl	%eax
	movl	%eax, (%esp)
	calll	malloc@PLT
	movl	%eax, 12(%esp)
	testl	%eax, %eax
	je	.LBB2_2
# %bb.1:                                # %if.then23
	movl	12(%esp), %eax
	movl	32(%esp), %ecx
	movl	16(%esp), %edx
	incl	%edx
	movl	%edx, 8(%esp)
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	calll	ft_memcpy
	movl	%eax, 12(%esp)
.LBB2_2:                                # %if.end24
	movl	12(%esp), %eax
	movl	%eax, 20(%esp)
	movl	20(%esp), %eax
	addl	$24, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end2:
	.size	ft_strdup, .Lfunc_end2-ft_strdup
	.cfi_endproc
                                        # -- End function
	.globl	ft_strmapi                      # -- Begin function ft_strmapi
	.p2align	4
	.type	ft_strmapi,@function
ft_strmapi:                             # @ft_strmapi
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	subl	$24, %esp
	.cfi_def_cfa_offset 32
	.cfi_offset %ebx, -8
	calll	.L3$pb
	.cfi_adjust_cfa_offset 4
.L3$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp1:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp1-.L3$pb), %ebx
	movl	$0, 12(%esp)
	cmpl	$0, 32(%esp)
	movb	$1, %al
	jne	.LBB3_1
# %bb.2:                                # %land.end30
	testb	%al, %al
	jne	.LBB3_6
	jmp	.LBB3_3
.LBB3_1:                                # %land.rhs29
	cmpl	$0, 36(%esp)
	sete	%al
	testb	%al, %al
	jne	.LBB3_6
.LBB3_3:                                # %if.end28
	movl	$-1, 16(%esp)
	movl	32(%esp), %eax
	movl	%eax, (%esp)
	calll	ft_strdup
	movl	%eax, 12(%esp)
	testl	%eax, %eax
	je	.LBB3_6
	jmp	.LBB3_4
	.p2align	4
.LBB3_5:                                # %if.else51
                                        #   in Loop: Header=BB3_4 Depth=1
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	movzbl	(%ecx,%eax), %ecx
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	calll	*36(%esp)
	movl	12(%esp), %ecx
	movl	16(%esp), %edx
	movb	%al, (%ecx,%edx)
.LBB3_4:                                # %for.cond43
                                        # =>This Inner Loop Header: Depth=1
	movl	16(%esp), %eax
	leal	1(%eax), %ecx
	movl	%ecx, 16(%esp)
	movl	12(%esp), %ecx
	cmpb	$0, 1(%ecx,%eax)
	jne	.LBB3_5
.LBB3_6:                                # %label.end33
	movl	12(%esp), %eax
	movl	%eax, 20(%esp)
	addl	$24, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end3:
	.size	ft_strmapi, .Lfunc_end3-ft_strmapi
	.cfi_endproc
                                        # -- End function
	.globl	factorial                       # -- Begin function factorial
	.p2align	4
	.type	factorial,@function
factorial:                              # @factorial
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	pushl	%esi
	.cfi_def_cfa_offset 12
	subl	$20, %esp
	.cfi_def_cfa_offset 32
	.cfi_offset %esi, -12
	.cfi_offset %ebx, -8
	calll	.L4$pb
	.cfi_adjust_cfa_offset 4
.L4$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp2:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp2-.L4$pb), %ebx
	cmpl	$0, 32(%esp)
	je	.LBB4_2
# %bb.1:                                # %if.then57
	movl	32(%esp), %esi
	leal	-1(%esi), %eax
	movl	%eax, (%esp)
	calll	factorial
	imull	%esi, %eax
	movl	%eax, 16(%esp)
	jmp	.LBB4_3
.LBB4_2:                                # %if.end58
	movl	$1, 16(%esp)
.LBB4_3:                                # %return56
	movl	16(%esp), %eax
	addl	$20, %esp
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end4:
	.size	factorial, .Lfunc_end4-factorial
	.cfi_endproc
                                        # -- End function
	.globl	ft_isalpha                      # -- Begin function ft_isalpha
	.p2align	4
	.type	ft_isalpha,@function
ft_isalpha:                             # @ft_isalpha
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%eax
	.cfi_def_cfa_offset 8
	movl	8(%esp), %eax
	orl	$32, %eax
	movl	%eax, 8(%esp)
	cmpl	$97, %eax
	jl	.LBB5_1
# %bb.2:                                # %land.rhs62
	cmpl	$123, 8(%esp)
	setl	%al
	jmp	.LBB5_3
.LBB5_1:
	xorl	%eax, %eax
.LBB5_3:                                # %land.end63
	movzbl	%al, %eax
	movl	%eax, (%esp)
	popl	%ecx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end5:
	.size	ft_isalpha, .Lfunc_end5-ft_isalpha
	.cfi_endproc
                                        # -- End function
	.globl	rot                             # -- Begin function rot
	.p2align	4
	.type	rot,@function
rot:                                    # @rot
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	pushl	%esi
	.cfi_def_cfa_offset 12
	subl	$20, %esp
	.cfi_def_cfa_offset 32
	.cfi_offset %esi, -12
	.cfi_offset %ebx, -8
	calll	.L6$pb
	.cfi_adjust_cfa_offset 4
.L6$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp3:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp3-.L6$pb), %ebx
	movl	36(%esp), %eax
	movl	%eax, (%esp)
	calll	ft_isalpha
	testl	%eax, %eax
	jne	.LBB6_2
# %bb.1:                                # %if.then68
	movzbl	36(%esp), %eax
	movb	%al, 19(%esp)
	jmp	.LBB6_6
.LBB6_2:                                # %if.end69
	cmpb	$90, 36(%esp)
	jg	.LBB6_4
# %bb.3:                                # %if.then71
	movsbl	36(%esp), %esi
	movl	32(%esp), %ecx
	movl	$1717986919, %edx               # imm = 0x66666667
	movl	%ecx, %eax
	imull	%edx
	movl	%edx, %eax
	shrl	$31, %eax
	sarl	$2, %edx
	addl	%eax, %edx
	addl	%edx, %edx
	leal	(%edx,%edx,4), %eax
	subl	%eax, %ecx
	movl	%ecx, (%esp)
	calll	factorial
	movl	%eax, %ecx
	addl	%esi, %ecx
	movl	$1717986919, %eax               # imm = 0x66666667
	imull	32(%esp)
	movl	%edx, %eax
	shrl	$31, %eax
	sarl	$2, %edx
	addl	%eax, %edx
	leal	-65(%ecx,%edx), %ecx
	movl	$1321528399, %edx               # imm = 0x4EC4EC4F
	movl	%ecx, %eax
	imull	%edx
	movl	%edx, %eax
	shrl	$31, %eax
	shrl	$3, %edx
	addl	%eax, %edx
	leal	(%edx,%edx,4), %eax
	leal	(%eax,%eax,4), %eax
	addl	%edx, %eax
	subl	%eax, %ecx
	addb	$65, %cl
	jmp	.LBB6_5
.LBB6_4:                                # %if.else72
	movsbl	36(%esp), %esi
	movl	32(%esp), %ecx
	movl	$1717986919, %edx               # imm = 0x66666667
	movl	%ecx, %eax
	imull	%edx
	movl	%edx, %eax
	shrl	$31, %eax
	sarl	$2, %edx
	addl	%eax, %edx
	addl	%edx, %edx
	leal	(%edx,%edx,4), %eax
	subl	%eax, %ecx
	movl	%ecx, (%esp)
	calll	factorial
	movl	%eax, %ecx
	addl	%esi, %ecx
	movl	$1717986919, %eax               # imm = 0x66666667
	imull	32(%esp)
	movl	%edx, %eax
	shrl	$31, %eax
	sarl	$2, %edx
	addl	%eax, %edx
	leal	-97(%ecx,%edx), %ecx
	movl	$1321528399, %edx               # imm = 0x4EC4EC4F
	movl	%ecx, %eax
	imull	%edx
	movl	%edx, %eax
	shrl	$31, %eax
	shrl	$3, %edx
	addl	%eax, %edx
	leal	(%edx,%edx,4), %eax
	leal	(%eax,%eax,4), %eax
	addl	%edx, %eax
	subl	%eax, %ecx
	addb	$97, %cl
.LBB6_5:                                # %return67
	movb	%cl, 19(%esp)
.LBB6_6:                                # %return67
	movzbl	19(%esp), %eax
	addl	$20, %esp
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end6:
	.size	rot, .Lfunc_end6-rot
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register %ebp
	pushl	%ebx
	pushl	%edi
	pushl	%esi
	subl	$12, %esp
	.cfi_offset %esi, -20
	.cfi_offset %edi, -16
	.cfi_offset %ebx, -12
	calll	.L7$pb
.L7$pb:
	popl	%ebx
.Ltmp4:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp4-.L7$pb), %ebx
	movl	$0, -20(%ebp)
	movl	$1, -16(%ebp)
	leal	rot@GOTOFF(%ebx), %esi
	.p2align	4
.LBB7_1:                                # %while.cond77
                                        # =>This Inner Loop Header: Depth=1
	movl	-16(%ebp), %eax
	cmpl	8(%ebp), %eax
	jge	.LBB7_3
# %bb.2:                                # %while.body78
                                        #   in Loop: Header=BB7_1 Depth=1
	movl	%esp, %edi
	leal	-16(%edi), %esp
	movl	12(%ebp), %eax
	movl	-16(%ebp), %ecx
	subl	$8, %esp
	pushl	%esi
	pushl	(%eax,%ecx,4)
	calll	ft_strmapi
	addl	$16, %esp
	movl	%eax, -16(%edi)
	subl	$12, %esp
	pushl	%eax
	calll	puts@PLT
	addl	$4, %esp
	pushl	-16(%edi)
	calll	free@PLT
	addl	$16, %esp
	incl	-16(%ebp)
	jmp	.LBB7_1
.LBB7_3:                                # %while.end79
	movl	-20(%ebp), %eax
	leal	-12(%ebp), %esp
	popl	%esi
	popl	%edi
	popl	%ebx
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end7:
	.size	main, .Lfunc_end7-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits

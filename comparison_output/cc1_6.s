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
# %bb.2:                                # %land.end8
	testb	%al, %al
	je	.LBB1_4
.LBB1_3:                                # %if.then5
	movl	$0, 4(%esp)
	jmp	.LBB1_11
.LBB1_1:                                # %land.rhs7
	cmpl	$0, 16(%esp)
	sete	%al
	testb	%al, %al
	jne	.LBB1_3
.LBB1_4:                                # %if.end6
	movl	$0, (%esp)
	.p2align	4
.LBB1_5:                                # %for.cond11
                                        # =>This Inner Loop Header: Depth=1
	movl	20(%esp), %eax
	movl	%eax, %ecx
	sarl	$31, %ecx
	shrl	$30, %ecx
	addl	%eax, %ecx
	sarl	$2, %ecx
	cmpl	%ecx, (%esp)
	jge	.LBB1_7
# %bb.6:                                # %for.body12
                                        #   in Loop: Header=BB1_5 Depth=1
	movl	16(%esp), %eax
	movl	(%esp), %ecx
	movl	(%eax,%ecx,4), %eax
	movl	12(%esp), %edx
	movl	%eax, (%edx,%ecx,4)
	incl	(%esp)
	jmp	.LBB1_5
.LBB1_7:                                # %for.end14
	shll	$2, (%esp)
	.p2align	4
.LBB1_8:                                # %for.cond15
                                        # =>This Inner Loop Header: Depth=1
	movl	(%esp), %eax
	cmpl	20(%esp), %eax
	jge	.LBB1_10
# %bb.9:                                # %for.body16
                                        #   in Loop: Header=BB1_8 Depth=1
	movl	16(%esp), %eax
	movl	(%esp), %ecx
	movzbl	(%eax,%ecx), %eax
	movl	12(%esp), %edx
	movb	%al, (%edx,%ecx)
	incl	(%esp)
	jmp	.LBB1_8
.LBB1_10:                               # %for.end18
	movl	12(%esp), %eax
	movl	%eax, 4(%esp)
.LBB1_11:                               # %return4
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
	subl	$28, %esp
	.cfi_def_cfa_offset 32
	movl	32(%esp), %eax
	movl	%eax, (%esp)
	calll	ft_strlen
	movl	%eax, 20(%esp)
	incl	%eax
	movl	%eax, (%esp)
	calll	malloc@PLT
	movl	%eax, 16(%esp)
	testl	%eax, %eax
	je	.LBB2_2
# %bb.1:                                # %if.then20
	movl	16(%esp), %eax
	movl	32(%esp), %ecx
	movl	20(%esp), %edx
	incl	%edx
	movl	%edx, 8(%esp)
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	calll	ft_memcpy
	movl	%eax, 16(%esp)
.LBB2_2:                                # %if.end21
	movl	16(%esp), %eax
	movl	%eax, 24(%esp)
	movl	24(%esp), %eax
	addl	$28, %esp
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
	subl	$28, %esp
	.cfi_def_cfa_offset 32
	movl	$0, 16(%esp)
	cmpl	$0, 32(%esp)
	movb	$1, %al
	jne	.LBB3_1
# %bb.2:                                # %land.end26
	testb	%al, %al
	jne	.LBB3_6
	jmp	.LBB3_3
.LBB3_1:                                # %land.rhs25
	cmpl	$0, 36(%esp)
	sete	%al
	testb	%al, %al
	jne	.LBB3_6
.LBB3_3:                                # %if.end24
	movl	$-1, 20(%esp)
	movl	32(%esp), %eax
	movl	%eax, (%esp)
	calll	ft_strdup
	movl	%eax, 16(%esp)
	testl	%eax, %eax
	je	.LBB3_6
	jmp	.LBB3_4
	.p2align	4
.LBB3_5:                                # %if.else41
                                        #   in Loop: Header=BB3_4 Depth=1
	movl	20(%esp), %eax
	movl	16(%esp), %ecx
	movzbl	(%ecx,%eax), %ecx
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	calll	*36(%esp)
	movl	16(%esp), %ecx
	movl	20(%esp), %edx
	movl	%eax, (%ecx,%edx)
.LBB3_4:                                # %for.cond35
                                        # =>This Inner Loop Header: Depth=1
	movl	20(%esp), %eax
	leal	1(%eax), %ecx
	movl	%ecx, 20(%esp)
	movl	16(%esp), %ecx
	cmpb	$0, 1(%ecx,%eax)
	jne	.LBB3_5
.LBB3_6:                                # %label.end29
	movl	16(%esp), %eax
	movl	%eax, 24(%esp)
	addl	$28, %esp
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
	subl	$12, %esp
	.cfi_def_cfa_offset 16
	cmpl	$0, 16(%esp)
	je	.LBB4_2
# %bb.1:                                # %if.then44
	movl	16(%esp), %eax
	decl	%eax
	movl	%eax, (%esp)
	calll	factorial
	imull	16(%esp), %eax
	movl	%eax, 8(%esp)
	jmp	.LBB4_3
.LBB4_2:                                # %if.end45
	movl	$1, 8(%esp)
.LBB4_3:                                # %return43
	movl	8(%esp), %eax
	addl	$12, %esp
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
# %bb.2:                                # %land.rhs47
	cmpl	$123, 8(%esp)
	setl	%al
	jmp	.LBB5_3
.LBB5_1:
	xorl	%eax, %eax
.LBB5_3:                                # %land.end48
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
	pushl	%esi
	.cfi_def_cfa_offset 8
	subl	$8, %esp
	.cfi_def_cfa_offset 16
	.cfi_offset %esi, -8
	movl	20(%esp), %eax
	movl	%eax, (%esp)
	calll	ft_isalpha
	testl	%eax, %eax
	jne	.LBB6_2
# %bb.1:                                # %if.then52
	movzbl	20(%esp), %eax
	movb	%al, 7(%esp)
	jmp	.LBB6_6
.LBB6_2:                                # %if.end53
	cmpb	$90, 20(%esp)
	jg	.LBB6_4
# %bb.3:                                # %if.then54
	movsbl	20(%esp), %esi
	movl	16(%esp), %ecx
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
	imull	16(%esp)
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
.LBB6_4:                                # %if.else55
	movsbl	20(%esp), %esi
	movl	16(%esp), %ecx
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
	imull	16(%esp)
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
.LBB6_5:                                # %return51
	movb	%cl, 7(%esp)
.LBB6_6:                                # %return51
	movzbl	7(%esp), %eax
	addl	$8, %esp
	.cfi_def_cfa_offset 8
	popl	%esi
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
	pushl	%esi
	subl	$20, %esp
	.cfi_offset %esi, -12
	movl	$0, -12(%ebp)
	movl	$1, -8(%ebp)
	.p2align	4
.LBB7_1:                                # %while.cond58
                                        # =>This Inner Loop Header: Depth=1
	movl	-8(%ebp), %eax
	cmpl	8(%ebp), %eax
	jge	.LBB7_3
# %bb.2:                                # %while.body59
                                        #   in Loop: Header=BB7_1 Depth=1
	movl	%esp, %esi
	leal	-16(%esi), %esp
	movl	12(%ebp), %eax
	movl	-8(%ebp), %ecx
	subl	$8, %esp
	pushl	$rot
	pushl	(%eax,%ecx,4)
	calll	ft_strmapi
	addl	$16, %esp
	movl	%eax, -16(%esi)
	subl	$12, %esp
	pushl	%eax
	calll	puts@PLT
	addl	$4, %esp
	pushl	-16(%esi)
	calll	free@PLT
	addl	$16, %esp
	incl	-8(%ebp)
	jmp	.LBB7_1
.LBB7_3:                                # %while.end60
	movl	-12(%ebp), %eax
	leal	-4(%ebp), %esp
	popl	%esi
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end7:
	.size	main, .Lfunc_end7-main
	.cfi_endproc
                                        # -- End function
	.type	size_t,@object                  # @size_t
	.bss
	.globl	size_t
	.p2align	2, 0x0
size_t:
	.long	0                               # 0x0
	.size	size_t, 4

	.section	".note.GNU-stack","",@progbits

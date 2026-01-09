	.file	"cc1"
	.text
	.globl	add_char                        # -- Begin function add_char
	.p2align	4
	.type	add_char,@function
add_char:                               # @add_char
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
	calll	.L0$pb
.L0$pb:
	popl	%ebx
.Ltmp0:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp0-.L0$pb), %ebx
	cmpl	$0, 8(%ebp)
	jne	.LBB0_2
# %bb.1:                                # %if.then1
	movl	12(%ebp), %eax
	movl	$1, (%eax)
	movl	12(%ebp), %eax
	subl	$8, %esp
	pushl	$1
	pushl	(%eax)
	calll	calloc@PLT
	addl	$16, %esp
	movl	%eax, 8(%ebp)
	movzbl	16(%ebp), %ecx
	movb	%cl, (%eax)
	jmp	.LBB0_5
.LBB0_2:                                # %if.else2
	movl	%esp, %edi
	leal	-16(%edi), %esi
	movl	%esi, %esp
	subl	$12, %esp
	pushl	8(%ebp)
	calll	strlen@PLT
	addl	$16, %esp
	movl	%eax, -16(%edi)
	movl	12(%ebp), %ecx
	cmpl	(%ecx), %eax
	jl	.LBB0_4
# %bb.3:                                # %if.then4
	movl	12(%ebp), %eax
	shll	(%eax)
	movl	12(%ebp), %eax
	subl	$8, %esp
	pushl	(%eax)
	pushl	8(%ebp)
	calll	realloc@PLT
	addl	$16, %esp
	movl	%eax, 8(%ebp)
	movl	(%esi), %ecx
	addl	%ecx, %eax
	movl	12(%ebp), %edx
	movl	(%edx), %edx
	subl	%ecx, %edx
	subl	$4, %esp
	pushl	%edx
	pushl	$0
	pushl	%eax
	calll	memset@PLT
	addl	$16, %esp
.LBB0_4:                                # %if.end5
	movzbl	16(%ebp), %eax
	movl	8(%ebp), %ecx
	movl	(%esi), %edx
	movb	%al, (%ecx,%edx)
.LBB0_5:                                # %if.end3
	movl	8(%ebp), %eax
	movl	%eax, -16(%ebp)
	leal	-12(%ebp), %esp
	popl	%esi
	popl	%edi
	popl	%ebx
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end0:
	.size	add_char, .Lfunc_end0-add_char
	.cfi_endproc
                                        # -- End function
	.globl	add_unicode                     # -- Begin function add_unicode
	.p2align	4
	.type	add_unicode,@function
add_unicode:                            # @add_unicode
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
	calll	.L1$pb
	.cfi_adjust_cfa_offset 4
.L1$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp1:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp1-.L1$pb), %ebx
	movswl	40(%esp), %eax
	cmpl	$127, %eax
	jg	.LBB1_2
# %bb.1:                                # %if.then8
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	44(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	44(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	44(%esp)
	.cfi_adjust_cfa_offset 4
	calll	add_char
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	jmp	.LBB1_9
.LBB1_2:                                # %if.end9
	movl	$1, 12(%esp)
	.p2align	4
.LBB1_3:                                # %for.cond11
                                        # =>This Inner Loop Header: Depth=1
	movswl	40(%esp), %eax
	movl	12(%esp), %edx
	leal	(%edx,%edx), %ecx
	leal	(%ecx,%ecx,2), %ecx
	subl	%edx, %ecx
	incb	%cl
                                        # kill: def $cl killed $cl killed $ecx
	sarl	%cl, %eax
	testl	%eax, %eax
	je	.LBB1_5
# %bb.4:                                # %for.body12
                                        #   in Loop: Header=BB1_3 Depth=1
	incl	12(%esp)
	jmp	.LBB1_3
.LBB1_5:                                # %for.end14
	movl	12(%esp), %eax
	movb	$8, %cl
	subb	%al, %cl
	movl	$255, %esi
	shll	%cl, %esi
	movswl	40(%esp), %edx
	addl	%eax, %eax
	leal	(%eax,%eax,2), %ecx
	addb	$-6, %cl
                                        # kill: def $cl killed $cl killed $ecx
	sarl	%cl, %edx
	orl	%esi, %edx
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	.p2align	4
.LBB1_6:                                # %for.cond15
                                        # =>This Inner Loop Header: Depth=1
	pushl	44(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	44(%esp)
	.cfi_adjust_cfa_offset 4
	calll	add_char
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	%eax, 32(%esp)
	decl	12(%esp)
	cmpl	$0, 12(%esp)
	jle	.LBB1_8
# %bb.7:                                # %for.body16
                                        #   in Loop: Header=BB1_6 Depth=1
	movswl	40(%esp), %eax
	movl	12(%esp), %ecx
	addl	%ecx, %ecx
	leal	(%ecx,%ecx,2), %ecx
	addb	$-6, %cl
                                        # kill: def $cl killed $cl killed $ecx
	sarl	%cl, %eax
	andl	$63, %eax
	orl	$32640, %eax                    # imm = 0x7F80
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	jmp	.LBB1_6
.LBB1_8:                                # %for.end18
	.cfi_def_cfa_offset 32
	movl	32(%esp), %eax
.LBB1_9:                                # %return7
	movl	%eax, 16(%esp)
	movl	16(%esp), %eax
	addl	$20, %esp
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end1:
	.size	add_unicode, .Lfunc_end1-add_unicode
	.cfi_endproc
                                        # -- End function
	.globl	parse_str                       # -- Begin function parse_str
	.p2align	4
	.type	parse_str,@function
parse_str:                              # @parse_str
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
	subl	$28, %esp
	.cfi_offset %esi, -20
	.cfi_offset %edi, -16
	.cfi_offset %ebx, -12
	calll	.L2$pb
.L2$pb:
	popl	%ebx
.Ltmp2:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp2-.L2$pb), %ebx
	movl	$0, -20(%ebp)
	movl	$0, -24(%ebp)
	.p2align	4
.LBB2_1:                                # %while.cond21
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB2_16 Depth 2
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB2_37
# %bb.2:                                # %while.body22
                                        #   in Loop: Header=BB2_1 Depth=1
	calll	getchar@PLT
	movb	%al, -13(%ebp)
	cmpl	$92, %eax
	je	.LBB2_6
# %bb.3:                                # %while.body22
                                        #   in Loop: Header=BB2_1 Depth=1
	cmpl	$-1, %eax
	je	.LBB2_37
# %bb.4:                                # %while.body22
                                        #   in Loop: Header=BB2_1 Depth=1
	cmpl	$34, %eax
	jne	.LBB2_36
	jmp	.LBB2_5
	.p2align	4
.LBB2_6:                                # %sw.case26
                                        #   in Loop: Header=BB2_1 Depth=1
	calll	getchar@PLT
	movb	%al, -13(%ebp)
	addl	$-34, %eax
	cmpl	$83, %eax
	ja	.LBB2_37
# %bb.7:                                # %sw.case26
                                        #   in Loop: Header=BB2_1 Depth=1
	movl	.LJTI2_0@GOTOFF(%ebx,%eax,4), %eax
	addl	%ebx, %eax
	jmpl	*%eax
.LBB2_36:                               # %sw.default27
                                        #   in Loop: Header=BB2_1 Depth=1
	movzbl	-13(%ebp), %eax
	subl	$4, %esp
	pushl	%eax
.LBB2_9:                                # %while.cond21
                                        #   in Loop: Header=BB2_1 Depth=1
	leal	-24(%ebp), %eax
	pushl	%eax
	pushl	-20(%ebp)
	calll	add_char
.LBB2_10:                               # %while.cond21
                                        #   in Loop: Header=BB2_1 Depth=1
	addl	$16, %esp
	movl	%eax, -20(%ebp)
	jmp	.LBB2_1
.LBB2_8:                                # %sw.case32
                                        #   in Loop: Header=BB2_1 Depth=1
	subl	$4, %esp
	pushl	$8
	jmp	.LBB2_9
.LBB2_11:                               # %sw.case33
                                        #   in Loop: Header=BB2_1 Depth=1
	subl	$4, %esp
	pushl	$12
	jmp	.LBB2_9
.LBB2_15:                               # %sw.case37
                                        #   in Loop: Header=BB2_1 Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %edi
	movl	%edi, %esp
	movl	%esp, %ecx
	leal	-16(%ecx), %esi
	movl	%esi, %esp
	movw	$0, -16(%ecx)
	movl	$0, -16(%eax)
	jmp	.LBB2_16
.LBB2_20:                               # %sw.case57
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_21:                               # %sw.case59
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_22:                               # %sw.case61
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_23:                               # %sw.case63
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_24:                               # %sw.case65
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_25:                               # %sw.case66
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_26:                               # %sw.case67
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_27:                               # %sw.case68
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_28:                               # %sw.case69
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_29:                               # %sw.case70
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_30:                               # %sw.case71
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_31:                               # %sw.case72
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_32:                               # %sw.case73
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_33:                               # %sw.case74
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
.LBB2_34:                               # %sw.end53
                                        #   in Loop: Header=BB2_16 Depth=2
	incl	(%edi)
.LBB2_16:                               # %for.cond49
                                        #   Parent Loop BB2_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	cmpl	$3, (%edi)
	jg	.LBB2_35
# %bb.17:                               # %for.body50
                                        #   in Loop: Header=BB2_16 Depth=2
	shlw	$4, (%esi)
	calll	getchar@PLT
	movb	%al, -13(%ebp)
	incl	%eax
	cmpl	$103, %eax
	ja	.LBB2_34
# %bb.18:                               # %for.body50
                                        #   in Loop: Header=BB2_16 Depth=2
	movl	.LJTI2_1@GOTOFF(%ebx,%eax,4), %eax
	addl	%ebx, %eax
	jmpl	*%eax
.LBB2_19:                               # %sw.case55
                                        #   in Loop: Header=BB2_16 Depth=2
	incw	(%esi)
	jmp	.LBB2_20
.LBB2_13:                               # %sw.case35
                                        #   in Loop: Header=BB2_1 Depth=1
	subl	$4, %esp
	pushl	$13
	jmp	.LBB2_9
.LBB2_12:                               # %sw.case34
                                        #   in Loop: Header=BB2_1 Depth=1
	subl	$4, %esp
	pushl	$10
	jmp	.LBB2_9
.LBB2_14:                               # %sw.case36
                                        #   in Loop: Header=BB2_1 Depth=1
	subl	$4, %esp
	pushl	$9
	jmp	.LBB2_9
.LBB2_35:                               # %for.end52
                                        #   in Loop: Header=BB2_1 Depth=1
	movzwl	(%esi), %eax
	subl	$4, %esp
	pushl	%eax
	leal	-24(%ebp), %eax
	pushl	%eax
	pushl	-20(%ebp)
	calll	add_unicode
	jmp	.LBB2_10
.LBB2_37:                               # %label.error79
	subl	$12, %esp
	pushl	-20(%ebp)
	calll	free@PLT
	addl	$16, %esp
	movl	$0, -28(%ebp)
	jmp	.LBB2_38
.LBB2_5:                                # %sw.case25
	movl	-20(%ebp), %eax
	movl	%eax, -28(%ebp)
.LBB2_38:                               # %return20
	movl	-28(%ebp), %eax
	leal	-12(%ebp), %esp
	popl	%esi
	popl	%edi
	popl	%ebx
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end2:
	.size	parse_str, .Lfunc_end2-parse_str
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.p2align	2, 0x0
.LJTI2_0:
	.long	.LBB2_36@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_36@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_36@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_8@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_11@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_12@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_13@GOTOFF
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_14@GOTOFF
	.long	.LBB2_15@GOTOFF
.LJTI2_1:
	.long	.LBB2_37@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_33@GOTOFF
	.long	.LBB2_32@GOTOFF
	.long	.LBB2_31@GOTOFF
	.long	.LBB2_30@GOTOFF
	.long	.LBB2_29@GOTOFF
	.long	.LBB2_28@GOTOFF
	.long	.LBB2_27@GOTOFF
	.long	.LBB2_26@GOTOFF
	.long	.LBB2_25@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_24@GOTOFF
	.long	.LBB2_23@GOTOFF
	.long	.LBB2_22@GOTOFF
	.long	.LBB2_21@GOTOFF
	.long	.LBB2_20@GOTOFF
	.long	.LBB2_19@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_34@GOTOFF
	.long	.LBB2_24@GOTOFF
	.long	.LBB2_23@GOTOFF
	.long	.LBB2_22@GOTOFF
	.long	.LBB2_21@GOTOFF
	.long	.LBB2_20@GOTOFF
	.long	.LBB2_19@GOTOFF
                                        # -- End function
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
	calll	.L3$pb
	.cfi_adjust_cfa_offset 4
.L3$pb:
	popl	%ebx
	.cfi_adjust_cfa_offset -4
.Ltmp3:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp3-.L3$pb), %ebx
	movl	$0, 16(%esp)
	calll	getchar@PLT
	cmpl	$34, %eax
	jne	.LBB3_3
# %bb.1:                                # %sw.case90
	calll	parse_str
	movl	%eax, 20(%esp)
	testl	%eax, %eax
	jne	.LBB3_2
.LBB3_3:                                # %sw.default91
	movl	$1, 16(%esp)
.LBB3_4:                                # %return88
	movl	16(%esp), %eax
	addl	$24, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.LBB3_2:                                # %if.end94
	.cfi_def_cfa_offset 32
	movl	20(%esp), %eax
	movl	%eax, (%esp)
	calll	puts@PLT
	movl	20(%esp), %eax
	movl	%eax, (%esp)
	calll	free@PLT
	movl	$0, 16(%esp)
	jmp	.LBB3_4
.Lfunc_end3:
	.size	main, .Lfunc_end3-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits

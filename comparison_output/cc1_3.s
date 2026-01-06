	.file	"cc1"
	.text
	.globl	static_test_signed_int_operations # -- Begin function static_test_signed_int_operations
	.p2align	4
	.type	static_test_signed_int_operations,@function
static_test_signed_int_operations:      # @static_test_signed_int_operations
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	subl	$8, %esp
	.cfi_def_cfa_offset 16
	.cfi_offset %ebx, -8
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	$.L.str.0
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_1:                                # %do.body1
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.0
	.cfi_adjust_cfa_offset 4
	pushl	$-3
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.1
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_1
# %bb.2:                                # %do.end3
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_3:                                # %do.body4
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.1
	.cfi_adjust_cfa_offset 4
	pushl	$-3
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.2
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_3
# %bb.4:                                # %do.end6
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_5:                                # %do.body7
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.2
	.cfi_adjust_cfa_offset 4
	pushl	$-3
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.3
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_5
# %bb.6:                                # %do.end9
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_7:                                # %do.body10
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.3
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.4
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_7
# %bb.8:                                # %do.end12
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_9:                                # %do.body13
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.4
	.cfi_adjust_cfa_offset 4
	pushl	$-3
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.5
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_9
# %bb.10:                               # %do.end15
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_11:                               # %do.body16
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.5
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.6
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_11
# %bb.12:                               # %do.end18
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_13:                               # %do.body19
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.6
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.7
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_13
# %bb.14:                               # %do.end21
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_15:                               # %do.body22
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.7
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.8
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_15
# %bb.16:                               # %do.end24
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_17:                               # %do.body25
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_signed_int_operations.val.8
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.9
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_17
# %bb.18:                               # %do.end27
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_19:                               # %do.body28
                                        # =>This Inner Loop Header: Depth=1
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	static_test_signed_int_operations.val.9
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.10
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_19
# %bb.20:                               # %do.end30
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_21:                               # %do.body31
                                        # =>This Inner Loop Header: Depth=1
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	static_test_signed_int_operations.val.10
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.11
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_21
# %bb.22:                               # %do.end33
	xorl	%ebx, %ebx
	.p2align	4
.LBB0_23:                               # %do.body34
                                        # =>This Inner Loop Header: Depth=1
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	static_test_signed_int_operations.val.11
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.12
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB0_23
# %bb.24:                               # %do.end36
	addl	$8, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	static_test_signed_int_operations, .Lfunc_end0-static_test_signed_int_operations
	.cfi_endproc
                                        # -- End function
	.globl	static_test_unsigned_int_operations # -- Begin function static_test_unsigned_int_operations
	.p2align	4
	.type	static_test_unsigned_int_operations,@function
static_test_unsigned_int_operations:    # @static_test_unsigned_int_operations
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	subl	$8, %esp
	.cfi_def_cfa_offset 16
	.cfi_offset %ebx, -8
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	$.L.str.13
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_1:                                # %do.body38
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.12
	.cfi_adjust_cfa_offset 4
	pushl	$3
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.14
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_1
# %bb.2:                                # %do.end40
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_3:                                # %do.body41
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.13
	.cfi_adjust_cfa_offset 4
	pushl	$3
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.15
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_3
# %bb.4:                                # %do.end43
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_5:                                # %do.body44
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.14
	.cfi_adjust_cfa_offset 4
	pushl	$3
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.16
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_5
# %bb.6:                                # %do.end46
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_7:                                # %do.body47
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.15
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.17
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_7
# %bb.8:                                # %do.end49
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_9:                                # %do.body50
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.16
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.18
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_9
# %bb.10:                               # %do.end52
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_11:                               # %do.body53
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.17
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.19
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_11
# %bb.12:                               # %do.end55
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_13:                               # %do.body56
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.18
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.20
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_13
# %bb.14:                               # %do.end58
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_15:                               # %do.body59
                                        # =>This Inner Loop Header: Depth=1
	pushl	static_test_unsigned_int_operations.val.19
	.cfi_adjust_cfa_offset 4
	pushl	$7
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.21
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_15
# %bb.16:                               # %do.end61
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_17:                               # %do.body62
                                        # =>This Inner Loop Header: Depth=1
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	static_test_unsigned_int_operations.val.20
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.22
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_17
# %bb.18:                               # %do.end64
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_19:                               # %do.body65
                                        # =>This Inner Loop Header: Depth=1
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	static_test_unsigned_int_operations.val.21
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.23
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_19
# %bb.20:                               # %do.end67
	xorl	%ebx, %ebx
	.p2align	4
.LBB1_21:                               # %do.body68
                                        # =>This Inner Loop Header: Depth=1
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	static_test_unsigned_int_operations.val.22
	.cfi_adjust_cfa_offset 4
	pushl	$15
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.24
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	testb	%bl, %bl
	jne	.LBB1_21
# %bb.22:                               # %do.end70
	addl	$8, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end1:
	.size	static_test_unsigned_int_operations, .Lfunc_end1-static_test_unsigned_int_operations
	.cfi_endproc
                                        # -- End function
	.globl	static_test_logical_operators   # -- Begin function static_test_logical_operators
	.p2align	4
	.type	static_test_logical_operators,@function
static_test_logical_operators:          # @static_test_logical_operators
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	pushl	%esi
	.cfi_def_cfa_offset 12
	pushl	%eax
	.cfi_def_cfa_offset 16
	.cfi_offset %esi, -12
	.cfi_offset %ebx, -8
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	$.L.str.25
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$4, %esp
	.cfi_adjust_cfa_offset -4
	pushl	$.L.str.26
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.27, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.28, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	jne	.LBB2_2
# %bb.1:                                # %land.rhs76
	movb	$1, %cl
.LBB2_2:                                # %land.end77
	movzbl	%cl, %ecx
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.29, %eax
	movb	$1, %cl
	testb	%cl, %cl
	je	.LBB2_4
# %bb.3:
	xorl	%ecx, %ecx
.LBB2_4:                                # %land.end81
	movzbl	%cl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.30, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	jne	.LBB2_6
# %bb.5:                                # %land.rhs84
	movb	$1, %cl
.LBB2_6:                                # %land.end85
	movzbl	%cl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.31, %eax
	movb	$1, %cl
	testb	%cl, %cl
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.32, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.33, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	jne	.LBB2_8
# %bb.7:                                # %land.rhs96
	movb	$1, %cl
.LBB2_8:                                # %land.end97
	movzbl	%cl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$4, %esp
	.cfi_adjust_cfa_offset -4
	pushl	$.L.str.34
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.35, %eax
	movb	$1, %bl
	testb	%bl, %bl
	movb	$1, %cl
	jne	.LBB2_10
# %bb.9:                                # %land.rhs100
	xorl	%ecx, %ecx
.LBB2_10:                               # %land.end101
	movzbl	%cl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.36, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	jne	.LBB2_12
# %bb.11:                               # %land.rhs104
	xorl	%ebx, %ebx
.LBB2_12:                               # %land.end105
	movzbl	%bl, %ecx
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.37, %eax
	movb	$1, %bl
	xorl	%ecx, %ecx
	testb	%cl, %cl
	movzbl	%bl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.38, %eax
	testb	%bl, %bl
	movzbl	%bl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.39, %eax
	movb	$1, %bl
	xorl	%ecx, %ecx
	testb	%cl, %cl
	movb	$1, %cl
	jne	.LBB2_14
# %bb.13:                               # %land.rhs116
	xorl	%ecx, %ecx
.LBB2_14:                               # %land.end117
	movzbl	%cl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.40, %eax
	testb	%bl, %bl
	jne	.LBB2_16
# %bb.15:                               # %land.rhs120
	xorl	%ebx, %ebx
.LBB2_16:                               # %land.end121
	movzbl	%bl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.41, %eax
	movb	$1, %bl
	testb	%bl, %bl
	movzbl	%bl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$4, %esp
	.cfi_adjust_cfa_offset -4
	pushl	$.L.str.42
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.43, %eax
	movl	$5, %esi
	testb	%bl, %bl
	movl	$5, %ecx
	jne	.LBB2_18
# %bb.17:                               # %cond.false129
	movl	$10, %ecx
.LBB2_18:                               # %cond.end130
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.44, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	jne	.LBB2_20
# %bb.19:                               # %cond.false134
	movl	$10, %esi
.LBB2_20:                               # %cond.end135
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.45, %eax
	movl	$5, %esi
	xorl	%ecx, %ecx
	testb	%cl, %cl
	movl	$5, %ecx
	jne	.LBB2_22
# %bb.21:                               # %cond.false139
	movl	$10, %ecx
.LBB2_22:                               # %cond.end140
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.46, %eax
	movb	$1, %cl
	testb	%cl, %cl
	jne	.LBB2_24
# %bb.23:                               # %cond.false144
	movl	$10, %esi
.LBB2_24:                               # %cond.end145
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.47, %eax
	movl	$5, %ecx
	xorl	%edx, %edx
	testb	%dl, %dl
	jne	.LBB2_26
# %bb.25:                               # %cond.false149
	movl	$10, %ecx
.LBB2_26:                               # %cond.end150
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	$10
	.cfi_adjust_cfa_offset 4
	pushl	$5
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$36, %esp
	.cfi_adjust_cfa_offset -36
	popl	%esi
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end2:
	.size	static_test_logical_operators, .Lfunc_end2-static_test_logical_operators
	.cfi_endproc
                                        # -- End function
	.globl	static_test_float_operations    # -- Begin function static_test_float_operations
	.p2align	4
	.type	static_test_float_operations,@function
static_test_float_operations:           # @static_test_float_operations
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	subl	$40, %esp
	.cfi_def_cfa_offset 48
	.cfi_offset %ebx, -8
	movl	$.L.str.48, (%esp)
	calll	printf@PLT
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_1:                                # %do.body154
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.23
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$1074370969, 16(%esp)           # imm = 0x40099999
	movl	$-1717986918, 12(%esp)          # imm = 0x9999999A
	movl	$.L.str.49, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_1
# %bb.2:                                # %do.end156
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_3:                                # %do.body157
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.24
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$1074370969, 16(%esp)           # imm = 0x40099999
	movl	$-1717986918, 12(%esp)          # imm = 0x9999999A
	movl	$.L.str.50, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_3
# %bb.4:                                # %do.end159
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_5:                                # %do.body160
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.25
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$1074370969, 16(%esp)           # imm = 0x40099999
	movl	$-1717986918, 12(%esp)          # imm = 0x9999999A
	movl	$.L.str.51, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_5
# %bb.6:                                # %do.end162
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_7:                                # %do.body163
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.26
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$1074370969, 16(%esp)           # imm = 0x40099999
	movl	$-1717986918, 12(%esp)          # imm = 0x9999999A
	movl	$.L.str.52, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_7
# %bb.8:                                # %do.end165
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_9:                                # %do.body166
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.27
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$-1073532109, 16(%esp)          # imm = 0xC0033333
	movl	$858993459, 12(%esp)            # imm = 0x33333333
	movl	$.L.str.53, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_9
# %bb.10:                               # %do.end168
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_11:                               # %do.body169
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.28
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$-1073532109, 16(%esp)          # imm = 0xC0033333
	movl	$858993459, 12(%esp)            # imm = 0x33333333
	movl	$.L.str.54, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_11
# %bb.12:                               # %do.end171
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_13:                               # %do.body172
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.29
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$-1073532109, 16(%esp)          # imm = 0xC0033333
	movl	$858993459, 12(%esp)            # imm = 0x33333333
	movl	$.L.str.55, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_13
# %bb.14:                               # %do.end174
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_15:                               # %do.body175
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.30
	fstpl	20(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$-1073532109, 16(%esp)          # imm = 0xC0033333
	movl	$858993459, 12(%esp)            # imm = 0x33333333
	movl	$.L.str.56, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_15
# %bb.16:                               # %do.end177
	xorl	%ebx, %ebx
	.p2align	4
.LBB3_17:                               # %do.body178
                                        # =>This Inner Loop Header: Depth=1
	flds	static_test_float_operations.val.31
	fstpl	12(%esp)
	movl	$1076166656, 8(%esp)            # imm = 0x40250000
	movl	$0, 4(%esp)
	movl	$.L.str.57, (%esp)
	calll	printf@PLT
	testb	%bl, %bl
	jne	.LBB3_17
# %bb.18:                               # %do.end180
	addl	$40, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end3:
	.size	static_test_float_operations, .Lfunc_end3-static_test_float_operations
	.cfi_endproc
                                        # -- End function
	.globl	test_operator_precedence        # -- Begin function test_operator_precedence
	.p2align	4
	.type	test_operator_precedence,@function
test_operator_precedence:               # @test_operator_precedence
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	pushl	%edi
	.cfi_def_cfa_offset 12
	pushl	%esi
	.cfi_def_cfa_offset 16
	subl	$32, %esp
	.cfi_def_cfa_offset 48
	.cfi_offset %esi, -16
	.cfi_offset %edi, -12
	.cfi_offset %ebx, -8
	movl	$10, 12(%esp)
	movl	$-3, 16(%esp)
	movl	$2, 8(%esp)
	movl	$5, 4(%esp)
	movl	$1093140480, 28(%esp)           # imm = 0x41280000
	movl	$1073741824, 24(%esp)           # imm = 0x40000000
	movl	$1084647014, 20(%esp)           # imm = 0x40A66666
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	$.L.str.58
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	8(%esp), %edx
	movl	%ecx, %esi
	imull	%edx, %esi
	addl	%eax, %esi
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.59
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %esi
	movl	4(%esp), %ecx
	movl	8(%esp), %edi
	movl	%ecx, %eax
	cltd
	idivl	%edi
	movl	%esi, %edx
	subl	%eax, %edx
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.60
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	8(%esp), %edx
	leal	(%eax,%ecx), %esi
	imull	%edx, %esi
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.61
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	8(%esp), %edx
	movl	%ecx, %esi
	andl	%edx, %esi
	addl	%eax, %esi
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.62
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	8(%esp), %edx
	movl	%ecx, %esi
	imull	%edx, %esi
	orl	%eax, %esi
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.63
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	8(%esp), %edx
	movl	%eax, %esi
	orl	%ecx, %esi
	imull	%edx, %esi
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.64
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	%ecx, %edx
	sarl	%edx
	leal	(%edx,%eax,2), %edx
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.65
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	%ecx, %edx
	sarl	%edx
	leal	(%edx,%eax,2), %edx
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.66
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.67, %edi
	movl	12(%esp), %ecx
	movl	4(%esp), %edx
	movl	8(%esp), %esi
	cmpl	%edx, %ecx
	movb	$1, %bl
	movb	$1, %al
	jl	.LBB4_2
# %bb.1:                                # %land.rhs182
	movl	4(%esp), %eax
	testl	%eax, 8(%esp)
	setne	%al
.LBB4_2:                                # %land.end183
	movzbl	%al, %eax
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.68, %eax
	movl	12(%esp), %ecx
	movl	4(%esp), %edx
	movl	8(%esp), %esi
	cmpl	%edx, %ecx
	jl	.LBB4_6
# %bb.3:                                # %land.rhs186
	cmpl	$0, 4(%esp)
	je	.LBB4_4
# %bb.5:                                # %land.rhs189
	cmpl	$0, 8(%esp)
	setne	%bl
	jmp	.LBB4_6
.LBB4_4:
	xorl	%ebx, %ebx
.LBB4_6:                                # %land.end187
	movzbl	%bl, %edi
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	8(%esp), %edx
	movl	16(%esp), %esi
	movl	%ecx, %edi
	imull	%edx, %edi
	subl	%esi, %edi
	addl	%eax, %edi
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.69
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	12(%esp), %eax
	movl	4(%esp), %ecx
	movl	8(%esp), %edx
	movl	16(%esp), %esi
	leal	(%eax,%ecx), %edi
	leal	(%edx,%esi), %ebx
	imull	%ebx, %edi
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.70
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$20, %esp
	.cfi_adjust_cfa_offset -20
	pushl	$.L.str.71
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	flds	28(%esp)
	flds	20(%esp)
	flds	24(%esp)
	fld	%st(1)
	fmul	%st(1), %st
	fadd	%st(3), %st
	subl	$48, %esp
	.cfi_adjust_cfa_offset 48
	fxch	%st(1)
	fstpl	20(%esp)
	fxch	%st(1)
	fstpl	12(%esp)
	fxch	%st(1)
	fstpl	4(%esp)
	fstpl	28(%esp)
	movl	$.L.str.72, (%esp)
	calll	printf@PLT
	addl	$48, %esp
	.cfi_adjust_cfa_offset -48
	flds	28(%esp)
	flds	20(%esp)
	flds	24(%esp)
	fld	%st(1)
	fdiv	%st(1), %st
	fsubr	%st(3), %st
	subl	$48, %esp
	.cfi_adjust_cfa_offset 48
	fxch	%st(1)
	fstpl	20(%esp)
	fxch	%st(1)
	fstpl	12(%esp)
	fxch	%st(1)
	fstpl	4(%esp)
	fstpl	28(%esp)
	movl	$.L.str.73, (%esp)
	calll	printf@PLT
	addl	$48, %esp
	.cfi_adjust_cfa_offset -48
	flds	28(%esp)
	flds	20(%esp)
	flds	24(%esp)
	fld	%st(2)
	fadd	%st(2), %st
	fmul	%st(1), %st
	subl	$48, %esp
	.cfi_adjust_cfa_offset 48
	fxch	%st(1)
	fstpl	20(%esp)
	fxch	%st(1)
	fstpl	12(%esp)
	fxch	%st(1)
	fstpl	4(%esp)
	fstpl	28(%esp)
	movl	$.L.str.74, (%esp)
	calll	printf@PLT
	addl	$36, %esp
	.cfi_adjust_cfa_offset -36
	pushl	$.L.str.75
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	16(%esp), %ecx
	movl	4(%esp), %esi
	movl	8(%esp), %edi
	movl	%ecx, %eax
	imull	%esi, %eax
	cltd
	idivl	%edi
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.76
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	16(%esp), %eax
	movl	12(%esp), %ecx
	movl	4(%esp), %edx
	movl	%edx, %esi
	negl	%esi
	imull	%ecx, %esi
	addl	%eax, %esi
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.77
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$64, %esp
	.cfi_adjust_cfa_offset -64
	popl	%esi
	.cfi_def_cfa_offset 12
	popl	%edi
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end4:
	.size	test_operator_precedence, .Lfunc_end4-test_operator_precedence
	.cfi_endproc
                                        # -- End function
	.globl	static_main                     # -- Begin function static_main
	.p2align	4
	.type	static_main,@function
static_main:                            # @static_main
	.cfi_startproc
# %bb.0:                                # %entry
	subl	$12, %esp
	.cfi_def_cfa_offset 16
	calll	static_test_signed_int_operations
	movl	$.L.str.78, (%esp)
	calll	printf@PLT
	calll	static_test_unsigned_int_operations
	movl	$.L.str.79, (%esp)
	calll	printf@PLT
	calll	static_test_float_operations
	movl	$.L.str.80, (%esp)
	calll	printf@PLT
	calll	static_test_logical_operators
	movl	$0, 8(%esp)
	xorl	%eax, %eax
	addl	$12, %esp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end5:
	.size	static_main, .Lfunc_end5-static_main
	.cfi_endproc
                                        # -- End function
	.globl	test_signed_int_operations      # -- Begin function test_signed_int_operations
	.p2align	4
	.type	test_signed_int_operations,@function
test_signed_int_operations:             # @test_signed_int_operations
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register %ebp
	pushl	%ebx
	subl	$20, %esp
	.cfi_offset %ebx, -12
	movl	$15, -8(%ebp)
	movl	$7, -12(%ebp)
	movl	$-3, -16(%ebp)
	subl	$12, %esp
	pushl	$.L.str.81
	calll	printf@PLT
	addl	$16, %esp
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_1:                                # %do.body196
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	addl	-16(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-16(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.82
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_1
# %bb.2:                                # %do.end198
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_3:                                # %do.body199
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	subl	-16(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-16(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.83
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_3
# %bb.4:                                # %do.end201
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_5:                                # %do.body202
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	imull	-16(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-16(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.84
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_5
# %bb.6:                                # %do.end204
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_7:                                # %do.body205
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %ecx
	leal	-16(%ecx), %esp
	movl	-8(%ebp), %eax
	cltd
	idivl	-12(%ebp)
	movl	%eax, -16(%ecx)
	pushl	%eax
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.85
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_7
# %bb.8:                                # %do.end207
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_9:                                # %do.body208
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %ecx
	leal	-16(%ecx), %esp
	movl	-8(%ebp), %eax
	cltd
	idivl	-16(%ebp)
	movl	%eax, -16(%ecx)
	pushl	%eax
	pushl	-16(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.86
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_9
# %bb.10:                               # %do.end210
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_11:                               # %do.body211
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %ecx
	leal	-16(%ecx), %esp
	movl	-8(%ebp), %eax
	cltd
	idivl	-12(%ebp)
	movl	%edx, -16(%ecx)
	pushl	%edx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.87
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_11
# %bb.12:                               # %do.end213
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_13:                               # %do.body214
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	andl	-12(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.88
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_13
# %bb.14:                               # %do.end216
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_15:                               # %do.body217
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	orl	-12(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.89
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_15
# %bb.16:                               # %do.end219
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_17:                               # %do.body220
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	xorl	-12(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.90
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_17
# %bb.18:                               # %do.end222
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_19:                               # %do.body223
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	notl	%ecx
	movl	%ecx, -16(%eax)
	subl	$4, %esp
	pushl	%ecx
	pushl	-8(%ebp)
	pushl	$.L.str.91
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_19
# %bb.20:                               # %do.end225
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_21:                               # %do.body226
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	addl	%ecx, %ecx
	movl	%ecx, -16(%eax)
	subl	$4, %esp
	pushl	%ecx
	pushl	-8(%ebp)
	pushl	$.L.str.92
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_21
# %bb.22:                               # %do.end228
	xorl	%ebx, %ebx
	.p2align	4
.LBB6_23:                               # %do.body229
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	sarl	%ecx
	movl	%ecx, -16(%eax)
	subl	$4, %esp
	pushl	%ecx
	pushl	-8(%ebp)
	pushl	$.L.str.93
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB6_23
# %bb.24:                               # %do.end231
	leal	-4(%ebp), %esp
	popl	%ebx
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end6:
	.size	test_signed_int_operations, .Lfunc_end6-test_signed_int_operations
	.cfi_endproc
                                        # -- End function
	.globl	test_unsigned_int_operations    # -- Begin function test_unsigned_int_operations
	.p2align	4
	.type	test_unsigned_int_operations,@function
test_unsigned_int_operations:           # @test_unsigned_int_operations
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register %ebp
	pushl	%ebx
	subl	$20, %esp
	.cfi_offset %ebx, -12
	movl	$15, -8(%ebp)
	movl	$7, -12(%ebp)
	movl	$3, -16(%ebp)
	subl	$12, %esp
	pushl	$.L.str.94
	calll	printf@PLT
	addl	$16, %esp
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_1:                                # %do.body233
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	addl	-16(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-16(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.95
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_1
# %bb.2:                                # %do.end235
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_3:                                # %do.body236
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	subl	-16(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-16(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.96
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_3
# %bb.4:                                # %do.end238
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_5:                                # %do.body239
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	imull	-16(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-16(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.97
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_5
# %bb.6:                                # %do.end241
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_7:                                # %do.body242
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %ecx
	leal	-16(%ecx), %esp
	movl	-8(%ebp), %eax
	cltd
	idivl	-12(%ebp)
	movl	%eax, -16(%ecx)
	pushl	%eax
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.98
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_7
# %bb.8:                                # %do.end244
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_9:                                # %do.body245
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %ecx
	leal	-16(%ecx), %esp
	movl	-8(%ebp), %eax
	cltd
	idivl	-12(%ebp)
	movl	%edx, -16(%ecx)
	pushl	%edx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.99
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_9
# %bb.10:                               # %do.end247
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_11:                               # %do.body248
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	andl	-12(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.100
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_11
# %bb.12:                               # %do.end250
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_13:                               # %do.body251
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	orl	-12(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.101
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_13
# %bb.14:                               # %do.end253
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_15:                               # %do.body254
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	xorl	-12(%ebp), %ecx
	movl	%ecx, -16(%eax)
	pushl	%ecx
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	$.L.str.102
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_15
# %bb.16:                               # %do.end256
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_17:                               # %do.body257
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	notl	%ecx
	movl	%ecx, -16(%eax)
	subl	$4, %esp
	pushl	%ecx
	pushl	-8(%ebp)
	pushl	$.L.str.103
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_17
# %bb.18:                               # %do.end259
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_19:                               # %do.body260
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	addl	%ecx, %ecx
	movl	%ecx, -16(%eax)
	subl	$4, %esp
	pushl	%ecx
	pushl	-8(%ebp)
	pushl	$.L.str.104
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_19
# %bb.20:                               # %do.end262
	xorl	%ebx, %ebx
	.p2align	4
.LBB7_21:                               # %do.body263
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-8(%ebp), %ecx
	sarl	%ecx
	movl	%ecx, -16(%eax)
	subl	$4, %esp
	pushl	%ecx
	pushl	-8(%ebp)
	pushl	$.L.str.105
	calll	printf@PLT
	addl	$16, %esp
	testb	%bl, %bl
	jne	.LBB7_21
# %bb.22:                               # %do.end265
	leal	-4(%ebp), %esp
	popl	%ebx
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end7:
	.size	test_unsigned_int_operations, .Lfunc_end7-test_unsigned_int_operations
	.cfi_endproc
                                        # -- End function
	.globl	test_logical_operators          # -- Begin function test_logical_operators
	.p2align	4
	.type	test_logical_operators,@function
test_logical_operators:                 # @test_logical_operators
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebx
	.cfi_def_cfa_offset 8
	pushl	%edi
	.cfi_def_cfa_offset 12
	pushl	%esi
	.cfi_def_cfa_offset 16
	subl	$32, %esp
	.cfi_def_cfa_offset 48
	.cfi_offset %esi, -16
	.cfi_offset %edi, -12
	.cfi_offset %ebx, -8
	movl	$1, 24(%esp)
	movl	$10, 16(%esp)
	movl	$5, 12(%esp)
	movl	$0, 20(%esp)
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	$.L.str.106
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$4, %esp
	.cfi_adjust_cfa_offset -4
	pushl	$.L.str.107
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.108, %eax
	movl	24(%esp), %ecx
	movl	20(%esp), %edx
	testl	%ecx, %ecx
	je	.LBB8_1
# %bb.2:                                # %land.rhs267
	cmpl	$0, 20(%esp)
	setne	%bl
	jmp	.LBB8_3
.LBB8_1:
	xorl	%ebx, %ebx
.LBB8_3:                                # %land.end268
	movzbl	%bl, %esi
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.109, %eax
	movl	24(%esp), %ecx
	testl	%ecx, %ecx
	je	.LBB8_4
# %bb.5:                                # %land.rhs271
	movb	$1, %dl
	jmp	.LBB8_6
.LBB8_4:
	xorl	%edx, %edx
.LBB8_6:                                # %land.end272
	movzbl	%dl, %edx
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.110, %eax
	movl	20(%esp), %ecx
	movl	12(%esp), %edx
	testl	%ecx, %ecx
	je	.LBB8_7
# %bb.8:                                # %land.rhs275
	cmpl	$0, 12(%esp)
	setne	%bl
	jmp	.LBB8_9
.LBB8_7:
	xorl	%ebx, %ebx
.LBB8_9:                                # %land.end276
	movzbl	%bl, %esi
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.111, %eax
	movl	12(%esp), %ecx
	movl	16(%esp), %edx
	testl	%ecx, %ecx
	je	.LBB8_10
# %bb.11:                               # %land.rhs279
	cmpl	$0, 16(%esp)
	setne	%bl
	jmp	.LBB8_12
.LBB8_10:
	xorl	%ebx, %ebx
.LBB8_12:                               # %land.end280
	movzbl	%bl, %esi
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.112, %eax
	movb	$1, %cl
	testb	%cl, %cl
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.113, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.114, %eax
	xorl	%ecx, %ecx
	testb	%cl, %cl
	jne	.LBB8_14
# %bb.13:                               # %land.rhs291
	movb	$1, %cl
.LBB8_14:                               # %land.end292
	movzbl	%cl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.115, %eax
	movl	20(%esp), %ecx
	movl	12(%esp), %edx
	testl	%ecx, %ecx
	je	.LBB8_15
# %bb.16:                               # %land.rhs295
	incl	12(%esp)
	setne	%bl
	jmp	.LBB8_17
.LBB8_15:
	xorl	%ebx, %ebx
.LBB8_17:                               # %land.end296
	movzbl	%bl, %esi
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$8, %esp
	.cfi_adjust_cfa_offset -8
	pushl	20(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.116
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$4, %esp
	.cfi_adjust_cfa_offset -4
	pushl	$.L.str.117
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.118, %esi
	movl	24(%esp), %ecx
	movl	20(%esp), %edx
	movb	$1, %bl
	testl	%ecx, %ecx
	movb	$1, %al
	jne	.LBB8_19
# %bb.18:                               # %land.rhs299
	cmpl	$0, 20(%esp)
	setne	%al
.LBB8_19:                               # %land.end300
	movzbl	%al, %eax
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.119, %eax
	movl	20(%esp), %ecx
	testl	%ecx, %ecx
	jne	.LBB8_21
# %bb.20:                               # %land.rhs303
	xorl	%ebx, %ebx
.LBB8_21:                               # %land.end304
	movzbl	%bl, %edx
	subl	$4, %esp
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.120, %eax
	movl	12(%esp), %ecx
	movb	$1, %bl
	xorl	%edx, %edx
	testb	%dl, %dl
	movb	$1, %dl
	jne	.LBB8_23
# %bb.22:                               # %land.rhs307
	cmpl	$0, 12(%esp)
	setne	%dl
.LBB8_23:                               # %land.end308
	movzbl	%dl, %edx
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.121, %eax
	movl	12(%esp), %ecx
	movl	16(%esp), %edx
	testl	%ecx, %ecx
	jne	.LBB8_25
# %bb.24:                               # %land.rhs311
	cmpl	$0, 16(%esp)
	setne	%bl
.LBB8_25:                               # %land.end312
	movzbl	%bl, %esi
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.122, %eax
	movb	$1, %bl
	xorl	%ecx, %ecx
	testb	%cl, %cl
	movb	$1, %cl
	jne	.LBB8_27
# %bb.26:                               # %land.rhs315
	xorl	%ecx, %ecx
.LBB8_27:                               # %land.end316
	movzbl	%cl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.123, %eax
	testb	%bl, %bl
	jne	.LBB8_29
# %bb.28:                               # %land.rhs319
	xorl	%ebx, %ebx
.LBB8_29:                               # %land.end320
	movzbl	%bl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$0
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.124, %eax
	movb	$1, %bl
	testb	%bl, %bl
	movzbl	%bl, %ecx
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	$1
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.125, %eax
	movl	24(%esp), %ecx
	movl	16(%esp), %edx
	testl	%ecx, %ecx
	jne	.LBB8_31
# %bb.30:                               # %land.rhs327
	incl	16(%esp)
	setne	%bl
.LBB8_31:                               # %land.end328
	movzbl	%bl, %esi
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$8, %esp
	.cfi_adjust_cfa_offset -8
	pushl	24(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.126
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$4, %esp
	.cfi_adjust_cfa_offset -4
	pushl	$.L.str.127
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$.L.str.128, %eax
	movl	24(%esp), %ecx
	movl	12(%esp), %edx
	movl	16(%esp), %esi
	testl	%ecx, %ecx
	je	.LBB8_33
# %bb.32:                               # %cond.true331
	movl	12(%esp), %edi
	jmp	.LBB8_34
.LBB8_33:                               # %cond.false332
	movl	16(%esp), %edi
.LBB8_34:                               # %cond.end333
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.129, %eax
	movl	20(%esp), %ecx
	movl	12(%esp), %edx
	movl	16(%esp), %esi
	testl	%ecx, %ecx
	je	.LBB8_36
# %bb.35:                               # %cond.true336
	movl	12(%esp), %edi
	jmp	.LBB8_37
.LBB8_36:                               # %cond.false337
	movl	16(%esp), %edi
.LBB8_37:                               # %cond.end338
	subl	$12, %esp
	.cfi_adjust_cfa_offset 12
	pushl	%edi
	.cfi_adjust_cfa_offset 4
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.130, %eax
	movl	12(%esp), %ecx
	movl	16(%esp), %edx
	cmpl	%edx, %ecx
	jle	.LBB8_39
# %bb.38:                               # %cond.true341
	movl	12(%esp), %esi
	jmp	.LBB8_40
.LBB8_39:                               # %cond.false342
	movl	16(%esp), %esi
.LBB8_40:                               # %cond.end343
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.131, %eax
	movl	12(%esp), %ecx
	movl	16(%esp), %edx
	cmpl	%edx, %ecx
	jge	.LBB8_42
# %bb.41:                               # %cond.true346
	movl	12(%esp), %esi
	jmp	.LBB8_43
.LBB8_42:                               # %cond.false347
	movl	16(%esp), %esi
.LBB8_43:                               # %cond.end348
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$32, %esp
	.cfi_adjust_cfa_offset -32
	movl	$.L.str.132, %eax
	movl	12(%esp), %ecx
	movl	16(%esp), %edx
	cmpl	%edx, %ecx
	jne	.LBB8_45
# %bb.44:                               # %cond.true351
	movl	12(%esp), %esi
	jmp	.LBB8_46
.LBB8_45:                               # %cond.false352
	movl	16(%esp), %esi
.LBB8_46:                               # %cond.end353
	subl	$8, %esp
	.cfi_adjust_cfa_offset 8
	pushl	%esi
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%edx
	.cfi_adjust_cfa_offset 4
	pushl	%ecx
	.cfi_adjust_cfa_offset 4
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$20, %esp
	.cfi_adjust_cfa_offset -20
	pushl	$.L.str.133
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	cmpl	$0, 24(%esp)
	je	.LBB8_48
# %bb.47:                               # %cond.true356
	movl	12(%esp), %eax
	addl	$2, %eax
	movl	%eax, 12(%esp)
	jmp	.LBB8_49
.LBB8_48:                               # %cond.false357
	movl	16(%esp), %eax
.LBB8_49:                               # %cond.end358
	movl	%eax, 28(%esp)
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	16(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	32(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.134
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$8, %esp
	.cfi_adjust_cfa_offset -8
	pushl	20(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.135
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$16, %esp
	.cfi_adjust_cfa_offset -16
	movl	$5, 12(%esp)
	cmpl	$0, 20(%esp)
	je	.LBB8_51
# %bb.50:                               # %cond.true361
	movl	12(%esp), %eax
	jmp	.LBB8_52
.LBB8_51:                               # %cond.false362
	movl	16(%esp), %eax
	addl	$3, %eax
	movl	%eax, 16(%esp)
.LBB8_52:                               # %cond.end363
	movl	%eax, 28(%esp)
	pushl	%eax
	.cfi_adjust_cfa_offset 4
	pushl	20(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	28(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.136
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$8, %esp
	.cfi_adjust_cfa_offset -8
	pushl	24(%esp)
	.cfi_adjust_cfa_offset 4
	pushl	$.L.str.137
	.cfi_adjust_cfa_offset 4
	calll	printf@PLT
	addl	$48, %esp
	.cfi_adjust_cfa_offset -48
	popl	%esi
	.cfi_def_cfa_offset 12
	popl	%edi
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end8:
	.size	test_logical_operators, .Lfunc_end8-test_logical_operators
	.cfi_endproc
                                        # -- End function
	.globl	test_float_operations           # -- Begin function test_float_operations
	.p2align	4
	.type	test_float_operations,@function
test_float_operations:                  # @test_float_operations
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register %ebp
	pushl	%ebx
	pushl	%esi
	subl	$16, %esp
	.cfi_offset %esi, -16
	.cfi_offset %ebx, -12
	movl	$1093140480, -12(%ebp)          # imm = 0x41280000
	movl	$-1072064102, -20(%ebp)         # imm = 0xC019999A
	movl	$1078774989, -16(%ebp)          # imm = 0x404CCCCD
	subl	$12, %esp
	pushl	$.L.str.138
	calll	printf@PLT
	addl	$16, %esp
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_1:                                # %do.body367
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fadds	-16(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-16(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.139, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_1
# %bb.2:                                # %do.end369
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_3:                                # %do.body370
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fsubs	-16(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-16(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.140, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_3
# %bb.4:                                # %do.end372
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_5:                                # %do.body373
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fmuls	-16(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-16(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.141, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_5
# %bb.6:                                # %do.end375
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_7:                                # %do.body376
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fdivs	-16(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-16(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.142, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_7
# %bb.8:                                # %do.end378
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_9:                                # %do.body379
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fadds	-20(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-20(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.143, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_9
# %bb.10:                               # %do.end381
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_11:                               # %do.body382
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fsubs	-20(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-20(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.144, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_11
# %bb.12:                               # %do.end384
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_13:                               # %do.body385
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fmuls	-20(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-20(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.145, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_13
# %bb.14:                               # %do.end387
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_15:                               # %do.body388
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	flds	-12(%ebp)
	fdivs	-20(%ebp)
	fstps	-16(%eax)
	flds	-12(%ebp)
	flds	-20(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	20(%esp)
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.146, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_15
# %bb.16:                               # %do.end390
	movl	$-2147483648, %esi              # imm = 0x80000000
	xorl	%ebx, %ebx
	.p2align	4
.LBB9_17:                               # %do.body391
                                        # =>This Inner Loop Header: Depth=1
	movl	%esp, %eax
	leal	-16(%eax), %esp
	movl	-12(%ebp), %ecx
	xorl	%esi, %ecx
	movl	%ecx, -16(%eax)
	flds	-12(%ebp)
	flds	-16(%eax)
	subl	$32, %esp
	fstpl	12(%esp)
	fstpl	4(%esp)
	movl	$.L.str.147, (%esp)
	calll	printf@PLT
	addl	$32, %esp
	testb	%bl, %bl
	jne	.LBB9_17
# %bb.18:                               # %do.end393
	leal	-8(%ebp), %esp
	popl	%esi
	popl	%ebx
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end9:
	.size	test_float_operations, .Lfunc_end9-test_float_operations
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
	calll	test_signed_int_operations
	movl	$.L.str.148, (%esp)
	calll	printf@PLT
	calll	test_unsigned_int_operations
	movl	$.L.str.149, (%esp)
	calll	printf@PLT
	calll	test_float_operations
	movl	$.L.str.150, (%esp)
	calll	printf@PLT
	calll	test_logical_operators
	movl	$.L.str.151, (%esp)
	calll	printf@PLT
	movl	$.L.str.152, (%esp)
	calll	printf@PLT
	calll	static_main
	movl	$.L.str.153, (%esp)
	calll	printf@PLT
	calll	test_operator_precedence
	movl	$0, 8(%esp)
	xorl	%eax, %eax
	addl	$12, %esp
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end10:
	.size	main, .Lfunc_end10-main
	.cfi_endproc
                                        # -- End function
	.type	static_test_signed_int_operations.val.0,@object # @static_test_signed_int_operations.val.0
	.data
	.p2align	2, 0x0
static_test_signed_int_operations.val.0:
	.long	12                              # 0xc
	.size	static_test_signed_int_operations.val.0, 4

	.type	static_test_signed_int_operations.val.1,@object # @static_test_signed_int_operations.val.1
	.p2align	2, 0x0
static_test_signed_int_operations.val.1:
	.long	18                              # 0x12
	.size	static_test_signed_int_operations.val.1, 4

	.type	static_test_signed_int_operations.val.2,@object # @static_test_signed_int_operations.val.2
	.p2align	2, 0x0
static_test_signed_int_operations.val.2:
	.long	4294967251                      # 0xffffffd3
	.size	static_test_signed_int_operations.val.2, 4

	.type	static_test_signed_int_operations.val.3,@object # @static_test_signed_int_operations.val.3
	.p2align	2, 0x0
static_test_signed_int_operations.val.3:
	.long	2                               # 0x2
	.size	static_test_signed_int_operations.val.3, 4

	.type	static_test_signed_int_operations.val.4,@object # @static_test_signed_int_operations.val.4
	.p2align	2, 0x0
static_test_signed_int_operations.val.4:
	.long	4294967291                      # 0xfffffffb
	.size	static_test_signed_int_operations.val.4, 4

	.type	static_test_signed_int_operations.val.5,@object # @static_test_signed_int_operations.val.5
	.p2align	2, 0x0
static_test_signed_int_operations.val.5:
	.long	1                               # 0x1
	.size	static_test_signed_int_operations.val.5, 4

	.type	static_test_signed_int_operations.val.6,@object # @static_test_signed_int_operations.val.6
	.p2align	2, 0x0
static_test_signed_int_operations.val.6:
	.long	7                               # 0x7
	.size	static_test_signed_int_operations.val.6, 4

	.type	static_test_signed_int_operations.val.7,@object # @static_test_signed_int_operations.val.7
	.p2align	2, 0x0
static_test_signed_int_operations.val.7:
	.long	15                              # 0xf
	.size	static_test_signed_int_operations.val.7, 4

	.type	static_test_signed_int_operations.val.8,@object # @static_test_signed_int_operations.val.8
	.p2align	2, 0x0
static_test_signed_int_operations.val.8:
	.long	8                               # 0x8
	.size	static_test_signed_int_operations.val.8, 4

	.type	static_test_signed_int_operations.val.9,@object # @static_test_signed_int_operations.val.9
	.p2align	2, 0x0
static_test_signed_int_operations.val.9:
	.long	4294967280                      # 0xfffffff0
	.size	static_test_signed_int_operations.val.9, 4

	.type	static_test_signed_int_operations.val.10,@object # @static_test_signed_int_operations.val.10
	.p2align	2, 0x0
static_test_signed_int_operations.val.10:
	.long	30                              # 0x1e
	.size	static_test_signed_int_operations.val.10, 4

	.type	static_test_signed_int_operations.val.11,@object # @static_test_signed_int_operations.val.11
	.p2align	2, 0x0
static_test_signed_int_operations.val.11:
	.long	7                               # 0x7
	.size	static_test_signed_int_operations.val.11, 4

	.type	static_test_unsigned_int_operations.val.12,@object # @static_test_unsigned_int_operations.val.12
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.12:
	.long	18                              # 0x12
	.size	static_test_unsigned_int_operations.val.12, 4

	.type	static_test_unsigned_int_operations.val.13,@object # @static_test_unsigned_int_operations.val.13
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.13:
	.long	12                              # 0xc
	.size	static_test_unsigned_int_operations.val.13, 4

	.type	static_test_unsigned_int_operations.val.14,@object # @static_test_unsigned_int_operations.val.14
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.14:
	.long	45                              # 0x2d
	.size	static_test_unsigned_int_operations.val.14, 4

	.type	static_test_unsigned_int_operations.val.15,@object # @static_test_unsigned_int_operations.val.15
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.15:
	.long	2                               # 0x2
	.size	static_test_unsigned_int_operations.val.15, 4

	.type	static_test_unsigned_int_operations.val.16,@object # @static_test_unsigned_int_operations.val.16
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.16:
	.long	1                               # 0x1
	.size	static_test_unsigned_int_operations.val.16, 4

	.type	static_test_unsigned_int_operations.val.17,@object # @static_test_unsigned_int_operations.val.17
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.17:
	.long	7                               # 0x7
	.size	static_test_unsigned_int_operations.val.17, 4

	.type	static_test_unsigned_int_operations.val.18,@object # @static_test_unsigned_int_operations.val.18
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.18:
	.long	15                              # 0xf
	.size	static_test_unsigned_int_operations.val.18, 4

	.type	static_test_unsigned_int_operations.val.19,@object # @static_test_unsigned_int_operations.val.19
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.19:
	.long	8                               # 0x8
	.size	static_test_unsigned_int_operations.val.19, 4

	.type	static_test_unsigned_int_operations.val.20,@object # @static_test_unsigned_int_operations.val.20
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.20:
	.long	4294967280                      # 0xfffffff0
	.size	static_test_unsigned_int_operations.val.20, 4

	.type	static_test_unsigned_int_operations.val.21,@object # @static_test_unsigned_int_operations.val.21
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.21:
	.long	30                              # 0x1e
	.size	static_test_unsigned_int_operations.val.21, 4

	.type	static_test_unsigned_int_operations.val.22,@object # @static_test_unsigned_int_operations.val.22
	.p2align	2, 0x0
static_test_unsigned_int_operations.val.22:
	.long	7                               # 0x7
	.size	static_test_unsigned_int_operations.val.22, 4

	.type	static_test_float_operations.val.23,@object # @static_test_float_operations.val.23
	.p2align	2, 0x0
static_test_float_operations.val.23:
	.long	0x415b3330                      # float 13.6999969
	.size	static_test_float_operations.val.23, 4

	.type	static_test_float_operations.val.24,@object # @static_test_float_operations.val.24
	.p2align	2, 0x0
static_test_float_operations.val.24:
	.long	0x40e99998                      # float 7.29999924
	.size	static_test_float_operations.val.24, 4

	.type	static_test_float_operations.val.25,@object # @static_test_float_operations.val.25
	.p2align	2, 0x0
static_test_float_operations.val.25:
	.long	0x42066660                      # float 33.5999756
	.size	static_test_float_operations.val.25, 4

	.type	static_test_float_operations.val.26,@object # @static_test_float_operations.val.26
	.p2align	2, 0x0
static_test_float_operations.val.26:
	.long	0x40520000                      # float 3.28125
	.size	static_test_float_operations.val.26, 4

	.type	static_test_float_operations.val.27,@object # @static_test_float_operations.val.27
	.p2align	2, 0x0
static_test_float_operations.val.27:
	.long	0x41019998                      # float 8.09999847
	.size	static_test_float_operations.val.27, 4

	.type	static_test_float_operations.val.28,@object # @static_test_float_operations.val.28
	.p2align	2, 0x0
static_test_float_operations.val.28:
	.long	0x414e6660                      # float 12.8999939
	.size	static_test_float_operations.val.28, 4

	.type	static_test_float_operations.val.29,@object # @static_test_float_operations.val.29
	.p2align	2, 0x0
static_test_float_operations.val.29:
	.long	0xc1c99998                      # float -25.1999969
	.size	static_test_float_operations.val.29, 4

	.type	static_test_float_operations.val.30,@object # @static_test_float_operations.val.30
	.p2align	2, 0x0
static_test_float_operations.val.30:
	.long	0xc08c0000                      # float -4.375
	.size	static_test_float_operations.val.30, 4

	.type	static_test_float_operations.val.31,@object # @static_test_float_operations.val.31
	.p2align	2, 0x0
static_test_float_operations.val.31:
	.long	0xc1280000                      # float -10.5
	.size	static_test_float_operations.val.31, 4

	.type	.L.str.0,@object                # @.str.0
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.0:
	.asciz	"Signed Integer Arithmetic and Bitwise Tests:\n"
	.size	.L.str.0, 46

	.type	.L.str.1,@object                # @.str.1
	.p2align	4, 0x0
.L.str.1:
	.asciz	"Addition:        %d + %d = %d\n"
	.size	.L.str.1, 31

	.type	.L.str.2,@object                # @.str.2
	.p2align	4, 0x0
.L.str.2:
	.asciz	"Subtraction:     %d - %d = %d\n"
	.size	.L.str.2, 31

	.type	.L.str.3,@object                # @.str.3
	.p2align	4, 0x0
.L.str.3:
	.asciz	"Multiplication:  %d * %d = %dd\n"
	.size	.L.str.3, 32

	.type	.L.str.4,@object                # @.str.4
	.p2align	4, 0x0
.L.str.4:
	.asciz	"Division:        %d / %d = %d\n"
	.size	.L.str.4, 31

	.type	.L.str.5,@object                # @.str.5
	.p2align	4, 0x0
.L.str.5:
	.asciz	"Division:        %d / %d = %d\n"
	.size	.L.str.5, 31

	.type	.L.str.6,@object                # @.str.6
	.p2align	4, 0x0
.L.str.6:
	.asciz	"Modulo:        %d %% %d = %d\n"
	.size	.L.str.6, 30

	.type	.L.str.7,@object                # @.str.7
	.p2align	4, 0x0
.L.str.7:
	.asciz	"Bitwise AND:     %d & %d = %d\n"
	.size	.L.str.7, 31

	.type	.L.str.8,@object                # @.str.8
	.p2align	4, 0x0
.L.str.8:
	.asciz	"Bitwise OR:      %d | %d = %d\n"
	.size	.L.str.8, 31

	.type	.L.str.9,@object                # @.str.9
	.p2align	4, 0x0
.L.str.9:
	.asciz	"Bitwise XOR:     %d ^ %d = %d\n"
	.size	.L.str.9, 31

	.type	.L.str.10,@object               # @.str.10
	.p2align	4, 0x0
.L.str.10:
	.asciz	"Bitwise NOT:     ~%d = %d\n"
	.size	.L.str.10, 27

	.type	.L.str.11,@object               # @.str.11
	.p2align	4, 0x0
.L.str.11:
	.asciz	"Shift Left:      %d << 1 = %d\n"
	.size	.L.str.11, 31

	.type	.L.str.12,@object               # @.str.12
	.p2align	4, 0x0
.L.str.12:
	.asciz	"Shift Right:     %d >> 1 = %d\n"
	.size	.L.str.12, 31

	.type	.L.str.13,@object               # @.str.13
	.p2align	4, 0x0
.L.str.13:
	.asciz	"Unsigned Integer Arithmetic and Bitwise Tests:\n"
	.size	.L.str.13, 48

	.type	.L.str.14,@object               # @.str.14
	.p2align	4, 0x0
.L.str.14:
	.asciz	"Addition:        %u + %u = %u\n"
	.size	.L.str.14, 31

	.type	.L.str.15,@object               # @.str.15
	.p2align	4, 0x0
.L.str.15:
	.asciz	"Subtraction:     %u - %u = %u\n"
	.size	.L.str.15, 31

	.type	.L.str.16,@object               # @.str.16
	.p2align	4, 0x0
.L.str.16:
	.asciz	"Multiplication:  %u * %u = %u\n"
	.size	.L.str.16, 31

	.type	.L.str.17,@object               # @.str.17
	.p2align	4, 0x0
.L.str.17:
	.asciz	"Division:        %u / %u = %u\n"
	.size	.L.str.17, 31

	.type	.L.str.18,@object               # @.str.18
	.p2align	4, 0x0
.L.str.18:
	.asciz	"Modulo:          %u %% %u = %u\n"
	.size	.L.str.18, 32

	.type	.L.str.19,@object               # @.str.19
	.p2align	4, 0x0
.L.str.19:
	.asciz	"Bitwise AND:     %u & %u = %u\n"
	.size	.L.str.19, 31

	.type	.L.str.20,@object               # @.str.20
	.p2align	4, 0x0
.L.str.20:
	.asciz	"Bitwise OR:      %u | %u = %u\n"
	.size	.L.str.20, 31

	.type	.L.str.21,@object               # @.str.21
	.p2align	4, 0x0
.L.str.21:
	.asciz	"Bitwise XOR:     %u ^ %u = %u\n"
	.size	.L.str.21, 31

	.type	.L.str.22,@object               # @.str.22
	.p2align	4, 0x0
.L.str.22:
	.asciz	"Bitwise NOT:     ~%u = %u\n"
	.size	.L.str.22, 27

	.type	.L.str.23,@object               # @.str.23
	.p2align	4, 0x0
.L.str.23:
	.asciz	"Shift Left:      %u << 1 = %u\n"
	.size	.L.str.23, 31

	.type	.L.str.24,@object               # @.str.24
	.p2align	4, 0x0
.L.str.24:
	.asciz	"Shift Right:     %u >> 1 = %u\n"
	.size	.L.str.24, 31

	.type	.L.str.25,@object               # @.str.25
	.p2align	4, 0x0
.L.str.25:
	.asciz	"Testing Logical Operators (&&, ||) and Ternary Operator (?:)\n\n"
	.size	.L.str.25, 63

	.type	.L.str.26,@object               # @.str.26
	.p2align	4, 0x0
.L.str.26:
	.asciz	"Testing Logical AND (&&):\n"
	.size	.L.str.26, 27

	.type	.L.str.27,@object               # @.str.27
	.p2align	4, 0x0
.L.str.27:
	.asciz	"a && b: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.27, 37

	.type	.L.str.28,@object               # @.str.28
	.p2align	4, 0x0
.L.str.28:
	.asciz	"a && 1: %d && 1 = %d (Expected: 1)\n"
	.size	.L.str.28, 36

	.type	.L.str.29,@object               # @.str.29
	.p2align	4, 0x0
.L.str.29:
	.asciz	"b && c: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.29, 37

	.type	.L.str.30,@object               # @.str.30
	.p2align	4, 0x0
.L.str.30:
	.asciz	"c && d: %d && %d = %d (Expected: 1)\n"
	.size	.L.str.30, 37

	.type	.L.str.31,@object               # @.str.31
	.p2align	4, 0x0
.L.str.31:
	.asciz	"0 && 0: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.31, 37

	.type	.L.str.32,@object               # @.str.32
	.p2align	4, 0x0
.L.str.32:
	.asciz	"1 && 0: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.32, 37

	.type	.L.str.33,@object               # @.str.33
	.p2align	4, 0x0
.L.str.33:
	.asciz	"1 && 1: %d && %d = %d (Expected: 1)\n"
	.size	.L.str.33, 37

	.type	.L.str.34,@object               # @.str.34
	.p2align	4, 0x0
.L.str.34:
	.asciz	"\nTesting Logical OR (||):\n"
	.size	.L.str.34, 27

	.type	.L.str.35,@object               # @.str.35
	.p2align	4, 0x0
.L.str.35:
	.asciz	"a || b: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.35, 37

	.type	.L.str.36,@object               # @.str.36
	.p2align	4, 0x0
.L.str.36:
	.asciz	"b || 0: %d || 0 = %d (Expected: 0)\n"
	.size	.L.str.36, 36

	.type	.L.str.37,@object               # @.str.37
	.p2align	4, 0x0
.L.str.37:
	.asciz	"0 || c: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.37, 37

	.type	.L.str.38,@object               # @.str.38
	.p2align	4, 0x0
.L.str.38:
	.asciz	"c || d: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.38, 37

	.type	.L.str.39,@object               # @.str.39
	.p2align	4, 0x0
.L.str.39:
	.asciz	"0 || 0: %d || %d = %d (Expected: 0)\n"
	.size	.L.str.39, 37

	.type	.L.str.40,@object               # @.str.40
	.p2align	4, 0x0
.L.str.40:
	.asciz	"1 || 0: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.40, 37

	.type	.L.str.41,@object               # @.str.41
	.p2align	4, 0x0
.L.str.41:
	.asciz	"1 || 1: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.41, 37

	.type	.L.str.42,@object               # @.str.42
	.p2align	4, 0x0
.L.str.42:
	.asciz	"\nTesting Ternary Operator (?:):\n"
	.size	.L.str.42, 33

	.type	.L.str.43,@object               # @.str.43
	.p2align	4, 0x0
.L.str.43:
	.asciz	"a ? c : d = %d ? %d : %d = %d (Expected: 5)\n"
	.size	.L.str.43, 45

	.type	.L.str.44,@object               # @.str.44
	.p2align	4, 0x0
.L.str.44:
	.asciz	"b ? c : d = %d ? %d : %d = %d (Expected: 10)\n"
	.size	.L.str.44, 46

	.type	.L.str.45,@object               # @.str.45
	.p2align	4, 0x0
.L.str.45:
	.asciz	"c > d ? c : d = %d > %d ? %d : %d = %d (Expected: 10)\n"
	.size	.L.str.45, 55

	.type	.L.str.46,@object               # @.str.46
	.p2align	4, 0x0
.L.str.46:
	.asciz	"c < d ? c : d = %d < %d ? %d : %d = %d (Expected: 5)\n"
	.size	.L.str.46, 54

	.type	.L.str.47,@object               # @.str.47
	.p2align	4, 0x0
.L.str.47:
	.asciz	"c == d ? c : d = %d == %d ? %d : %d = %d (Expected: 10)\n"
	.size	.L.str.47, 57

	.type	.L.str.48,@object               # @.str.48
	.p2align	4, 0x0
.L.str.48:
	.asciz	"Float Arithmetic Tests:\n\n"
	.size	.L.str.48, 26

	.type	.L.str.49,@object               # @.str.49
	.p2align	4, 0x0
.L.str.49:
	.asciz	"a + b:               %.2f + %.2f = %.2f\n"
	.size	.L.str.49, 41

	.type	.L.str.50,@object               # @.str.50
	.p2align	4, 0x0
.L.str.50:
	.asciz	"a - b:               %.2f - %.2f = %.2f\n"
	.size	.L.str.50, 41

	.type	.L.str.51,@object               # @.str.51
	.p2align	4, 0x0
.L.str.51:
	.asciz	"a * b:               %.2f * %.2f = %.2f\n"
	.size	.L.str.51, 41

	.type	.L.str.52,@object               # @.str.52
	.p2align	4, 0x0
.L.str.52:
	.asciz	"a / b:               %.2f / %.2f = %.2f\n"
	.size	.L.str.52, 41

	.type	.L.str.53,@object               # @.str.53
	.p2align	4, 0x0
.L.str.53:
	.asciz	"a + c:               %.2f + %.2f = %.2f\n"
	.size	.L.str.53, 41

	.type	.L.str.54,@object               # @.str.54
	.p2align	4, 0x0
.L.str.54:
	.asciz	"a - c:               %.2f - %.2f = %.2f\n"
	.size	.L.str.54, 41

	.type	.L.str.55,@object               # @.str.55
	.p2align	4, 0x0
.L.str.55:
	.asciz	"a * c:               %.2f * %.2f = %.2f\n"
	.size	.L.str.55, 41

	.type	.L.str.56,@object               # @.str.56
	.p2align	4, 0x0
.L.str.56:
	.asciz	"a / c:               %.2f / %.2f = %.2f\n"
	.size	.L.str.56, 41

	.type	.L.str.57,@object               # @.str.57
	.p2align	4, 0x0
.L.str.57:
	.asciz	"-a:                  -%.2f = %.2f\n"
	.size	.L.str.57, 35

	.type	.L.str.58,@object               # @.str.58
	.p2align	4, 0x0
.L.str.58:
	.asciz	"Operator Precedence Tests:\n\n"
	.size	.L.str.58, 29

	.type	.L.str.59,@object               # @.str.59
	.p2align	4, 0x0
.L.str.59:
	.asciz	"a + b * c:              %d + %d * %d = %d\n"
	.size	.L.str.59, 43

	.type	.L.str.60,@object               # @.str.60
	.p2align	4, 0x0
.L.str.60:
	.asciz	"a - b / c:              %d - %d / %d = %d\n"
	.size	.L.str.60, 43

	.type	.L.str.61,@object               # @.str.61
	.p2align	4, 0x0
.L.str.61:
	.asciz	"(a + b) * c:            (%d + %d) * %d = %d\n"
	.size	.L.str.61, 45

	.type	.L.str.62,@object               # @.str.62
	.p2align	4, 0x0
.L.str.62:
	.asciz	"a + b & c:              %d + %d & %d = %d\n"
	.size	.L.str.62, 43

	.type	.L.str.63,@object               # @.str.63
	.p2align	4, 0x0
.L.str.63:
	.asciz	"a | b * c:              %d | %d * %d = %d\n"
	.size	.L.str.63, 43

	.type	.L.str.64,@object               # @.str.64
	.p2align	4, 0x0
.L.str.64:
	.asciz	"(a | b) * c:            (%d | %d) * %d = %d\n"
	.size	.L.str.64, 45

	.type	.L.str.65,@object               # @.str.65
	.p2align	4, 0x0
.L.str.65:
	.asciz	"a << 1 + b >> 1:        %d << 1 + %d >> 1 = %d\n"
	.size	.L.str.65, 48

	.type	.L.str.66,@object               # @.str.66
	.p2align	4, 0x0
.L.str.66:
	.asciz	"(a << 1) + (b >> 1):    (%d << 1) + (%d >> 1) = %d\n"
	.size	.L.str.66, 52

	.type	.L.str.67,@object               # @.str.67
	.p2align	4, 0x0
.L.str.67:
	.asciz	"a < b || b & c:         %d < %d || %d & %d = %d\n"
	.size	.L.str.67, 49

	.type	.L.str.68,@object               # @.str.68
	.p2align	4, 0x0
.L.str.68:
	.asciz	"a < b || b && c:        %d < %d || %d && %d = %d\n"
	.size	.L.str.68, 50

	.type	.L.str.69,@object               # @.str.69
	.p2align	4, 0x0
.L.str.69:
	.asciz	"a + (b * c - d):        %d + (%d * %d - %d) = %d\n"
	.size	.L.str.69, 50

	.type	.L.str.70,@object               # @.str.70
	.p2align	4, 0x0
.L.str.70:
	.asciz	"(a + b) * (c + d):      (%d + %d) * (%d + %d) = %d\n"
	.size	.L.str.70, 52

	.type	.L.str.71,@object               # @.str.71
	.p2align	4, 0x0
.L.str.71:
	.asciz	"\nFloat Operator Precedence Tests:\n"
	.size	.L.str.71, 35

	.type	.L.str.72,@object               # @.str.72
	.p2align	4, 0x0
.L.str.72:
	.asciz	"x + y * z:              %.2f + %.2f * %.2f = %.2f\n"
	.size	.L.str.72, 51

	.type	.L.str.73,@object               # @.str.73
	.p2align	4, 0x0
.L.str.73:
	.asciz	"x - y / z:              %.2f - %.2f / %.2f = %.2f\n"
	.size	.L.str.73, 51

	.type	.L.str.74,@object               # @.str.74
	.p2align	4, 0x0
.L.str.74:
	.asciz	"(x + y) * z:            (%.2f + %.2f) * %.2f = %.2f\n"
	.size	.L.str.74, 53

	.type	.L.str.75,@object               # @.str.75
	.p2align	4, 0x0
.L.str.75:
	.asciz	"\nEdge Cases with Negative Values:\n"
	.size	.L.str.75, 35

	.type	.L.str.76,@object               # @.str.76
	.p2align	4, 0x0
.L.str.76:
	.asciz	"d * b / c:              %d * %d / %d = %d\n"
	.size	.L.str.76, 43

	.type	.L.str.77,@object               # @.str.77
	.p2align	4, 0x0
.L.str.77:
	.asciz	"d + a * -b:             %d + %d * -%d = %d\n"
	.size	.L.str.77, 44

	.type	.L.str.78,@object               # @.str.78
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.78:
	.asciz	"\n"
	.size	.L.str.78, 2

	.type	.L.str.79,@object               # @.str.79
.L.str.79:
	.asciz	"\n"
	.size	.L.str.79, 2

	.type	.L.str.80,@object               # @.str.80
.L.str.80:
	.asciz	"\n"
	.size	.L.str.80, 2

	.type	.L.str.81,@object               # @.str.81
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.81:
	.asciz	"Signed Integer Arithmetic and Bitwise Tests:\n"
	.size	.L.str.81, 46

	.type	.L.str.82,@object               # @.str.82
	.p2align	4, 0x0
.L.str.82:
	.asciz	"Addition:        %d + %d = %d\n"
	.size	.L.str.82, 31

	.type	.L.str.83,@object               # @.str.83
	.p2align	4, 0x0
.L.str.83:
	.asciz	"Subtraction:     %d - %d = %d\n"
	.size	.L.str.83, 31

	.type	.L.str.84,@object               # @.str.84
	.p2align	4, 0x0
.L.str.84:
	.asciz	"Multiplication:  %d * %d = %dd\n"
	.size	.L.str.84, 32

	.type	.L.str.85,@object               # @.str.85
	.p2align	4, 0x0
.L.str.85:
	.asciz	"Division:        %d / %d = %d\n"
	.size	.L.str.85, 31

	.type	.L.str.86,@object               # @.str.86
	.p2align	4, 0x0
.L.str.86:
	.asciz	"Division:        %d / %d = %d\n"
	.size	.L.str.86, 31

	.type	.L.str.87,@object               # @.str.87
	.p2align	4, 0x0
.L.str.87:
	.asciz	"Modulo:        %d %% %d = %d\n"
	.size	.L.str.87, 30

	.type	.L.str.88,@object               # @.str.88
	.p2align	4, 0x0
.L.str.88:
	.asciz	"Bitwise AND:     %d & %d = %d\n"
	.size	.L.str.88, 31

	.type	.L.str.89,@object               # @.str.89
	.p2align	4, 0x0
.L.str.89:
	.asciz	"Bitwise OR:      %d | %d = %d\n"
	.size	.L.str.89, 31

	.type	.L.str.90,@object               # @.str.90
	.p2align	4, 0x0
.L.str.90:
	.asciz	"Bitwise XOR:     %d ^ %d = %d\n"
	.size	.L.str.90, 31

	.type	.L.str.91,@object               # @.str.91
	.p2align	4, 0x0
.L.str.91:
	.asciz	"Bitwise NOT:     ~%d = %d\n"
	.size	.L.str.91, 27

	.type	.L.str.92,@object               # @.str.92
	.p2align	4, 0x0
.L.str.92:
	.asciz	"Shift Left:      %d << 1 = %d\n"
	.size	.L.str.92, 31

	.type	.L.str.93,@object               # @.str.93
	.p2align	4, 0x0
.L.str.93:
	.asciz	"Shift Right:     %d >> 1 = %d\n"
	.size	.L.str.93, 31

	.type	.L.str.94,@object               # @.str.94
	.p2align	4, 0x0
.L.str.94:
	.asciz	"Unsigned Integer Arithmetic and Bitwise Tests:\n"
	.size	.L.str.94, 48

	.type	.L.str.95,@object               # @.str.95
	.p2align	4, 0x0
.L.str.95:
	.asciz	"Addition:        %u + %u = %u\n"
	.size	.L.str.95, 31

	.type	.L.str.96,@object               # @.str.96
	.p2align	4, 0x0
.L.str.96:
	.asciz	"Subtraction:     %u - %u = %u\n"
	.size	.L.str.96, 31

	.type	.L.str.97,@object               # @.str.97
	.p2align	4, 0x0
.L.str.97:
	.asciz	"Multiplication:  %u * %u = %u\n"
	.size	.L.str.97, 31

	.type	.L.str.98,@object               # @.str.98
	.p2align	4, 0x0
.L.str.98:
	.asciz	"Division:        %u / %u = %u\n"
	.size	.L.str.98, 31

	.type	.L.str.99,@object               # @.str.99
	.p2align	4, 0x0
.L.str.99:
	.asciz	"Modulo:          %u %% %u = %u\n"
	.size	.L.str.99, 32

	.type	.L.str.100,@object              # @.str.100
	.p2align	4, 0x0
.L.str.100:
	.asciz	"Bitwise AND:     %u & %u = %u\n"
	.size	.L.str.100, 31

	.type	.L.str.101,@object              # @.str.101
	.p2align	4, 0x0
.L.str.101:
	.asciz	"Bitwise OR:      %u | %u = %u\n"
	.size	.L.str.101, 31

	.type	.L.str.102,@object              # @.str.102
	.p2align	4, 0x0
.L.str.102:
	.asciz	"Bitwise XOR:     %u ^ %u = %u\n"
	.size	.L.str.102, 31

	.type	.L.str.103,@object              # @.str.103
	.p2align	4, 0x0
.L.str.103:
	.asciz	"Bitwise NOT:     ~%u = %u\n"
	.size	.L.str.103, 27

	.type	.L.str.104,@object              # @.str.104
	.p2align	4, 0x0
.L.str.104:
	.asciz	"Shift Left:      %u << 1 = %u\n"
	.size	.L.str.104, 31

	.type	.L.str.105,@object              # @.str.105
	.p2align	4, 0x0
.L.str.105:
	.asciz	"Shift Right:     %u >> 1 = %u\n"
	.size	.L.str.105, 31

	.type	.L.str.106,@object              # @.str.106
	.p2align	4, 0x0
.L.str.106:
	.asciz	"Testing Logical Operators (&&, ||) and Ternary Operator (?:)\n\n"
	.size	.L.str.106, 63

	.type	.L.str.107,@object              # @.str.107
	.p2align	4, 0x0
.L.str.107:
	.asciz	"Testing Logical AND (&&):\n"
	.size	.L.str.107, 27

	.type	.L.str.108,@object              # @.str.108
	.p2align	4, 0x0
.L.str.108:
	.asciz	"a && b: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.108, 37

	.type	.L.str.109,@object              # @.str.109
	.p2align	4, 0x0
.L.str.109:
	.asciz	"a && 1: %d && 1 = %d (Expected: 1)\n"
	.size	.L.str.109, 36

	.type	.L.str.110,@object              # @.str.110
	.p2align	4, 0x0
.L.str.110:
	.asciz	"b && c: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.110, 37

	.type	.L.str.111,@object              # @.str.111
	.p2align	4, 0x0
.L.str.111:
	.asciz	"c && d: %d && %d = %d (Expected: 1)\n"
	.size	.L.str.111, 37

	.type	.L.str.112,@object              # @.str.112
	.p2align	4, 0x0
.L.str.112:
	.asciz	"0 && 0: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.112, 37

	.type	.L.str.113,@object              # @.str.113
	.p2align	4, 0x0
.L.str.113:
	.asciz	"1 && 0: %d && %d = %d (Expected: 0)\n"
	.size	.L.str.113, 37

	.type	.L.str.114,@object              # @.str.114
	.p2align	4, 0x0
.L.str.114:
	.asciz	"1 && 1: %d && %d = %d (Expected: 1)\n"
	.size	.L.str.114, 37

	.type	.L.str.115,@object              # @.str.115
	.p2align	4, 0x0
.L.str.115:
	.asciz	"Short-circuit test (b && ++c): %d && ++%d = %d (Expected: 0, c should not increment)\n"
	.size	.L.str.115, 86

	.type	.L.str.116,@object              # @.str.116
	.p2align	4, 0x0
.L.str.116:
	.asciz	"Value of c after short-circuit test: %d (Expected: 5)\n"
	.size	.L.str.116, 55

	.type	.L.str.117,@object              # @.str.117
	.p2align	4, 0x0
.L.str.117:
	.asciz	"\nTesting Logical OR (||):\n"
	.size	.L.str.117, 27

	.type	.L.str.118,@object              # @.str.118
	.p2align	4, 0x0
.L.str.118:
	.asciz	"a || b: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.118, 37

	.type	.L.str.119,@object              # @.str.119
	.p2align	4, 0x0
.L.str.119:
	.asciz	"b || 0: %d || 0 = %d (Expected: 0)\n"
	.size	.L.str.119, 36

	.type	.L.str.120,@object              # @.str.120
	.p2align	4, 0x0
.L.str.120:
	.asciz	"0 || c: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.120, 37

	.type	.L.str.121,@object              # @.str.121
	.p2align	4, 0x0
.L.str.121:
	.asciz	"c || d: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.121, 37

	.type	.L.str.122,@object              # @.str.122
	.p2align	4, 0x0
.L.str.122:
	.asciz	"0 || 0: %d || %d = %d (Expected: 0)\n"
	.size	.L.str.122, 37

	.type	.L.str.123,@object              # @.str.123
	.p2align	4, 0x0
.L.str.123:
	.asciz	"1 || 0: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.123, 37

	.type	.L.str.124,@object              # @.str.124
	.p2align	4, 0x0
.L.str.124:
	.asciz	"1 || 1: %d || %d = %d (Expected: 1)\n"
	.size	.L.str.124, 37

	.type	.L.str.125,@object              # @.str.125
	.p2align	4, 0x0
.L.str.125:
	.asciz	"Short-circuit test (a || ++d): %d || ++%d = %d (Expected: 1, d should not increment)\n"
	.size	.L.str.125, 86

	.type	.L.str.126,@object              # @.str.126
	.p2align	4, 0x0
.L.str.126:
	.asciz	"Value of d after short-circuit test: %d (Expected: 10)\n"
	.size	.L.str.126, 56

	.type	.L.str.127,@object              # @.str.127
	.p2align	4, 0x0
.L.str.127:
	.asciz	"\nTesting Ternary Operator (?:):\n"
	.size	.L.str.127, 33

	.type	.L.str.128,@object              # @.str.128
	.p2align	4, 0x0
.L.str.128:
	.asciz	"a ? c : d = %d ? %d : %d = %d (Expected: 5)\n"
	.size	.L.str.128, 45

	.type	.L.str.129,@object              # @.str.129
	.p2align	4, 0x0
.L.str.129:
	.asciz	"b ? c : d = %d ? %d : %d = %d (Expected: 10)\n"
	.size	.L.str.129, 46

	.type	.L.str.130,@object              # @.str.130
	.p2align	4, 0x0
.L.str.130:
	.asciz	"c > d ? c : d = %d > %d ? %d : %d = %d (Expected: 10)\n"
	.size	.L.str.130, 55

	.type	.L.str.131,@object              # @.str.131
	.p2align	4, 0x0
.L.str.131:
	.asciz	"c < d ? c : d = %d < %d ? %d : %d = %d (Expected: 5)\n"
	.size	.L.str.131, 54

	.type	.L.str.132,@object              # @.str.132
	.p2align	4, 0x0
.L.str.132:
	.asciz	"c == d ? c : d = %d == %d ? %d : %d = %d (Expected: 10)\n"
	.size	.L.str.132, 57

	.type	.L.str.133,@object              # @.str.133
	.p2align	4, 0x0
.L.str.133:
	.asciz	"Short-circuit test in ternary operator:\n"
	.size	.L.str.133, 41

	.type	.L.str.134,@object              # @.str.134
	.p2align	4, 0x0
.L.str.134:
	.asciz	"result = a ? (c += 2) : d; with a = %d, c = %d => result = %d (Expected: 7)\n"
	.size	.L.str.134, 77

	.type	.L.str.135,@object              # @.str.135
	.p2align	4, 0x0
.L.str.135:
	.asciz	"Value of c after ternary: %d (Expected: 7)\n"
	.size	.L.str.135, 44

	.type	.L.str.136,@object              # @.str.136
	.p2align	4, 0x0
.L.str.136:
	.asciz	"result = b ? c : (d += 3); with b = %d, d = %d => result = %d (Expected: 13)\n"
	.size	.L.str.136, 78

	.type	.L.str.137,@object              # @.str.137
	.p2align	4, 0x0
.L.str.137:
	.asciz	"Value of d after ternary: %d (Expected: 13)\n"
	.size	.L.str.137, 45

	.type	.L.str.138,@object              # @.str.138
	.p2align	4, 0x0
.L.str.138:
	.asciz	"Float Arithmetic Tests:\n\n"
	.size	.L.str.138, 26

	.type	.L.str.139,@object              # @.str.139
	.p2align	4, 0x0
.L.str.139:
	.asciz	"a + b:               %.2f + %.2f = %.2f\n"
	.size	.L.str.139, 41

	.type	.L.str.140,@object              # @.str.140
	.p2align	4, 0x0
.L.str.140:
	.asciz	"a - b:               %.2f - %.2f = %.2f\n"
	.size	.L.str.140, 41

	.type	.L.str.141,@object              # @.str.141
	.p2align	4, 0x0
.L.str.141:
	.asciz	"a * b:               %.2f * %.2f = %.2f\n"
	.size	.L.str.141, 41

	.type	.L.str.142,@object              # @.str.142
	.p2align	4, 0x0
.L.str.142:
	.asciz	"a / b:               %.2f / %.2f = %.2f\n"
	.size	.L.str.142, 41

	.type	.L.str.143,@object              # @.str.143
	.p2align	4, 0x0
.L.str.143:
	.asciz	"a + c:               %.2f + %.2f = %.2f\n"
	.size	.L.str.143, 41

	.type	.L.str.144,@object              # @.str.144
	.p2align	4, 0x0
.L.str.144:
	.asciz	"a - c:               %.2f - %.2f = %.2f\n"
	.size	.L.str.144, 41

	.type	.L.str.145,@object              # @.str.145
	.p2align	4, 0x0
.L.str.145:
	.asciz	"a * c:               %.2f * %.2f = %.2f\n"
	.size	.L.str.145, 41

	.type	.L.str.146,@object              # @.str.146
	.p2align	4, 0x0
.L.str.146:
	.asciz	"a / c:               %.2f / %.2f = %.2f\n"
	.size	.L.str.146, 41

	.type	.L.str.147,@object              # @.str.147
	.p2align	4, 0x0
.L.str.147:
	.asciz	"-a:                  -%.2f = %.2f\n"
	.size	.L.str.147, 35

	.type	.L.str.148,@object              # @.str.148
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.148:
	.asciz	"\n"
	.size	.L.str.148, 2

	.type	.L.str.149,@object              # @.str.149
.L.str.149:
	.asciz	"\n"
	.size	.L.str.149, 2

	.type	.L.str.150,@object              # @.str.150
.L.str.150:
	.asciz	"\n"
	.size	.L.str.150, 2

	.type	.L.str.151,@object              # @.str.151
.L.str.151:
	.asciz	"\n"
	.size	.L.str.151, 2

	.type	.L.str.152,@object              # @.str.152
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4, 0x0
.L.str.152:
	.asciz	"Test with constant expressions:\n"
	.size	.L.str.152, 33

	.type	.L.str.153,@object              # @.str.153
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.153:
	.asciz	"\n"
	.size	.L.str.153, 2

	.section	".note.GNU-stack","",@progbits

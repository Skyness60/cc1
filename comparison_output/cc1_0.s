	.file	"cc1"
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushl	%eax
	.cfi_def_cfa_offset 8
	movl	$42, (%esp)
	movl	(%esp), %eax
	popl	%ecx
	.cfi_def_cfa_offset 4
	retl
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits

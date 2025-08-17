/* Minimal C header describing C89 sizes/alignments for i386 vs x86_64 (SysV). */
#ifndef ABI_MATRIX_H
#define ABI_MATRIX_H

#if defined(__x86_64__) || defined(_M_X64)
# define ABI_IS_X86_64 1
#else
# define ABI_IS_X86_64 0
#endif

/* These macros are purely informative for tests; the compiler proper
 * must use its own target description (front::semantics::target.rs). */
#if ABI_IS_X86_64
# define SIZEOF_CHAR 1
# define SIZEOF_SHORT 2
# define SIZEOF_INT 4
# define SIZEOF_LONG 8
# define SIZEOF_LONG_LONG 8
# define SIZEOF_POINTER 8
# define SIZEOF_FLOAT 4
# define SIZEOF_DOUBLE 8
# define SIZEOF_LONG_DOUBLE 16
#else /* i386 */
# define SIZEOF_CHAR 1
# define SIZEOF_SHORT 2
# define SIZEOF_INT 4
# define SIZEOF_LONG 4
# define SIZEOF_LONG_LONG 8
# define SIZEOF_POINTER 4
# define SIZEOF_FLOAT 4
# define SIZEOF_DOUBLE 8
# define SIZEOF_LONG_DOUBLE 12 /* SysV i386 */
#endif

#endif /* ABI_MATRIX_H */

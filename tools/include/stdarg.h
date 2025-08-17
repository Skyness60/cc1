#ifndef __FCC_STUB_STDARG_H__
#define __FCC_STUB_STDARG_H__

/* Minimal, semantics-only stdarg stub for tests */
typedef char* va_list;
#define va_start(ap, last) ((void)0)
#define va_end(ap) ((void)0)
/* Return a zero value of the requested type to satisfy typing */
#define va_arg(ap, T) ((T)0)

#endif

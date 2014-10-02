#ifndef _SYS_CDEFS
#define _SYS_CDEFS 1
 
#define __myos_libc 1
 
#if defined(__cplusplus)
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif
 
#endif

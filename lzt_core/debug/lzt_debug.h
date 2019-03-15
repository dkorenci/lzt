
#ifndef LZT_DEBUG_H
#define	LZT_DEBUG_H

#ifdef LZT_DEBUG

// libcwd
#define CWDEBUG 

#ifndef _GNU_SOURCE                     // Already defined by g++ 3.0 and higher.
#define _GNU_SOURCE                     // This must be defined before including <libcwd/sys.h>
#endif
#include <libcwd/sys.h>                 // This must be the first header file
// This line should actually be part of a custom "debug.h" file.  See tutorial 2.
#include <libcwd/debug.h>


#endif	//LZT_DEBUG

#endif /* LZT_DEBUG_H */
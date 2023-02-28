/*
 * Copyright (c) 2003, 2007-8 Matteo Frigo
 * Copyright (c) 2003, 2007-8 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */


/* machine-dependent cycle counters code. Needs to be inlined. */

/***************************************************************************/
/* To use the cycle counters in your code, simply #include "cycle.h" (this
   file), and then use the functions/macros:

                 ticks getticks(void);

   ticks is an opaque typedef defined below, representing the current time.
   You extract the elapsed time between two calls to gettick() via:

                 double elapsed(ticks t1, ticks t0);

   which returns a double-precision variable in arbitrary units.  You
   are not expected to convert this into human units like seconds; it
   is intended only for *comparisons* of time intervals.

   (In order to use some of the OS-dependent timer routines like
   Solaris' gethrtime, you need to paste the autoconf snippet below
   into your configure.ac file and #include "config.h" before cycle.h,
   or define the relevant macros manually if you are not using autoconf.)
*/

/***************************************************************************/
/* This file uses macros like HAVE_GETHRTIME that are assumed to be
   defined according to whether the corresponding function/type/header
   is available on your system.  The necessary macros are most
   conveniently defined if you are using GNU autoconf, via the tests:
   
   dnl ---------------------------------------------------------------------

   AC_C_INLINE
   AC_HEADER_TIME
   AC_CHECK_HEADERS([sys/time.h c_asm.h intrinsics.h mach/mach_time.h])

   AC_CHECK_TYPE([hrtime_t],[AC_DEFINE(HAVE_HRTIME_T, 1, [Define to 1 if hrtime_t is defined in <sys/time.h>])],,[#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif])

   AC_CHECK_FUNCS([gethrtime read_real_time time_base_to_time clock_gettime mach_absolute_time])

   dnl Cray UNICOS _rtc() (real-time clock) intrinsic
   AC_MSG_CHECKING([for _rtc intrinsic])
   rtc_ok=yes
   AC_TRY_LINK([#ifdef HAVE_INTRINSICS_H
#include <intrinsics.h>
#endif], [_rtc()], [AC_DEFINE(HAVE__RTC,1,[Define if you have the UNICOS _rtc() intrinsic.])], [rtc_ok=no])
   AC_MSG_RESULT($rtc_ok)

   dnl ---------------------------------------------------------------------
*/

/***************************************************************************/


#include <time.h>

/*
 * ARM cycle counter; WARNING: many Linux distributions do not reliably enable user-mode access
 */
typedef unsigned ticks;

static __inline__ ticks getticks(void)
{
     unsigned cc;
     static int init = 0;
     if(!init) {
         __asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 2" :: "r"(1<<31)); /* stop the cc */
         __asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 0" :: "r"(5));     /* initialize */
         __asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 1" :: "r"(1<<31)); /* start the cc */
         init = 1;
     }
     __asm__ __volatile__ ("mrc p15, 0, %0, c9, c13, 0" : "=r"(cc));
     return cc;
}

unsigned elapsed(ticks t1, ticks t0)
{
     return t1 - t0;
}

static __inline__ void start_timer(void){

	// init the timer to be 0
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 2" :: "r"(1<<31)); /* stop the cc */
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 0" :: "r"(5));     /* initialize */
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 1" :: "r"(1<<31)); /* start the cc */

}

static __inline ticks stop_timer(void){
	unsigned cc;
	 __asm__ __volatile__ ("mrc p15, 0, r10, c9, c13, 0");
	 __asm__ __volatile__ ("mov %0, r10" : "=r"(cc));
	 return cc;
}

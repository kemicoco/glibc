/* __sigsetjmp_cancel for thread cancellation.  Generic version.
   Copyright (C) 2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_SETJMP_CANCEL_H
#define _BITS_SETJMP_CANCEL_H  1

#if !defined _SETJMP_H && !defined _PTHREAD_H
# error "Never include <bits/setjmp-cancel.h> directly; use <setjmp.h> instead."
#endif

#define __sigsetjmp_cancel __sigsetjmp

struct __jmp_buf_tag;
/* Function used in the macros.  */
extern int __sigsetjmp (struct __jmp_buf_tag *__env,
			int __savemask) __THROWNL;

#endif  /* bits/setjmp-cancel.h */

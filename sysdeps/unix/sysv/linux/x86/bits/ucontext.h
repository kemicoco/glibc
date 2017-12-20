/* Copyright (C) 2017 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_UCONTEXT_H
#define _BITS_UCONTEXT_H  1

#if !defined _UCONTEXT_H
# error "Never use <bits/ucontext.h> directly; include <ucontext.h> instead."
#endif

#if defined(__SHSTK__) && !defined(_LIBC)
# error -mshstk incompatible with ucontext.h APIs
#endif

#endif /* _BITS_UCONTEXT_H */

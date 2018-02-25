/* Internal header file for <bits/setjmp-cancel.h>.  x86 version.
   Copyright (C) 2017-2018 Free Software Foundation, Inc.
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

extern int __setjmp_cancel (struct __jmp_buf_tag __env[1])
  __attribute__((__returns_twice__));
libc_hidden_proto (__setjmp_cancel, __returns_twice__)

extern void __longjmp_cancel (__jmp_buf __env, int __val)
     __attribute__ ((__noreturn__)) attribute_hidden;

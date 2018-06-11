/* Copyright (C) 2018 Free Software Foundation, Inc.
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

#ifndef _BITS_PRCTL_H
#define _BITS_PRCTL_H	1

#ifndef ARCH_CET_STATUS
/* CET features:
   IBT:   GNU_PROPERTY_X86_FEATURE_1_IBT
   SHSTK: GNU_PROPERTY_X86_FEATURE_1_SHSTK
 */
/* Return CET features in unsigned long *addr:
     features: addr[0].
     always_on: addr[1].
     shadow stack size for a new image: addr[2].
 */
# define ARCH_CET_STATUS	0x3001
/* Disable CET features in unsigned int features.  */
# define ARCH_CET_DISABLE	0x3002
/* Lock all CET features.  */
# define ARCH_CET_LOCK		0x3003
/* Control how CET features should be enabled when launching a new image.
   unsigned long *addr:
     features: addr[0].
     control: addr[1].
     shadow stack size for a new image: addr[2].
 */
# define ARCH_CET_EXEC		0x3004
/* Allocate a new shadow stack with __syscall_ulong_t *addr:
     IN: requested shadow stack size: *addr.
     OUT: allocated shadow stack address: *addr.
 */
# define ARCH_CET_ALLOC_SHSTK	0x3005
/* Restore shadow stack pointer to __syscall_ulong_t ssp. */
# define ARCH_CET_RSTOR_SHSTK	0x3006
/* Return legacy region bitmap info in unsigned long *addr:
     address: addr[0].
     size: addr[1].
 */
# define ARCH_CET_LEGACY_BITMAP	0x3007

/* Valid control values of ARCH_CET_EXEC:
   0: Enable CET features based on ELF property note.
   1: Always disable CET features.
   2: Always enable CET features.
 */
# define ARCH_CET_EXEC_ELF_PROPERTY	0
# define ARCH_CET_EXEC_ALWAYS_OFF	1
# define ARCH_CET_EXEC_ALWAYS_ON	2
# define ARCH_CET_EXEC_MAX		ARCH_CET_EXEC_ALWAYS_ON
#endif /* ARCH_CET_STATUS */

#endif  /* bits/prctl.h */

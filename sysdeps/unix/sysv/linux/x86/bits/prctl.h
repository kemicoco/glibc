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

#ifndef ARCH_CET_CONTROL_STATUS
/* CET features:
   IBT:   GNU_PROPERTY_X86_FEATURE_1_IBT
   SHSTK: GNU_PROPERTY_X86_FEATURE_1_SHSTK
 */
/* Return CET features in unsigned int *features_p.  */
# define ARCH_CET_CONTROL_STATUS		0x3001
/* Disable CET features in unsigned int features.  */
# define ARCH_CET_CONTROL_DISABLE		0x3002
/* Lock all CET features.  */
# define ARCH_CET_CONTROL_LOCK			0x3003
/* Mark a legacy region from unsigned long *addr:
   address: addr[0]
   size: addr[1].
 */
# define ARCH_CET_CONTROL_MARK_LEGACY_REGION	0x3004
/* Turn on CET_EXEC_ENABLED features in unsigned int features.  */
# define ARCH_CET_CONTROL_EXEC_ON		0x3005
/* Turn off CET_EXEC_ENABLED features in unsigned int features.  */
# define ARCH_CET_CONTROL_EXEC_OFF		0x3006
/* Exit with int status.  */
# define ARCH_CET_CONTROL_EXIT			0x3007
#endif /* ARCH_CET_CONTROL_STATUS */

#endif  /* bits/prctl.h */

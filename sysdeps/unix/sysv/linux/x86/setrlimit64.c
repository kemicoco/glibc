/* Linux/x86 setrlimit64 implementation (64 bits off_t).
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

#include <errno.h>
#include <sys/types.h>
#include <ldsodefs.h>
#include <shlib-compat.h>

/* Add this redirection so the strong_alias for __RLIM_T_MATCHES_RLIM64_T
   linking setrlimit64 to {__}setrlimit does not throw a type error.  */
#undef setrlimit
#undef __setrlimit
#define setrlimit setrlimit_redirect
#define __setrlimit __setrlimit_redirect
#include <sys/resource.h>
#undef setrlimit
#undef __setrlimit

/* Set the soft and hard limits for RESOURCE to *RLIMITS.
   Only the super-user can increase hard limits.
   Return 0 if successful, -1 if not (and sets errno).  */
int
__setrlimit64 (enum __rlimit_resource resource, const struct rlimit64 *rlimits)
{
  struct rlimit64 rlim64 = *rlimits;

  if (resource == RLIMIT_AS)
    {
      if (rlim64.rlim_cur != RLIM_INFINITY)
	rlim64.rlim_cur += GL(dl_x86_legacy_bitmap)[1];
      if (rlim64.rlim_max != RLIM_INFINITY)
	rlim64.rlim_max += GL(dl_x86_legacy_bitmap)[1];
    }

  return INLINE_SYSCALL_CALL (prlimit64, 0, resource, &rlim64, NULL);
}
weak_alias (__setrlimit64, setrlimit64)

#if __RLIM_T_MATCHES_RLIM64_T
strong_alias (__setrlimit64, __setrlimit)
weak_alias (__setrlimit64, setrlimit)
# ifdef SHARED
__hidden_ver1 (__setrlimit64, __GI___setrlimit, __setrlimit64);
# endif
#endif

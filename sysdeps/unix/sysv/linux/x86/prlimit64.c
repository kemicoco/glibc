/* Linux/x86 prlimit64 implementation (64 bits off_t).
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
#include <sys/syscall.h>
#include <ldsodefs.h>

/* Add this redirection so the strong_alias for __RLIM_T_MATCHES_RLIM64_T
   linking prlimit64 to prlimit does not throw a type error.  */
#undef prlimit
#define prlimit prlimit_redirect
#include <sys/resource.h>
#undef prlimit

int
prlimit64 (__pid_t pid, enum __rlimit_resource resource,
	   const struct rlimit64 *new_rlimit, struct rlimit64 *old_rlimit)
{
  struct rlimit64 new_rlimit64_mem;
  struct rlimit64 *new_rlimit64 = NULL;

  if (new_rlimit != NULL)
    {
      new_rlimit64_mem = *new_rlimit;
      if (resource == RLIMIT_AS)
	{
	  if (new_rlimit64_mem.rlim_cur != RLIM_INFINITY)
	    new_rlimit64_mem.rlim_cur += GL(dl_x86_legacy_bitmap)[1];
	  if (new_rlimit64_mem.rlim_max != RLIM_INFINITY)
	    new_rlimit64_mem.rlim_max += GL(dl_x86_legacy_bitmap)[1];
	}
      new_rlimit64 = &new_rlimit64_mem;
    }

  int res = INLINE_SYSCALL (prlimit64, 4, pid, resource, new_rlimit64,
			    old_rlimit);

  if (res == 0 && resource == RLIMIT_AS)
    {
      if (old_rlimit->rlim_cur != RLIM_INFINITY)
	old_rlimit->rlim_cur -= GL(dl_x86_legacy_bitmap)[1];
      if (old_rlimit->rlim_max != RLIM_INFINITY)
	old_rlimit->rlim_max -= GL(dl_x86_legacy_bitmap)[1];
    }

  return res;
}

#if __RLIM_T_MATCHES_RLIM64_T
weak_alias (prlimit64, prlimit)
# ifdef SHARED
#if 0
__hidden_ver1 (__setrlimit64, __GI___setrlimit, __setrlimit64);
# endif
# endif
#endif

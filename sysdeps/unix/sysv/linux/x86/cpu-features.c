/* Initialize CPU feature data for Linux/x86.
   This file is part of the GNU C Library.
   Copyright (C) 2018 Free Software Foundation, Inc.

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

#ifdef __CET__
# define init_cpu_features x86_init_cpu_features
#endif

#include_next <cpu-features.c>

#ifdef __CET__
# undef init_cpu_features
# include <sys/syscall.h>
# include <sys/prctl.h>

static inline void
init_cpu_features (struct cpu_features *cpu_features)
{
  /* Check CET status.  */
  unsigned long cet_status[3];
  INTERNAL_SYSCALL_DECL (err);
  int res = INTERNAL_SYSCALL (arch_prctl, err, 2, ARCH_CET_STATUS,
			      cet_status);

  if (res == 0)
    {
      /* Update dl_x86_feature_1.  */
      GL(dl_x86_feature_1)[0] = cet_status[0];
      GL(dl_x86_feature_1)[1] = cet_status[1];
    }

  x86_init_cpu_features (cpu_features);
}
#endif

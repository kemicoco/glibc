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
  __syscall_ulong_t cet_status[4];
  INTERNAL_SYSCALL_DECL (err);
  int res = INTERNAL_SYSCALL (arch_prctl, err, 2, ARCH_CET_STATUS_NEW,
			      cet_status);

  /* Update dl_x86_feature_1.  */
  if (res == 0)
    {
      GL(dl_x86_feature_1)[0] = cet_status[0];
      GL(dl_x86_feature_1)[1] = cet_status[1];
      GL(dl_x86_ssp)[0] = cet_status[2];
      GL(dl_x86_ssp)[1] = cet_status[3];
    }

  x86_init_cpu_features (cpu_features);

  if (res == 0)
    {
# ifndef SHARED
      /* Check if IBT and SHSTK are enabled by kernel.  */
      if ((GL(dl_x86_feature_1)[0] & GNU_PROPERTY_X86_FEATURE_1_IBT)
	  || (GL(dl_x86_feature_1)[0] & GNU_PROPERTY_X86_FEATURE_1_SHSTK))
	{
	  /* Disable IBT and/or SHSTK if they are enabled by kernel, but
	     disabled by environment variable:

	     GLIBC_TUNABLES=glibc.tune.hwcaps=-IBT,-SHSTK
	   */
	  unsigned int cet_feature = 0;
	  if (!HAS_CPU_FEATURE (IBT))
	    cet_feature |= GNU_PROPERTY_X86_FEATURE_1_IBT;
	  if (!HAS_CPU_FEATURE (SHSTK))
	    cet_feature |= GNU_PROPERTY_X86_FEATURE_1_SHSTK;

	  if (cet_feature)
	    {
	      INTERNAL_SYSCALL_DECL (err);
	      int res = INTERNAL_SYSCALL (arch_prctl, err, 2,
					  ARCH_CET_DISABLE,
					  cet_feature);

	      /* Clear the disabled bits in dl_x86_feature_1.  */
	      if (res == 0)
		GL(dl_x86_feature_1)[0] &= ~cet_feature;
	    }

	  /* Lock CET if IBT or SHSTK is enabled in executable.  Don't
	     lock CET if SHSTK is enabled permissively.  */
	  if (((cet_status[1] >> ARCH_CET_EXEC_MAX)
	       & ((1 << ARCH_CET_EXEC_MAX) - 1))
	       != ARCH_CET_EXEC_PERMISSIVE)
	      {
		INTERNAL_SYSCALL_DECL (err);
		INTERNAL_SYSCALL (arch_prctl, err, 2, ARCH_CET_LOCK, 0);
	      }
	}
# endif
    }
}

# ifndef SHARED
static inline void
x86_setup_tls (void)
{
  __libc_setup_tls ();
  THREAD_SETMEM (THREAD_SELF, header.feature_1, GL(dl_x86_feature_1)[0]);
  THREAD_SETMEM (THREAD_SELF, header.ssp.base, GL(dl_x86_ssp)[0]);
  THREAD_SETMEM (THREAD_SELF, header.ssp.size, GL(dl_x86_ssp)[1]);
  THREAD_SETMEM (THREAD_SELF, header.ssp.limit,
		 GL(dl_x86_ssp)[0] + GL(dl_x86_ssp)[1]);
}

#  define ARCH_SETUP_TLS() x86_setup_tls ()
# endif
#endif

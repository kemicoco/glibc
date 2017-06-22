/* Linux/x86 CET initializers function.
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

#include <unistd.h>
#include <errno.h>
#include <libintl.h>
#include <ldsodefs.h>

static int
dl_cet_mark_legacy_region (struct link_map *l)
{
  /* FIXME: Mark legacy region  */
  return 0;
}

/* Check if object M is compatible with CET.  */

static void
dl_cet_check (struct link_map *m, const char *program)
{
  /* Check if IBT is enabled by kernel.  */
  bool ibt_enabled
    = (GL(dl_x86_feature_1)[0] & GNU_PROPERTY_X86_FEATURE_1_IBT) != 0;
  /* Check if SHSTK is enabled by kernel.  */
  bool shstk_enabled
    = (GL(dl_x86_feature_1)[0] & GNU_PROPERTY_X86_FEATURE_1_SHSTK) != 0;

  if (ibt_enabled || shstk_enabled)
    {
      struct link_map *l = NULL;

      /* Check if IBT and SHSTK are enabled in object.  */
      bool enable_ibt = ibt_enabled;
      bool enable_shstk = shstk_enabled;
      if (program)
	{
	  /* Enable IBT and SHSTK only if they are enabled in executable.
	     NB: IBT and SHSTK may be disabled by environment variable:

	     GLIBC_TUNABLES=glibc.tune.hwcaps=-IBT,-SHSTK
	   */
	  enable_ibt &= (HAS_CPU_FEATURE (IBT)
			 && (m->l_cet & lc_ibt) != 0);
	  enable_shstk &= (HAS_CPU_FEATURE (SHSTK)
			   && (m->l_cet & lc_shstk) != 0);
	}

      /* ld.so is CET-enabled by kernel.  But shared objects may not
	 support IBT nor SHSTK.  */
      if (enable_ibt || enable_shstk)
	{
	  int res;
	  unsigned int i;
	  unsigned int first_legacy, last_legacy;
	  bool need_legacy_bitmap = false;

	  i = m->l_searchlist.r_nlist;
	  while (i-- > 0)
	    {
	      /* Check each shared object to see if IBT and SHSTK are
		 enabled.  */
	      l = m->l_initfini[i];

	      if (l->l_init_called)
		continue;

#ifdef SHARED
	      /* Skip CET check for ld.so since ld.so is CET-enabled.
		 CET will be disabled later if CET isn't enabled in
		 executable.  */
	      if (l == &GL(dl_rtld_map)
		  ||  l->l_real == &GL(dl_rtld_map)
		  || (program && l == m))
		continue;
#endif

	      if (enable_ibt && !(l->l_cet & lc_ibt))
		{
		  /* Remember the first and last legacy objects.  */
		  if (!need_legacy_bitmap)
		    last_legacy = i;
		  first_legacy = i;
		  need_legacy_bitmap = true;
		}

	      /* SHSTK is enabled only if it is enabled in executable as
		 well as all shared objects.  */
	      enable_shstk &= (l->l_cet & lc_shstk) != 0;
	    }

	  if (need_legacy_bitmap)
	    {
	      /* Put legacy shared objects in legacy bitmap.  */
	      for (i = first_legacy; i <= last_legacy; i++)
		{
		  l = m->l_initfini[i];

		  if (l->l_init_called || (l->l_cet & lc_ibt))
		    continue;

#ifdef SHARED
		  if (l == &GL(dl_rtld_map)
		      ||  l->l_real == &GL(dl_rtld_map)
		      || (program && l == m))
		    continue;
#endif

		  /* If IBT is enabled in executable and IBT isn't enabled
		     in this shard object, mark PT_LOAD segments with PF_X
		     in legacy code page bitmap.  */
		  res = dl_cet_mark_legacy_region (l);
		  if (res != 0)
		    {
		      if (program)
			_dl_fatal_printf ("%s: fail to mark legacy code region\n",
					  l->l_name);
		      else
			_dl_signal_error (-res, l->l_name, "dlopen",
					  N_("fail to mark legacy code region"));
		    }
		}
	    }
	}

      if (enable_ibt != ibt_enabled || enable_shstk != shstk_enabled)
	{
	  if (!program)
	    {
	      /* When SHSTK is enabled, we can't dlopening a shared
		 object without SHSTK.  */
	      if (enable_shstk != shstk_enabled)
		_dl_signal_error (EINVAL, l->l_name, "dlopen",
				  N_("shadow stack isn't enabled"));
	      return;
	    }

#ifdef SHARED
	  /* FIXME: Disable IBT and/or SHSTK if they are enabled in
	     ld.so, but disabled in executable or shared objects.  */
	  ;
#endif
	}

#ifdef SHARED
      if (program && (ibt_enabled || shstk_enabled))
	{
	  /* FIXME: Lock CET if IBT or SHSTK is enabled in executable.  */
	  ;
	}
#endif
    }
}

void
_dl_cet_open_check (struct link_map *l)
{
  dl_cet_check (l, NULL);
}

#ifdef SHARED

# ifndef LINKAGE
#  define LINKAGE
# endif

LINKAGE
void
_dl_cet_check (struct link_map *main_map, const char *program)
{
  dl_cet_check (main_map, program);
}
#endif /* SHARED */

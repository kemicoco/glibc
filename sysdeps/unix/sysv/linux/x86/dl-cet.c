/* Linux/x86 CET initializers function.
   Copyright (C) 2017 Free Software Foundation, Inc.

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
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

static int
dl_cet_mark_legacy_region (struct link_map *l)
{
  /* Mark PT_LOAD segments with PF_X in legacy code page bitmap.  */
  size_t i, phnum = l->l_phnum;
  const ElfW(Phdr) *phdr = l->l_phdr;
#ifdef __x86_64__
  typedef unsigned long long word_t;
#else
  typedef unsigned long word_t;
#endif
  unsigned int bits_to_set;
  word_t mask_to_set;
#define BITS_PER_WORD (sizeof (word_t) * 8)
#define BITMAP_FIRST_WORD_MASK(start) \
  (~((word_t) 0) << ((start) & (BITS_PER_WORD - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) \
  (~((word_t) 0) >> (-(nbits) & (BITS_PER_WORD - 1)))

  word_t *bitmap = (word_t *) GL(dl_x86_legacy_bitmap)[0];
  word_t bitmap_size = GL(dl_x86_legacy_bitmap)[1];
  word_t *p;
  size_t page_size = GLRO(dl_pagesize);

  for (i = 0; i < phnum; i++)
    if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X))
      {
	/* One bit in legacy bitmap represents a page.  */
	ElfW(Addr) start = (phdr[i].p_vaddr + l->l_addr) / page_size;
	ElfW(Addr) len = (phdr[i].p_memsz + page_size - 1) / page_size;
	ElfW(Addr) end = start + len;

	if ((end / 8) > bitmap_size)
	  return -EFAULT;

	p = bitmap + (start / BITS_PER_WORD);
	bits_to_set = BITS_PER_WORD - (start % BITS_PER_WORD);
	mask_to_set = BITMAP_FIRST_WORD_MASK (start);

	while (len >= bits_to_set)
	  {
	    *p |= mask_to_set;
	    len -= bits_to_set;
	    bits_to_set = BITS_PER_WORD;
	    mask_to_set = ~((word_t) 0);
	    p++;
	  }
	if (len)
	  {
	    mask_to_set &= BITMAP_LAST_WORD_MASK (end);
	    *p |= mask_to_set;
	  }
      }

  return 0;
}

/* Check if object M is compatible with CET.  */

static void
dl_cet_check (struct link_map *m, const char *program)
{
  /* Check if IBT is enabled by kernel.  */
  bool ibt_enabled
    = (GL(dl_x86_feature_1) & GNU_PROPERTY_X86_FEATURE_1_IBT) != 0;
  /* Check if SHSTK is enabled by kernel.  */
  bool shstk_enabled
    = (GL(dl_x86_feature_1) & GNU_PROPERTY_X86_FEATURE_1_SHSTK) != 0;

  if (ibt_enabled || shstk_enabled)
    {
      struct link_map *l = NULL;

      /* Check if IBT and SHSTK are enabled in object.  */
      bool enable_ibt = ibt_enabled;
      bool enable_shstk = shstk_enabled;
      if (program)
	{
	  /* Enable IBT and SHSTK onlyf if they are enabled in
	     executable.  */
	  enable_ibt &= (m->l_cet & lc_ibt) != 0;
	  enable_shstk &= (m->l_cet & lc_shstk) != 0;
	}

      /* ld.so is CET-enabled by kernel.  But shared objects may not
	 support IBT nor SHSTK.  */
      if (enable_ibt || enable_shstk)
	{
	  int res;
	  unsigned int i;
	  bool legacy_bitmap_enabled
	    = (GL(dl_x86_legacy_bitmap)[0] != 0
	       && __mprotect ((void *) GL(dl_x86_legacy_bitmap)[0],
			      GL(dl_x86_legacy_bitmap)[1],
			      PROT_READ|PROT_WRITE) == 0);

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
		  /* If IBT is enabled in executable and IBT isn't
		     enabled in this shard object, mark PT_LOAD segments
		     with PF_X in legacy code page bitmap.  */

		  if (!legacy_bitmap_enabled)
		    {
		      if (program)
			_dl_fatal_printf ("%s: legacy bitmap isn't available\n",
					  l->l_name);
		      else
			_dl_signal_error (EINVAL, l->l_name, "dlopen",
					  N_("legacy bitmap isn't available"));
		    }

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

	      enable_ibt &= (l->l_cet & lc_ibt) != 0;

	      /* SHSTK is enabled only if it is enabled in executable as
		 well as all shared objects.  */
	      enable_shstk &= (l->l_cet & lc_shstk) != 0;

	      /* Stop if both IBT and SHSTK are disabled in an object.  */
	      if (!enable_ibt && !enable_shstk)
		break;
	    }

	  if (legacy_bitmap_enabled)
	    __mprotect ((void *) GL(dl_x86_legacy_bitmap)[0],
			GL(dl_x86_legacy_bitmap)[1], PROT_READ);
	}

#ifdef SHARED
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

	  /* Disable IBT and/or SHSTK if they are enabled in ld.so, but
	     disabled in executable or shared objects.  */
	  unsigned int cet_feature = 0;

	  if (!enable_ibt)
	    cet_feature |= GNU_PROPERTY_X86_FEATURE_1_IBT;
	  if (!enable_shstk)
	    cet_feature |= GNU_PROPERTY_X86_FEATURE_1_SHSTK;

	  INTERNAL_SYSCALL_DECL (err);
	  int res = INTERNAL_SYSCALL (arch_prctl, err, 2,
				      ARCH_CET_DISABLE,
				      cet_feature);
	  if (res != 0)
	    _dl_fatal_printf ("%s: can't disable CET\n", program);

	  /* Clear the disabled bits in dl_x86_feature_1.  */
	  GL(dl_x86_feature_1) &= ~cet_feature;
	}

      if (program && (ibt_enabled || shstk_enabled))
	{
	  /* Lock CET if IBT or SHSTK is enabled in executable.  */
	  INTERNAL_SYSCALL_DECL (err);
	  int res = INTERNAL_SYSCALL (arch_prctl, err, 2,
				      ARCH_CET_LOCK, 0);
	  if (res != 0)
	    _dl_fatal_printf ("%s: can't lock CET\n", program);

	  unsigned int feature_1 = 0;
	  if (enable_ibt)
	    feature_1 |= GNU_PROPERTY_X86_FEATURE_1_IBT;
	  if (enable_shstk)
	    feature_1 |= GNU_PROPERTY_X86_FEATURE_1_SHSTK;
	  struct pthread *self = THREAD_SELF;
	  THREAD_SETMEM (self, header.feature_1, feature_1);
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
_dl_cet_init (struct link_map *main_map, int argc, char **argv, char **env)
{
  dl_cet_check (main_map, argv[0]);
  _dl_init (main_map, argc, argv, env);
}
#endif /* SHARED */

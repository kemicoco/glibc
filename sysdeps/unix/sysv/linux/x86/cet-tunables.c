/* Linux/x86 CET tuning.
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
# include <bits/prctl.h>

attribute_hidden
void
TUNABLE_CALLBACK (set_x86_ibt) (tunable_val_t *valp)
{
  if (DEFAULT_MEMCMP (valp->strval, "on", sizeof ("on")) == 0)
    {
      GL(dl_x86_feature_1)[1] &= ~((1 << ARCH_CET_EXEC_MAX) - 1);
      GL(dl_x86_feature_1)[1] |= ARCH_CET_EXEC_ALWAYS_ON;
    }
  else if (DEFAULT_MEMCMP (valp->strval, "off", sizeof ("off")) == 0)
    {
      GL(dl_x86_feature_1)[1] &= ~((1 << ARCH_CET_EXEC_MAX) - 1);
      GL(dl_x86_feature_1)[1] |= ARCH_CET_EXEC_ALWAYS_OFF;
    }
  else if (DEFAULT_MEMCMP (valp->strval, "permissive",
			   sizeof ("permissive")) == 0)
    {
      GL(dl_x86_feature_1)[1] &= ~((1 << ARCH_CET_EXEC_MAX) - 1);
      GL(dl_x86_feature_1)[1] |= ARCH_CET_EXEC_PERMISSIVE;
    }
}

attribute_hidden
void
TUNABLE_CALLBACK (set_x86_shstk) (tunable_val_t *valp)
{
  if (DEFAULT_MEMCMP (valp->strval, "on", sizeof ("on")) == 0)
    {
      GL(dl_x86_feature_1)[1] &= ~(((1 << ARCH_CET_EXEC_MAX) - 1)
				   << ARCH_CET_EXEC_MAX);
      GL(dl_x86_feature_1)[1] |= (ARCH_CET_EXEC_ALWAYS_ON
				  << ARCH_CET_EXEC_MAX);
    }
  else if (DEFAULT_MEMCMP (valp->strval, "off", sizeof ("off")) == 0)
    {
      GL(dl_x86_feature_1)[1] &= ~(((1 << ARCH_CET_EXEC_MAX) - 1)
				   << ARCH_CET_EXEC_MAX);
      GL(dl_x86_feature_1)[1] |= (ARCH_CET_EXEC_ALWAYS_OFF
				  << ARCH_CET_EXEC_MAX);
    }
  else if (DEFAULT_MEMCMP (valp->strval, "permissive",
			   sizeof ("permissive")) == 0)
    {
      GL(dl_x86_feature_1)[1] &= ~(((1 << ARCH_CET_EXEC_MAX) - 1)
				   << ARCH_CET_EXEC_MAX);
      GL(dl_x86_feature_1)[1] |= (ARCH_CET_EXEC_PERMISSIVE
				  << ARCH_CET_EXEC_MAX);
    }
}
#endif

/* Run-time dynamic linker data structures for x86 loaded ELF shared objects.
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

#ifndef	_LDSODEFS_H

/* Get the real definitions.  */
#include_next <ldsodefs.h>

#ifdef __CET__
extern void _dl_cet_open_check (struct link_map *);
# define DL_OPEN_CHECK(l) _dl_cet_open_check ((l))
#endif

#endif /* ldsodefs.h */

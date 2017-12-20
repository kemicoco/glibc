#!/bin/sh
# Check for error with -fcf-protection -mshstk on makecontext family
# functions.
# Copyright (C) 2017 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with the GNU C Library; if not, see
# <http://www.gnu.org/licenses/>.

set -e

CC=$1
CFLAGS=$2
shift 2

rc=0
for src in $*; do
  if $CC $CFLAGS $CPPFLAGS -fcf-protection -mshstk -S $src -o /dev/null 2>&1; then
    echo "FAIL: No error on $src with -fcf-protection -mshstk"
    rc=1
  else
    echo "PASS: Error on $src with -fcf-protection -mshstk"
  fi
done

exit $rc

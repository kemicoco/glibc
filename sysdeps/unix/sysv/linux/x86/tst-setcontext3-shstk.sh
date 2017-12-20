#!/bin/sh
# Check that GNU_PROPERTY_X86_FEATURE_1_SHSTK isn't set when setcontext()
# is used.
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

objpfx=$1
readelf=$2

test="${objpfx}tst-setcontext3"

if $readelf -n $test | grep SHSTK; then
  echo "FAIL: GNU_PROPERTY_X86_FEATURE_1_SHSTK is set on tst-setcontext3"
  exit 1
else
  echo "PASS: GNU_PROPERTY_X86_FEATURE_1_SHSTK isn't set on tst-setcontext3"
  exit 0
fi

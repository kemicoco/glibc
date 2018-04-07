# We don't need any header files.
abi-includes :=

abi-variants := 32 64 x32

abi-32-condition := !defined __x86_64__
abi-64-condition := defined __x86_64__ && defined __LP64__
abi-x32-condition := defined __x86_64__ && defined __ILP32__

ifeq ($(subdir),misc)
sysdep_headers += sys/elf.h sys/perm.h sys/reg.h sys/vm86.h sys/debugreg.h sys/io.h
endif

ifeq ($(subdir),nptl)
libpthread-sysdep_routines += elision-lock elision-unlock elision-timed \
			      elision-trylock
endif

ifeq ($(subdir),elf)
sysdep_routines += dl-vdso
endif

ifeq ($(subdir),setjmp)
tests += tst-saved_mask-1
endif

ifeq ($(enable-cet),yes)
ifeq ($(subdir),elf)
sysdep-dl-routines += dl-cet

tests += tst-legacy-1 tst-legacy-2 tst-legacy-2a tst-legacy-3 \
	 tst-legacy-4 tst-legacy-4a tst-legacy-4b tst-legacy-4c
modules-names += tst-legacy-mod-1 tst-legacy-mod-2 tst-legacy-mod-4

CFLAGS-tst-legacy-2.c += -fcf-protection=branch
CFLAGS-tst-legacy-2a.c += -fcf-protection
CFLAGS-tst-legacy-mod-1.c += -fcf-protection=none
CFLAGS-tst-legacy-mod-2.c += -fcf-protection=none
CFLAGS-tst-legacy-3.c += -fcf-protection=none
CFLAGS-tst-legacy-4.c += -fcf-protection=branch
CFLAGS-tst-legacy-4a.c += -fcf-protection
CFLAGS-tst-legacy-4b.c += -fcf-protection
CFLAGS-tst-legacy-mod-4.c += -fcf-protection=none

$(objpfx)tst-legacy-1: $(objpfx)tst-legacy-mod-1.so \
		       $(objpfx)tst-legacy-mod-2.so
$(objpfx)tst-legacy-2: $(objpfx)tst-legacy-mod-2.so $(libdl)
$(objpfx)tst-legacy-2.out: $(objpfx)tst-legacy-mod-1.so
$(objpfx)tst-legacy-2a: $(objpfx)tst-legacy-mod-2.so $(libdl)
$(objpfx)tst-legacy-2a.out: $(objpfx)tst-legacy-mod-1.so
$(objpfx)tst-legacy-4: $(libdl)
$(objpfx)tst-legacy-4.out: $(objpfx)tst-legacy-mod-4.so
$(objpfx)tst-legacy-4a: $(libdl)
$(objpfx)tst-legacy-4a.out: $(objpfx)tst-legacy-mod-4.so
tst-legacy-4a-ENV = GLIBC_TUNABLES=glibc.tune.x86_shstk=permissive
$(objpfx)tst-legacy-4b: $(libdl)
$(objpfx)tst-legacy-4b.out: $(objpfx)tst-legacy-mod-4.so
tst-legacy-4b-ENV = GLIBC_TUNABLES=glibc.tune.x86_shstk=on
$(objpfx)tst-legacy-4c: $(libdl)
$(objpfx)tst-legacy-4c.out: $(objpfx)tst-legacy-mod-4.so
tst-legacy-4c-ENV = GLIBC_TUNABLES=glibc.tune.x86_shstk=off
endif

# Add -fcf-protection to CFLAGS when CET is enabled.
CFLAGS-.o += -fcf-protection
CFLAGS-.os += -fcf-protection
CFLAGS-.op += -fcf-protection
CFLAGS-.oS += -fcf-protection

# Compile assembly codes with <cet.h> when CET is enabled.
asm-CPPFLAGS += -fcf-protection -include cet.h

ifeq ($(subdir),elf)
ifeq (yes,$(build-shared))
tests-special += $(objpfx)check-cet.out
endif

# FIXME: Can't use all-built-dso in elf/Makefile since this file is
# processed before elf/Makefile.  Duplicate it here.
cet-built-dso := $(common-objpfx)elf/ld.so $(common-objpfx)libc.so \
		 $(filter-out $(common-objpfx)linkobj/libc.so, \
			      $(sort $(wildcard $(addprefix $(common-objpfx), \
							    */lib*.so \
							    iconvdata/*.so))))

$(cet-built-dso:=.note): %.note: %
	@rm -f $@T
	LC_ALL=C $(READELF) -n $< > $@T
	test -s $@T
	mv -f $@T $@
common-generated += $(cet-built-dso:$(common-objpfx)%=%.note)

$(objpfx)check-cet.out: $(..)sysdeps/unix/sysv/linux/x86/check-cet.awk \
			$(cet-built-dso:=.note)
	LC_ALL=C $(AWK) -f $^ > $@; \
	$(evaluate-test)
generated += check-cet.out
endif
endif

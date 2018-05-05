ifeq ($(subdir),cpu-rt-c)
libcpu-rt-c-sysdep_routines += cacheinfo cpu-rt-misc cpu-rt-tunables

tunables-type = $(addprefix TUNABLES_FRONTEND_,$(have-tunables))
CPPFLAGS-cpu-rt-tunables.c += -DTUNABLES_FRONTEND=$(tunables-type)

# Make sure that the compiler does not insert any library calls in tunables
# code paths.
ifeq (yes,$(have-loop-to-function))
CFLAGS-cpu-rt-tunables.c += -fno-tree-loop-distribute-patterns
endif
endif

ifeq ($(subdir),csu)
gen-as-const-headers += cpu-features-offsets.sym
endif

ifeq ($(subdir),elf)
sysdep-dl-routines += dl-get-cpu-features

tests += tst-get-cpu-features
tests-static += tst-get-cpu-features-static
endif

ifeq ($(subdir),setjmp)
sysdep_routines += __longjmp_cancel
endif

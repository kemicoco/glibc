/* Create new context.
   Copyright (C) 2002-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@suse.de>, 2002.

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

#include <sysdep.h>
#include <stdarg.h>
#include <stdint.h>
#include <ucontext.h>
#if defined __CET__ && (__CET__ & 2) != 0
# include <stdio.h>
# include <pthread.h>
# include <sys/syscall.h>
# include <sys/prctl.h>
# include <x86intrin.h>
# include <libc-pointer-arith.h>
#endif

#include "ucontext_i.h"

/* This implementation can handle any ARGC value but only
   normal integer parameters.
   makecontext sets up a stack and the registers for the
   user context. The stack looks like this:
               +-----------------------+
               | next context          |
               +-----------------------+
               | parameter 7-n         |
	       +-----------------------+
	       | trampoline address    |
    %rsp ->    +-----------------------+

   The registers are set up like this:
     %rdi,%rsi,%rdx,%rcx,%r8,%r9: parameter 1 to 6
     %rbx   : address of next context
     %rsp   : stack pointer.
*/

/* XXX: This implementation currently only handles integer arguments.
   To handle long int and pointer arguments the va_arg arguments needs
   to be changed to long and also the stdlib/tst-setcontext.c file needs
   to be changed to pass long arguments to makecontext.  */


void
__makecontext (ucontext_t *ucp, void (*func) (void), int argc, ...)
{
  extern void __start_context (void) attribute_hidden;
  greg_t *sp;
  unsigned int idx_uc_link;
  va_list ap;
  int i;

  /* Generate room on stack for parameter if needed and uc_link.  */
  sp = (greg_t *) ((uintptr_t) ucp->uc_stack.ss_sp
		   + ucp->uc_stack.ss_size);
  sp -= (argc > 6 ? argc - 6 : 0) + 1;
  /* Align stack and make space for trampoline address.  */
  sp = (greg_t *) ((((uintptr_t) sp) & -16L) - 8);

  idx_uc_link = (argc > 6 ? argc - 6 : 0) + 1;

  /* Setup context ucp.  */
  /* Address to jump to.  */
  ucp->uc_mcontext.gregs[REG_RIP] = (uintptr_t) func;
  /* Setup rbx.*/
  ucp->uc_mcontext.gregs[REG_RBX] = (uintptr_t) &sp[idx_uc_link];
  ucp->uc_mcontext.gregs[REG_RSP] = (uintptr_t) sp;

  /* Setup stack.  */
  sp[0] = (uintptr_t) &__start_context;
  sp[idx_uc_link] = (uintptr_t) ucp->uc_link;

#if defined __CET__ && (__CET__ & 2) != 0
  struct pthread *self = THREAD_SELF;
  unsigned int feature_1 = THREAD_GETMEM (self, header.feature_1);
  /* NB: We must check feature_1 before accessing __ssp since caller
	 may be compiled against ucontext_t without __ssp.  */
  if ((feature_1 & (1 << 1)) != 0)
    {
      /* Shadow stack is enabled.  We need to allocate a new shadow
         stack.  Assuming each stack frame takes 8 byte return address
	 + 32 byte local stack.  */
# define SIZE_PER_STACK_FRAME 32
      unsigned long ssp_size = (((uintptr_t) sp
				 - (uintptr_t) ucp->uc_stack.ss_sp)
				/ ((8 + SIZE_PER_STACK_FRAME) / 8));
      /* Align shadow stack to 8 bytes.  */
      ssp_size = ALIGN_UP (ssp_size, 8);

      /* Allocate a new shadow stack with return address pointing to
	 __start_context.  */
      unsigned long long shstk = ssp_size;
      INTERNAL_SYSCALL_DECL (err);
      int res = INTERNAL_SYSCALL (arch_prctl, err, 2,
				  ARCH_CET_ALLOC_SHSTK, &shstk);
      if (res)
	__libc_fatal ("makecontext: failed to allocate shadow stack");

      /* Tell setcontext and swapcontext to restore shadow stack pointer
	 to the top of the new shadow stack.  NB: To free the new shadow
	 stack, caller of makecontext must call ARCH_CET_FREE_SHSTK with
	 ucp->__ssp[2] and ucp->__ssp[3].  */
      ucp->__ssp[0] = shstk + ssp_size - 8;
      ucp->__ssp[1] = (uintptr_t) &__start_context;
      ucp->__ssp[2] = shstk;
      ucp->__ssp[3] = ssp_size;
    }
#endif

  va_start (ap, argc);
  /* Handle arguments.

     The standard says the parameters must all be int values.  This is
     an historic accident and would be done differently today.  For
     x86-64 all integer values are passed as 64-bit values and
     therefore extending the API to copy 64-bit values instead of
     32-bit ints makes sense.  It does not break existing
     functionality and it does not violate the standard which says
     that passing non-int values means undefined behavior.  */
  for (i = 0; i < argc; ++i)
    switch (i)
      {
      case 0:
	ucp->uc_mcontext.gregs[REG_RDI] = va_arg (ap, greg_t);
	break;
      case 1:
	ucp->uc_mcontext.gregs[REG_RSI] = va_arg (ap, greg_t);
	break;
      case 2:
	ucp->uc_mcontext.gregs[REG_RDX] = va_arg (ap, greg_t);
	break;
      case 3:
	ucp->uc_mcontext.gregs[REG_RCX] = va_arg (ap, greg_t);
	break;
      case 4:
	ucp->uc_mcontext.gregs[REG_R8] = va_arg (ap, greg_t);
	break;
      case 5:
	ucp->uc_mcontext.gregs[REG_R9] = va_arg (ap, greg_t);
	break;
      default:
	/* Put value on stack.  */
	sp[i - 5] = va_arg (ap, greg_t);
	break;
      }
  va_end (ap);

}


weak_alias (__makecontext, makecontext)

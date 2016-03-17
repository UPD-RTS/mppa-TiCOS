/*
 * 		Copyright (c) 2007-2009 POK team
 *
 *		Redistribution and use in source and binary forms, with or without
 *		modification, are permitted provided that the following conditions
 *		are met:
 *
 *			* Redistributions of source code must retain the above copyright
 *			  notice, this list of conditions and the following disclaimer.
 *			* Redistributions in binary form must reproduce the above
 *			  copyright notice, this list of conditions and the following
 *			  disclaimer in the documentation and/or other materials
 *			  provided with the distribution.
 *			* Neither the name of the POK Team nor the names of its main
 *			  author (Julien Delange) or its contributors may be used to
 *			  endorse or promote products derived from this software
 *			  without specific prior written permission.
 *
 *		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *		AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *		LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *		FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *		COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *		INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *		BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *		ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *		POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file	 arch/mppa/arch.c
 * \brief	Provide generic architecture access for MPPA architecture
 */

#include <bsp.h>
#include <types.h>
#include <errno.h>
#include <core/partition.h>
#include <core/time.h>
#include <core/thread.h>
#include <mOS_vcore_u.h>

 #ifdef POK_NEEDS_DEBUG
	#include <libc.h>
 #endif

//#include <machine/exceptions.h>
//#include "rtc.h"

/** Patches for arch.c
 *  [1] pok_arch_init modified to enable FP bit in MSR
 *  [2] pok_arch_idle modified to enable interrupts for the IDLE thread
 *  [3] pok_arch_cache_freeze/unfreeze procedures
 */

unsigned* _loader_baseaddr;
/// _loader_off - the offset of the loading function (one per core)
unsigned* _loader_off;

extern __k1_uint8_t _bss_start;
extern __k1_uint8_t _bss_end;
extern __k1_uint8_t _sbss_start;
extern __k1_uint8_t _sbss_end;

extern mOS_scoreboard_t _scoreboard_start; /* score_board pointer */
extern int _scoreboard_kstack_start    __attribute__((weak));
extern int KSTACK_SIZE          __attribute__((weak));
extern void __vk1_asm_interrupt_handler(void);
extern void _system_call_ISR(void);

/* TODO Patmos leftovers
extern unsigned long long time_new;

extern void _interval_ISR(void);

extern void _system_call_ISR(void);

extern void pok_clear_bss(void) __attribute__((used));
*/

// Inits the architecture, no need to do anything in PATMOS
pok_ret_t pok_arch_init ()
{
	int pid = __k1_get_cpu_id();

	uintptr_t kstack = (uintptr_t)(&_scoreboard_kstack_start)
						- pid * (int)&KSTACK_SIZE;
	mOS_register_stack_handler((uint64_t*) kstack);
	mOS_register_it_handler(__vk1_asm_interrupt_handler);
	_scoreboard_start.SCB_VCORE.MAGIC_VALUE = 0x12344321;

	if (pid == 0) {
		/* Disable Dcache and Icache */
		mOS_dcache_disable();
		mOS_icache_disable(); //__k1_icache_disable

		/* Clear bss. This function is in legay.h, thus deprecated */
		__k1_bss_section(((__k1_uint8_t*) &_bss_start), ((__k1_uint32_t)&_bss_end) - ((__k1_uint32_t)     &_bss_start));
		__k1_bss_section(((__k1_uint8_t*) &_sbss_start), ((__k1_uint32_t)&_sbss_end) - ((__k1_uint32_t)   &_sbss_start));

		/* Register _system_call_ISR function as interrupt service routine
		 * for system call */
		mOS_register_scall_handler((mOS_exception_handler_t) &_system_call_ISR);
	}

	printf ("Exiting\n");
	exit(0);
	/* go to sleep for a while */
	do {
		mOS_idle1();
	} while (1);

	return POK_ERRNO_OK;
}

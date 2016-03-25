/*
 *  Copyright (C) 2013-2016 Kalray SA.
 *  All rights reserved.
 *
 *  This file is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
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
 *		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTEtions and the following
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
 *		FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NORRUPTION) HOWEVER
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
#include <HAL/hal/hal.h>

//TODO: remove, used for current test
#include "thread.h"

#ifdef POK_NEEDS_DEBUG
#include <libc.h>
#endif

/*
unsigned* _loader_baseaddr;
/// _loader_off - the offset of the loading function (one per core)
unsigned* _loader_off;
*/

extern __k1_uint8_t _bss_start;
extern __k1_uint8_t _bss_end;
extern __k1_uint8_t _sbss_start;
extern __k1_uint8_t _sbss_end;

extern mOS_scoreboard_t _scoreboard_start; /* score_board pointer */
extern int _scoreboard_kstack_start    __attribute__((weak));
extern int KSTACK_SIZE          __attribute__((weak));
extern void __vk1_asm_interrupt_handler(void);
extern void _system_call_ISR(void);

/* TODO: Patmos leftovers
extern unsigned long long time_new;

extern void _interval_ISR(void);

extern void _system_call_ISR(void);

extern void pok_clear_bss(void) __attribute__((used));
*/


// Set the processor to idle (lightsleep)
pok_ret_t pok_arch_idle()
{
	mOS_idle1();

	//normally what follows isn't executed until an interrupt arrives
	/*
	while (1)
	{
#ifdef POK_NEEDS_SCHED_O1
		// macro for pok_udpate_tick if schedO1
		// is enabled or for pok_tick_counter
		// both of timer.c
		uint64_t now = POK_GETTICK();

		if (POK_CURRENT_PARTITION.head_asynch_queue != POK_NULL && POK_CURRENT_PARTITION.head_asynch_queue->timestamp <= now)
		{
			pok_sched_end_period();
		}
#endif
	}
	*/
	return (POK_ERRNO_OK);
}


/** TODO: REMOVE, ALSO FEATURED IN core/thread.c
 * Start a thread, giving its entry call with \a entry
 * and its identifier with \a id
 * DO NO REMOVE - This function is called in cswitch in order to start the
 * idle thread
 * MINOR CHANGE: function original name: pok_thread_start
 * the name has been changed in order to avoid ambiguity with the next function
 */
void pok_thread_start_execution(void (*entry)(), unsigned int id)
{
	(void) id;
	entry();
}


// Function used to start a system thread such as the idle thread
// thread's id and entry point are supposed to be in r18 and r19
// (see arch/thread.c)
void pok_arch_thread_start() {

	uint32_t entry;
	uint32_t id;

	__asm__ __volatile__ (
		"copy %0, $r18\n\t;;"
		"copy %1, $r19\n\t;;"
		: "=r"(entry), "=r"(id)
	);

	pok_thread_start_execution((void (*)())entry, id);
}

uint32_t test;
uint32_t test2;

void pok_test() {
	printf("I'm inside the pok test function\n");
	pok_context_switch(test2);
}

void pok_test2() {
	printf("I'm inside the pok test2 function\n");
	pok_context_switch(test);
}

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

	test = pok_context_create(POK_CONFIG_NB_THREADS-1, 4096, (uint32_t)pok_test);
	test2 = pok_context_create(POK_CONFIG_NB_THREADS-2, 4096, (uint32_t)pok_test2);
	pok_context_print((context_t *)test);
	pok_context_print((context_t *)test2);

	pok_context_switch(test);

	printf ("Exiting\n");
	exit(0);
	return POK_ERRNO_OK;
}

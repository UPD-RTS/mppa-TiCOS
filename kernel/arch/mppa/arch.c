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

#ifdef POK_NEEDS_DEBUG
#include <libc.h>
#endif

/* bss and shadowbss sections holdmarkers */
extern __k1_uint8_t _bss_start;
extern __k1_uint8_t _bss_end;
extern __k1_uint8_t _sbss_start;
extern __k1_uint8_t _sbss_end;

extern mOS_scoreboard_t _scoreboard_start; /* score_board pointer */
extern int _scoreboard_kstack_start __attribute__((weak));
extern int KERNEL_STACK_SIZE __attribute__((weak));
extern void _interval_ISR(void);
extern void _system_call_ISR(void);

// Set the processor to idle (lightsleep)
pok_ret_t pok_arch_idle()
{
	while (1) {
		mOS_idle1();
	}
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

pok_ret_t pok_arch_cache_disable ()
{
	mOS_dcache_disable();
	mOS_icache_disable();
	return (POK_ERRNO_OK);
}

pok_ret_t pok_arch_cache_enable ()
{
	mOS_dcache_enable();
	mOS_icache_enable();
	return (POK_ERRNO_OK);
}

// Inits the architecture, no need to do anything in PATMOS
pok_ret_t pok_arch_init ()
{
	int pid = __k1_get_cpu_id();

	uintptr_t kstack = (uintptr_t)(&_scoreboard_kstack_start)
						- pid * (int)&KERNEL_STACK_SIZE;
	mOS_register_stack_handler((uint64_t *) kstack);
	mOS_register_it_handler(_interval_ISR);
	_scoreboard_start.SCB_VCORE.MAGIC_VALUE = 0x12344321;

	if (pid == 0) {
		/* Disable Dcache and Icache */
		pok_arch_cache_disable();

		/* Clear bss. This function is in legacy.h, thus deprecated */
		__k1_bss_section(((__k1_uint8_t*) &_bss_start), ((__k1_uint32_t)&_bss_end) - ((__k1_uint32_t)     &_bss_start));
		__k1_bss_section(((__k1_uint8_t*) &_sbss_start), ((__k1_uint32_t)&_sbss_end) - ((__k1_uint32_t)   &_sbss_start));

		/* Register _system_call_ISR function as interrupt service routine
		 * for system call */
		mOS_register_scall_handler((mOS_exception_handler_t) &_system_call_ISR);
	}

#ifdef POK_NEEDS_DEBUG
	printf ("[DEBUG] Pok arch init finished.\n");
#endif
	return POK_ERRNO_OK;
}

// Computes the stack address of a given thread in a given
// partition
uint32_t pok_thread_stack_addr (const uint8_t partition_id, const uint32_t local_thread_id)
{
	return pok_partitions[partition_id].size - (local_thread_id * POK_USER_STACK_SIZE);
}

pok_ret_t pok_arch_preempt_disable()
{
	// clear pending flags
	mOS_it_clear_num(MOS_VC_IT_TIMER_0);
	mOS_it_clear_num(MOS_VC_IT_TIMER_1);

	mOS_set_it_level(0);
	// enable interrupts
	mOS_it_disable();

	return (POK_ERRNO_OK);
}

pok_ret_t pok_arch_preempt_enable()
{
	// clear pending flags
	mOS_it_clear_num(MOS_VC_IT_TIMER_0);
	mOS_it_clear_num(MOS_VC_IT_TIMER_1);

	/* Activate stack switch for interrupts and exceptions */
	_scoreboard_start.SCB_VCORE.PER_CPU[0].SFR_PS.isw = 1;
	_scoreboard_start.SCB_VCORE.PER_CPU[0].SFR_PS.esw = 1;

	mOS_set_it_level(0);
	// enable interrupts
	mOS_it_enable();

	return (POK_ERRNO_OK);
}


pok_ret_t pok_arch_cache_invalidate ()
{
	/* Performs a full data and instruction cache invalidation */
	mOS_iinval();
	mOS_dinval();
	return (POK_ERRNO_OK);
}

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
 *              Copyright (c) 2007-2009 POK team
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

#include <types.h>
#include "prep/bsp.h"
#include "mOS_vcore_u.h"
#include "mOS_vcore_c.h"
#include "mOS_ftu_u.h"
#include "mOS_segment_manager_u.h"
#include "locked_assert.h" //TODO VBSP leftover
#include "dsu_os_comm.h" //TODO VBSP leftover

extern mOS_scoreboard_t _scoreboard_start; /* score_board pointer */
extern int _user_stack_start   __attribute__((weak));
extern int _user_stack_end     __attribute__((weak));
extern int _scoreboard_kstack_start    __attribute__((weak));
extern int KSTACK_SIZE          __attribute__((weak));

char __in_debug_might_trap[MOS_NB_PE_P] __attribute__ ((section(".locked_data")));
char __in_debug_trapped[MOS_NB_PE_P] __attribute__ ((section(".locked_data")));

void __vk1_asm_interrupt_handler(void);
void __vk1_asm_trap_handler24(void);
extern void __vk1_asm_debug_handler(void);
extern void __k1_replace_sp_and_call(uint32_t value, void (*)(uint32_t));

extern void _do_slave_pe(uint32_t old_sp);
extern void _do_master_pe(uint32_t old_sp);

extern __k1_uint8_t _bss_start;
extern __k1_uint8_t _bss_end;
extern __k1_uint8_t _sbss_start;
extern __k1_uint8_t _sbss_end;

void
__user_do_scall_w(int r0, int r1, int r2, int r3, int r4, int r5, int r6, int r7);

__attribute__((unused)) static int xputs(const char *str)  {
	const char *ps = str;
	while (*ps)
		ps++;

	#if 1
	__asm__ __volatile__ ("make $r20 = 1\n\t;;");
	__k1_club_syscall2 (/*1104*/ 4094, (volatile int) str, ps - str);
	#endif

	return 0;
}

/* Debug auxiliary functions */
__attribute__((unused)) static struct dsu_os_s *get_os_mem () {
	register int ret __asm__("$r0");
	__asm__ __volatile__ ("scall %0 \n\t;;" : : "ir" (1099));
	return (struct dsu_os_s *) ret;
}

#define DEBUG_HANDLER_STACK_SIZE 512
unsigned char __debug_handler_stack[MOS_NB_PE_P * DEBUG_HANDLER_STACK_SIZE] __attribute__ ((section(".locked_data"), aligned (8)));

#define DEBUG_HANDLER_MEM_AREA_SIZE 256
struct dsu_os_s __debug_handler_mem_area __attribute__ ((section(".locked_data"), aligned (8)));

/* Interruption handlers */
static mppa_ticos_pe_callback_t pe_callbacks [BSP_NB_PE_MAX]
					__attribute__((section (TARGET_DATA)));
mppa_ticos_timer_callback_t timer_callbacks [BSP_NB_PE_MAX][BSP_NB_TIMERS]
					__attribute__((section (TARGET_DATA)));

void mppa_ticos_it_pe_handler(int line, __k1_vcontext_t *ctx)
					__attribute__((section(TARGET_TEXT)));
void mppa_ticos_it_handler(int ev_src, __k1_vcontext_t *ctx)
					__attribute__((section(TARGET_TEXT)));

void __attribute__((section(TARGET_TEXT)))
mppa_ticos_it_pe_handler(int line, __k1_vcontext_t *ctx __attribute__((unused))) {
	int pid                 = __k1_get_cpu_id();
	mOS_vcore_set_t srcs    = bsp_inter_pe_interrupt_clear(line - BSP_IT_PE_0);
	mppa_ticos_pe_callback_t cb  = (mppa_ticos_pe_callback_t)
					__builtin_k1_lwu(&pe_callbacks[pid]);
	if (cb)
		cb (srcs);
}

void __attribute__((section(TARGET_TEXT)))
mppa_ticos_it_handler(int ev_src, __k1_vcontext_t *ctx __attribute__((unused))) {
	int pid = __k1_get_cpu_id();
	switch (ev_src) {
		case BSP_IT_TIMER_0:
		case BSP_IT_TIMER_1:
		{
			/* enums ev_src value are 0/1 */
			mppa_ticos_timer_callback_t cb = (mppa_ticos_timer_callback_t)
				__builtin_k1_lwu(&timer_callbacks[pid][ev_src]);
			if(cb) {
				cb (ev_src);
			} else {
				locked_puts ("No handler set for a called interrupt.");
			}
		}
		break;
		case BSP_IT_WDOG:
		case BSP_IT_WDOG_U:
			assert(0 && "Not yet supported \n");
			break;
		default:
			assert(0 && "Error, not expected interrupt id \n");
			break;
		}
}

/* The C entry point function (called from the S entry point in entry.S file.)
 * ALL the processors are booted, it is up to the user to split code / make
 * processors sleep.
 */
void _c_entry_point() __attribute__((section (".init_code")));
void _c_entry_point() {
	int pid = __k1_get_cpu_id();

	/* Register a pointer to the stack used for traps/int. The stack
	 * switching is only performed by the first nesting level, nested
	 * int./traps. use the same stack. Upon a trap/int. a small context
	 * (16 + 4 registers) will be saved on this stack.
	 */
	uintptr_t kstack = (uintptr_t)(&_scoreboard_kstack_start)
						- pid * (int)&KSTACK_SIZE;
	mOS_register_stack_handler((uint64_t*) kstack);
	mOS_register_trap_handler(__vk1_asm_trap_handler24);

	/* Register the ebug handler. If a full context save must be
	 * performed, the entry point should be an ASM routine that will save
	 * the remaining registers on the stack.
	 */
	mOS_register_it_handler(__vk1_asm_interrupt_handler);

	_scoreboard_start.SCB_VCORE.MAGIC_VALUE = 0x12344321;

	/* Register the debug handler, its stack and mem area. The entry point
	 * is also a ASM routine.
	 */
	mOS_register_debug_handler(__vk1_asm_debug_handler);
	mOS_register_debug_handler_stack((uint64_t *) (__debug_handler_stack +
		(pid * DEBUG_HANDLER_STACK_SIZE) + DEBUG_HANDLER_STACK_SIZE));
	mOS_register_debug_mem_area((uint64_t *) &__debug_handler_mem_area);

	if (pid == 0) {
		/* Disable most protection on devices, assumption is that the
		 * user owns the whole MPPA. Therefore, it is not required to
		 * check routes, tags etc...
		 */
		bsp_event_init();
		bsp_handlers_init();

		mOS_configure_int (MOS_VC_IT_TIMER_0, 1 /* level */);
		mOS_configure_int (MOS_VC_IT_TIMER_1, 1 /* level */);
		mOS_configure_int (MOS_VC_IT_WATCHDOG, 1 /* level */);

		bsp_register_it(mppa_ticos_it_handler, BSP_IT_TIMER_0);
		bsp_register_it(mppa_ticos_it_handler, BSP_IT_TIMER_1);
		bsp_register_it(mppa_ticos_it_pe_handler, BSP_IT_PE_0 +
							BSP_EVENT_LIBOS + 0);
		bsp_register_it(mppa_ticos_it_handler, BSP_IT_WDOG_U);
		bsp_register_it(mppa_ticos_it_handler, BSP_IT_WDOG);

		mOS_set_it_level(0);
		mOS_it_enable();

		// flush stack
		__builtin_k1_wpurge();
		__builtin_k1_fence();

		__k1_bss_section(((__k1_uint8_t*) &_bss_start), ((__k1_uint32_t)&_bss_end) - ((__k1_uint32_t)     &_bss_start));
     		__k1_bss_section(((__k1_uint8_t*) &_sbss_start), ((__k1_uint32_t)&_sbss_end) - ((__k1_uint32_t)   &_sbss_start));

		/* Register a syscall handle (see prep/bsp_handlers.c) */
		mOS_register_scall_handler((mOS_exception_handler_t) &__user_do_scall_w);
	}

	/* go to sleep for a while */
	do {
		mOS_idle1();
	} while (1);
}

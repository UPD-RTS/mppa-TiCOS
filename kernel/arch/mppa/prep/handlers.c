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
#include "bsp.h"
#include "mOS_vcore_u.h"
#include "mOS_dnoc_u.h"
#include "mOS_segment_manager_u.h"
#include <HAL/hal/hal.h>

/*********************************************************************************************
 * Interrupts handlers should be used as they would be on a real hardware. They should save  *
 * the remaining registers of the context, or ensure that they will not modify any of these  *
 * registers. Do not forget to disable hw-loops in your code if you don't save the           *
 * appropriate registers.                                                                    *
 *                                                                                           *
 * The hypervisor performs a seamless (almost) control transfer between the user code and    *
 * its handler. This handler is executed in user mode and thus is constrained by the user    *
 * context ( MPPA_DSM_CLIENT pages, device access right etc...).                             *
 *                                                                                           *
 * As on real hardware, an "RFE" instruction must be issued to resume the execution. This    *
 * instruction is emulated by the mOS_RFE hypercall.                                         *
 *                                                                                           *
 * mOS provides some usefull arguments to the handler, it_number, $spc, $spc register values,*
 * and a pointer to the partially saved context (r0-r16, and r32-r35.)                       *
 *                                                                                           *
 *********************************************************************************************/

/* MOS_NB_PE_P number of processor elements in the core
 * BSP_NB_IT_SOURCES number of possible event sources */
it_handler_t second_level_handler[MOS_NB_PE_P][BSP_NB_IT_SOURCES]
				__attribute__((section(".locked_data")));


void bsp_register_it(it_handler_t it_handler, bsp_ev_src_e source)
{
	second_level_handler[__inl_k1_get_cpu_id()][source] = it_handler;
}

it_handler_t bsp_get_registerered_it(bsp_ev_src_e source)
{
	return second_level_handler[__inl_k1_get_cpu_id()][source];
}


void
bsp_handlers_init(void)
{
	int i, j;

	for (j = 0; j < MOS_NB_PE_P; j++) {
		for (i = 0; i < BSP_NB_IT_SOURCES; i++) {
			second_level_handler[j][i] = 0;
		}
	}

	__builtin_k1_wpurge();
	__builtin_k1_fence();

}


#ifdef __k1dp__
static mOS_vcore_ev_src_t remaining_sources [MOS_NB_PE_P] __attribute__((section(".locked_data")));
#endif

void
__k1_do_int(int it_nb  __attribute__((unused)), __k1_vcontext_t *ctx)
{

	/*********************************************************
	 *                                                       *
	 * To avoid loosing events, all the it sources must be   *
	 * processed. To do this, you may scan all the inputs    *
	 * in the scoreboard, or (better) check only for         *
	 * expected sources.                                     *
	 *                                                       *
	 *********************************************************/
#ifdef __k1dp__
	int current_src;
#endif
	int pid = __k1_get_cpu_id();
	mOS_enable_hw_loop();
	switch (it_nb) {
	case MOS_VC_IT_TIMER_0:
		if (second_level_handler[pid][BSP_IT_TIMER_0]) {
			second_level_handler[pid][BSP_IT_TIMER_0](BSP_IT_TIMER_0, ctx);
		}
		break;
	case MOS_VC_IT_TIMER_1:
		if (second_level_handler[pid][BSP_IT_TIMER_1]) {
			second_level_handler[pid][BSP_IT_TIMER_1](BSP_IT_TIMER_1, ctx);
		}
		break;
#if (defined __k1b__ || defined __k1io__)
	case MOS_VC_IT_MAILBOX:
		if (second_level_handler[pid][BSP_IT_CN]) {
			second_level_handler[pid][BSP_IT_CN](BSP_IT_CN, ctx);
		}
		break;
	case MOS_VC_IT_DNOC_RX:
		if (second_level_handler[pid][BSP_IT_RX]) {
			second_level_handler[pid][BSP_IT_RX](BSP_IT_RX, ctx);
		}
		break;
	case MOS_VC_IT_UCORE:
		if (second_level_handler[pid][BSP_IT_UC]) {
			second_level_handler[pid][BSP_IT_UC](BSP_IT_UC, ctx);
		}
		break;
	case MOS_VC_IT_NOCERR:
		if (second_level_handler[pid][BSP_IT_NE]) {
			second_level_handler[pid][BSP_IT_NE](BSP_IT_NE, ctx);
		}
		break;
#endif
	case MOS_VC_IT_USER_0:
	{
#ifdef __k1dp__
		__builtin_k1_wpurge();
		__builtin_k1_fence();
		// check if source is a pe on line 0
		// bug, interruption peut n'être invoquée qu'une fois!
		remaining_sources [pid]._dword |=
			__builtin_k1_ldc(&(_scoreboard_start.SCB_VCORE.PER_CPU [pid].EV_GLOBAL_SRCS [BSP_IT_LINE]._dword));

		while (remaining_sources [pid]._dword) {
			current_src                                 = __builtin_k1_ctzdl(remaining_sources [pid]._dword) / 8;
			remaining_sources [pid].array[current_src]  = 0;
			if (remaining_sources [pid]._dword) {
				// magic happen's here ! we raise an SWIT to guarantee that if the second_level_handler nerver
				// returns ( issues an RFE), then the remaining interrupts will not be loosed. They will
				// be catched again when the RFE resumes..
				mOS_it_raise_num(MOS_VC_IT_USER_0);
			}
			switch (current_src) {
			case MOS_VC_EV_RX:
#ifdef  __k1b__
				/* TODO not yet implemented
				assert(0 && "Rx events are not virtualized anymore"); */
#endif
				if (second_level_handler[pid][BSP_IT_RX]) {
					second_level_handler[pid][BSP_IT_RX](BSP_IT_RX /* pass source interrupt */, ctx);
				}
				break;
			case MOS_VC_EV_UC:
				if (second_level_handler[pid][BSP_IT_UC]) {
					second_level_handler[pid][BSP_IT_UC](BSP_IT_UC /* pass source interrupt */,  ctx);
				}
				break;
			case MOS_VC_EV_PE:
			{
				/* TODO not yet implemented
				pe_it_dispatcher(ctx); */
			}
			break;
			case MOS_VC_EV_MB:
#ifdef  __k1b__
				/* TODO not yet implemented
				assert(0 && "Mb events are not virtualized anymore"); */
#endif
				if (second_level_handler[pid][BSP_IT_CN]) {
					second_level_handler[pid][BSP_IT_CN](BSP_IT_CN /* pass source interrupt */, ctx);
				}
				break;
			case MOS_VC_EV_NE:
				if (second_level_handler[pid][BSP_IT_NE]) {
					second_level_handler[pid][BSP_IT_NE](BSP_IT_NE /* pass source interrupt */, ctx);
				}
				break;
			}
		}
#else
		pe_it_dispatcher(ctx);
#endif
	}
	break;

	case MOS_VC_IT_WATCHDOG:
		if (second_level_handler[pid][BSP_IT_WDOG]) {
			second_level_handler[pid][BSP_IT_WDOG](BSP_IT_WDOG, ctx);
		}
		break;
	case MOS_VC_IT_WATCHDOG_OVERFLOW:
		if (second_level_handler[pid][BSP_IT_WDOG_U]) {
			second_level_handler[pid][BSP_IT_WDOG_U](BSP_IT_WDOG_U, ctx);
		}
		/* TODO not yet implemented
		assert(0 && "Interrupt line not supported yet \n"); */
		break;
#ifdef __k1io__
	case MOS_VC_IT_GIC_1:
	case MOS_VC_IT_GIC_2:
	case MOS_VC_IT_GIC_3:
		if (second_level_handler[pid][BSP_IT_GIC_1 + (it_nb - MOS_VC_IT_GIC_1)]) {
			second_level_handler[pid][BSP_IT_GIC_1 + (it_nb - MOS_VC_IT_GIC_1)](BSP_IT_GIC_1 + (it_nb - MOS_VC_IT_GIC_1), ctx);
		}
		break;
#endif
	}
}

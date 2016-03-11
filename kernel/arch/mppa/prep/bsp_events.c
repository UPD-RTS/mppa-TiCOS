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

#include "bsp.h"
#include "mOS_vcore_u.h"

mOS_pad_vcore_set_t sub_pe_ev_source[MOS_NB_PE_P][MOS_VC_NB_EV_LINES][BSP_NB_PE_EV_SRC] __attribute__((section(".locked_data"), aligned(_K1_DCACHE_LINE_SIZE)));
uint64_t global_pe_lines __attribute__((
		section(".locked_data"))) = 0;
extern it_handler_t second_level_handler[MOS_NB_PE_P][BSP_NB_IT_SOURCES /* number of possible event sources */]
__attribute__((
		section(".locked_data")));

#if BSP_NB_PE_EV_SRC > 16
#  error "BSP_NB_PE_EV_SRC > 16 needs minor code modifications in bsp_events.c"
#endif


enum {
	IT_MASKED   = 0x1,
	IT_PENDING  = 0x2,
};

/* 2 bits for each source -> 32 bits per PE */
uint32_t masked_it[MOS_NB_PE_P] __attribute__((section(".locked_data")));

void
bsp_event_init()
{
	int i, j, k;
	for (i = 0; i < MOS_NB_PE_P; i++) {
		for (j = 0; j < BSP_NB_PE_EV_SRC; j++) {
			for (k = 0; k < MOS_VC_NB_EV_LINES; k++) {
				sub_pe_ev_source[i][k][j]._word = 0;
			}
			masked_it[i] = 0;
		}
	}
	__builtin_k1_wpurge();
	__builtin_k1_fence();
}

int
bsp_inter_pe_event_is_present(mOS_vcore_set_t mask, int pe_line)
{
	int pid = __inl_k1_get_cpu_id();
	mOS_pad_vcore_set_t poped_list;
	poped_list._word = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]);
	return poped_list.pe_list & mask;
}

mOS_vcore_set_t
bsp_inter_pe_event_waitany_clear(int pe_line)
{

	unsigned swap_failed;
	__k1_swap_value_t mb_value;

	mOS_pad_vcore_set_t poped_list;

	int pid = __inl_k1_get_cpu_id();
	while (1) {
		poped_list._word = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]);
		if (poped_list.pe_list) {           // at least someone
			do {                            // reset them
				poped_list._word    = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]);
				mb_value.old_value  = poped_list._word;
				mb_value.new_value  = 0;    // pop waiting list
				swap_failed         = __k1_try_swap_values((uint32_t*) (&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]),
					mb_value);
			} while (swap_failed);
			__builtin_k1_fence();
			break;
		}
		// else wait for a notification
		mOS_pe_event_waitclear(BSP_EV_LINE);    // wait for an event
	}
	return poped_list.pe_list;
}

void
bsp_inter_pe_event_waitclear(mOS_vcore_set_t mask, int pe_line)
{

	unsigned swap_failed;
	__k1_swap_value_t mb_value;

	mOS_pad_vcore_set_t wait_mask;
	mOS_pad_vcore_set_t poped_list;
	wait_mask._word     = 0;
	wait_mask.pe_list   = mask;

	int pid = __inl_k1_get_cpu_id();
	while (1) {
		poped_list._word = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]);
		if ((poped_list.pe_list & mask) == mask) {                              // all the waited id's are there
			do {                                                                // reset them
				poped_list._word    = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]);
				mb_value.old_value  = poped_list._word;
				mb_value.new_value  = poped_list._word & ~(wait_mask.pe_list);  // pop waiting list
				swap_failed         = __k1_try_swap_values((uint32_t*) (&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]),
					mb_value);
			} while (swap_failed);
			__builtin_k1_fence();
			break;
		}
		// else wait for a notification
		mOS_pe_event_waitclear(BSP_EV_LINE);    // wait for an event
	}
}

void
bsp_inter_pe_event_clear(mOS_vcore_set_t mask, int pe_line)
{

	unsigned swap_failed;
	__k1_swap_value_t mb_value;

	mOS_pad_vcore_set_t wait_mask;
	mOS_pad_vcore_set_t poped_list;
	wait_mask._word     = 0;
	wait_mask.pe_list   = mask;

	int pid = __inl_k1_get_cpu_id();

	poped_list._word = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]);
	do {                                                                // reset them
		poped_list._word    = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]);
		mb_value.old_value  = poped_list._word;
		mb_value.new_value  = poped_list._word & ~(wait_mask.pe_list);  // pop waiting list
		swap_failed         =
			__k1_try_swap_values((uint32_t*) (&sub_pe_ev_source[pid][BSP_EV_LINE][pe_line]), mb_value);
	} while (swap_failed);
	__builtin_k1_fence();
	// else wait for a notification
	mOS_pe_event_clear(BSP_EV_LINE);    // wait for an event
}

void
bsp_inter_pe_event_notify(mOS_vcore_set_t mask, int pe_line)
{
	unsigned swap_failed;
	__k1_swap_value_t mb_value;
	int pid = __inl_k1_get_cpu_id();

	mOS_pad_vcore_set_t pad_mask;
	pad_mask._word      = 0;
	pad_mask.pe_list    = mask;
	mOS_pad_vcore_set_t poped_list;
	while (pad_mask._word) {  // for each src id
		int current = __builtin_k1_ctz(pad_mask._word);
		pad_mask._word = pad_mask._word & ~(0x1 << current);

		do {                                        // add me to the list
			poped_list._word    = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[current][BSP_EV_LINE][pe_line]);
			mb_value.old_value  = poped_list._word;
			poped_list.pe_list  |= (0x1 << pid);    // add myself in the list
			mb_value.new_value  = poped_list._word; // pop waiting list
			swap_failed         = __k1_try_swap_values((uint32_t*) (&sub_pe_ev_source[current][BSP_EV_LINE][pe_line]),
				mb_value);
		} while (swap_failed);
	}

	__builtin_k1_fence();
	mOS_pe_notify(mask, BSP_EV_LINE, 1, (pe_line == BSP_IT_LINE));

}

void
bsp_inter_pe_interrupt_raise(mOS_vcore_set_t mask, int line)
{
	int pe_line = line - BSP_IT_PE_0;
	locked_assert((pe_line >= 0) && (pe_line < 16) && "Raising IT other than PE 2 PE not supported");

	unsigned swap_failed;
	__k1_swap_value_t mb_value;
	int pid         = __inl_k1_get_cpu_id();
	int deactivated = 0;
	mOS_vc_vps_t ps;
	ps = _scoreboard_start.SCB_VCORE.PER_CPU [pid].SFR_PS;
	if (ps.ie) {
		deactivated = 1;
		mOS_it_disable();
	}

	mOS_pad_vcore_set_t pad_mask;
	pad_mask._word      = 0;
	pad_mask.pe_list    = mask;
	mOS_pad_vcore_set_t poped_list;
	while (pad_mask._word) {  // for each src id
		int current = __builtin_k1_ctz(pad_mask._word);
		pad_mask._word = pad_mask._word & ~(0x1 << current);

		do {                                        // add me to the list
			poped_list._word    = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[current][BSP_IT_LINE][pe_line]);
			mb_value.old_value  = poped_list._word;
			poped_list.pe_list  |= (0x1 << pid);    // add myself in the list
			mb_value.new_value  = poped_list._word; // pop waiting list
			swap_failed         = __k1_try_swap_values((uint32_t*) (&sub_pe_ev_source[current][BSP_IT_LINE][pe_line]),
				mb_value);
		} while (swap_failed);
	}
	__builtin_k1_fence();
	mOS_pe_notify(mask & ~(0x1 << pid), BSP_IT_LINE, (line == BSP_EV_LINE), 1);

	if (mask & (0x1 << pid)) {
#ifdef __k1dp__
#ifndef __k1a__
		__builtin_k1_sbu(&(_scoreboard_start.SCB_VCORE.PER_CPU [pid].EV_GLOBAL_SRCS [BSP_IT_LINE].sources.pe), 1);
#else
		_scoreboard_start.SCB_VCORE.PER_CPU [pid].EV_GLOBAL_SRCS [BSP_IT_LINE].sources.pe = 1;
#endif
		__builtin_k1_wpurge();
		__builtin_k1_fence();
#endif
		mOS_it_raise_num(MOS_VC_IT_USER_0);
	}

	if (deactivated) {
		mOS_it_enable();
	}
}

mOS_vcore_set_t
bsp_inter_pe_interrupt_clear(int pe_line)
{

	int pid = __inl_k1_get_cpu_id();
	unsigned swap_failed;
	__k1_swap_value_t mb_value;

	mOS_pad_vcore_set_t pop;

	do {
		pop._word           = __builtin_k1_lwu((uint32_t*)&sub_pe_ev_source[pid][BSP_IT_LINE][pe_line]);
		mb_value.old_value  = pop._word;
		mb_value.new_value  = 0;    // pop waiting list
		swap_failed         =
			__k1_try_swap_values((uint32_t*) (&sub_pe_ev_source[pid][BSP_IT_LINE][pe_line]), mb_value);
	} while (swap_failed);

#ifdef __k1io__
	mOS_gic_clear_rm2rm_it(_MOS_VC_IT_GIC_0);
#endif

	return pop.pe_list;
}

void
bsp_interrupt_enable(bsp_ev_src_e src)
{

	int source = src - BSP_IT_PE_0;
	locked_assert((source >= 0) && (source < 16) && "Enabling IT other than PE 2 PE not supported");

	int deactivated = 0;
	int pid         = __inl_k1_get_cpu_id();

	mOS_vc_vps_t ps;
	ps = _scoreboard_start.SCB_VCORE.PER_CPU [pid].SFR_PS;
	if (ps.ie) {
		deactivated = 1;
		mOS_it_disable();
	}

	int pending = masked_it[pid] & (IT_PENDING << (source * 2));
	masked_it[pid] &= ~((IT_PENDING | IT_MASKED) << (source * 2));

	if (pending) {
#ifdef __k1dp__
#ifndef __k1a__
		__builtin_k1_sbu(&(_scoreboard_start.SCB_VCORE.PER_CPU [pid].EV_GLOBAL_SRCS [BSP_IT_LINE].sources.pe), 1);
#else
		_scoreboard_start.SCB_VCORE.PER_CPU [pid].EV_GLOBAL_SRCS [BSP_IT_LINE].sources.pe = 1;
#endif
		__builtin_k1_wpurge();
		__builtin_k1_fence();
#endif
		mOS_it_raise_num(MOS_VC_IT_USER_0);
	}

	if (deactivated) {
		mOS_it_enable();
	}
}

void
bsp_interrupt_disable(bsp_ev_src_e src)
{
	int source = src - BSP_IT_PE_0;
	locked_assert((source >= 0) && (source < 16) && "Masking IT other than PE 2 PE not supported");

	masked_it[__inl_k1_get_cpu_id()] |= IT_MASKED << (source * 2);
}



void
pe_it_dispatcher(
	__k1_vcontext_t *ctx)
{


	int pid     = __inl_k1_get_cpu_id();
	int line    = 0;

	__k1_dcache_invalidate_mem_area((__k1_uintptr_t) &sub_pe_ev_source[pid][BSP_IT_LINE][line], sizeof(sub_pe_ev_source[pid][BSP_IT_LINE]));

	for (line = 0; line < BSP_NB_PE_EV_SRC; line++) {
		uint32_t src = sub_pe_ev_source[pid][BSP_IT_LINE][line]._word;
		if (src) { /* present and not masked */
			if (!(masked_it[pid] & (IT_MASKED << (line * 2)))) {
				if (second_level_handler[pid][BSP_IT_PE_0 + line]) {
					__builtin_k1_wpurge();
					__builtin_k1_fence();
					second_level_handler[pid][BSP_IT_PE_0 + line](BSP_IT_PE_0 + line, ctx);
				}
			} else {
				masked_it[pid] |= IT_PENDING << (line * 2);
			}
		}
	}
}

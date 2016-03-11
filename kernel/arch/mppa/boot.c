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
#include "dsu_os_comm.h" //TODO VBSP leftover

extern mOS_scoreboard_t _scoreboard_start; /* score_board pointer */
extern int _user_stack_start   __attribute__((weak));
extern int _user_stack_end     __attribute__((weak));
extern int _scoreboard_kstack_start    __attribute__((weak));
extern int KSTACK_SIZE          __attribute__((weak));
extern __k1_uint32_t INTERNAL_RAM_SIZE;

char __in_debug_might_trap[MOS_NB_PE_P] __attribute__ ((section(".locked_data")));
char __in_debug_trapped[MOS_NB_PE_P] __attribute__ ((section(".locked_data")));

void __vk1_asm_interrupt_handler(void);
void __vk1_asm_trap_handler24(void);
extern void __vk1_asm_debug_handler(void);
extern void __k1_replace_sp_and_call(uint32_t value, void (*)(uint32_t));

/*
extern void _do_slave_pe(uint32_t old_sp);
extern void _do_master_pe(uint32_t old_sp);
*/

extern __k1_uint8_t _bss_start;
extern __k1_uint8_t _bss_end;
extern __k1_uint8_t _sbss_start;
extern __k1_uint8_t _sbss_end;

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

/* Syscall auxiliary functions */


void
__user_do_scall_w(int r0, int r1, int r2, int r3, int r4, int r5, int r6, int r7);

int __attribute__((section(TARGET_TEXT)))
__user_do_scall(int r0, int r1, int r2, int r3 __attribute__((unused)), int r4 __attribute__(
	(unused)), int r5 __attribute__((unused)), int r6 __attribute__((unused)), int r7)
{
	int ret = -1;
	mOS_enable_hw_loop();
	return ret;
}


/* Debug auxiliary functions */
static struct dsu_os_s *get_os_mem () {
	register int ret __asm__("$r0");
	__asm__ __volatile__ ("scall %0 \n\t;;" : : "ir" (1099));
	return (struct dsu_os_s *) ret;
}

#define DEBUG_HANDLER_STACK_SIZE 512
unsigned char __debug_handler_stack[MOS_NB_PE_P * DEBUG_HANDLER_STACK_SIZE] __attribute__ ((section(".locked_data"), aligned (8)));

#define DEBUG_HANDLER_MEM_AREA_SIZE 256
struct dsu_os_s __debug_handler_mem_area __attribute__ ((section(".locked_data"), aligned (8)));

__k1_tlb_entry_t    init_ltlb[MOS_VC_NB_USER_LTLB] __attribute__ ((section(".locked_data")));   /* Initial ltlb entry values for all the pe's */
__k1_tlb_entry_t    init_jtlb[MOS_VC_NB_USER_JTLB] __attribute__ ((section(".locked_data")));

static int __try_patch_mmu_entry(unsigned char *addr __attribute__ ((unused)), int *old_pa_p  __attribute__ ((unused)), int *old_idx_p __attribute__ ((unused)), __k1_tlb_entry_t *tlb_p __attribute__ ((unused))){
#ifndef __k1a__
	/* The MMU might not allow us to write to this location, let's cheat */
	__k1_tlb_entry_t tlb;
	signed int old_pa = -1;
	signed int old_idx = -1;
	tlb._dword = 0ULL;

	unsigned int idx = mOS_probetlb((unsigned int) addr);
	if(!(idx & _K1_MASK_MMC_E)) {
		tlb = mOS_readtlb(idx);
		old_pa = tlb._fields.pa;
		old_idx = idx;
		if(tlb._fields.pa != _K1_MMU_PA_RW_RW && tlb._fields.pa != _K1_MMU_PA_RWX_RW && tlb._fields.pa != _K1_MMU_PA_RWX_RWX) {
			tlb._fields.pa = _K1_MMU_PA_RWX_RWX;
			mOS_mem_write_jtlb (tlb, idx & 0x1);
			*old_pa_p = old_pa;
			*old_idx_p = old_idx;
			*tlb_p = tlb;
			return 1;
		}
	}
#endif
	return 0;
}


static void __unpatch_mmu_entry(int old_pa __attribute__ ((unused)), int old_idx  __attribute__ ((unused)), __k1_tlb_entry_t tlb __attribute__ ((unused))){
#ifndef __k1a__
	if(old_pa != -1 && (old_pa ==  _K1_MMU_PA_RW_RW || old_pa == _K1_MMU_PA_RWX_RW || old_pa ==  _K1_MMU_PA_RWX_RWX)) {
		tlb._fields.pa = old_pa;
		__k1_tlb_entry_t new_tlb = mOS_readtlb(old_idx);   /* Might have changed since */
		if(new_tlb._fields.pn == tlb._fields.pn) {
			new_tlb._fields.pa = old_pa;
			mOS_mem_write_jtlb (new_tlb, old_idx & 0x1);
		}
	}
#endif
}


int __debug_handler_read_write_mem (unsigned char *dst,
				unsigned char *src, unsigned char *src_end)
{
	int ret = 1;
	int pid = __k1_get_cpu_id ();
	__in_debug_might_trap[pid] = 1;

	if ((((unsigned int) src) & 7) == 0 && (((unsigned int) dst) & 7) == 0)
	{
		while (src_end - src >= 8)
		{
			__builtin_k1_barrier ();
			uint64_t tmp = *((uint64_t *) src);
			__builtin_k1_barrier ();
			*((uint64_t *) dst) = tmp;
			__builtin_k1_barrier ();

			if(__in_debug_trapped[pid] == 1)
			{
				int old_pa, old_idx;
				__k1_tlb_entry_t tlb;
				if(__try_patch_mmu_entry(dst, &old_pa, &old_idx, &tlb)) {
					__in_debug_trapped[pid] = 0;

					__builtin_k1_barrier ();
					*((uint64_t *) dst) = tmp;
					__builtin_k1_barrier ();

					__unpatch_mmu_entry(old_pa, old_idx, tlb);
					if(__in_debug_trapped[pid] == 1) {
						ret = 0;
						goto end;
					}
				} else {
					ret = 0;
					goto end;
				}
			}

			src += 8;
			dst += 8;
		}
	}

	if ((((unsigned int) src) & 3) == 0 && (((unsigned int) dst) & 3) == 0)
	{
		while (src_end - src >= 4)
		{
			__builtin_k1_barrier ();
			uint32_t tmp = *((uint32_t *) src);
			__builtin_k1_barrier ();
			*((uint32_t *) dst) = tmp;
			__builtin_k1_barrier ();

			if (__in_debug_trapped[pid] == 1)
			{
				int old_pa, old_idx;
				__k1_tlb_entry_t tlb;
				if(__try_patch_mmu_entry(dst, &old_pa, &old_idx, &tlb)) {
					__in_debug_trapped[pid] = 0;

					__builtin_k1_barrier ();
					*((uint32_t *) dst) = tmp;
					__builtin_k1_barrier ();

					__unpatch_mmu_entry(old_pa, old_idx, tlb);
					if(__in_debug_trapped[pid] == 1) {
						ret = 0;
						goto end;
					}
				} else {
					ret = 0;
					goto end;
				}
			}

			src += 4;
			dst += 4;
		}
	}

	while (src < src_end)
	{
		__builtin_k1_barrier ();
		unsigned char tmp = *src;
		__builtin_k1_barrier ();
		*dst = tmp;
		__builtin_k1_barrier ();

		if(__in_debug_trapped[pid] == 1)
		{
			int old_pa, old_idx;
			__k1_tlb_entry_t tlb;
			if(__try_patch_mmu_entry(dst, &old_pa, &old_idx, &tlb)) {
				__in_debug_trapped[pid] = 0;

				__builtin_k1_barrier ();
				*dst = tmp;
				__builtin_k1_barrier ();

				__unpatch_mmu_entry(old_pa, old_idx, tlb);
				if(__in_debug_trapped[pid] == 1) {
					ret = 0;
					goto end;
				}
			} else {
				ret = 0;
				goto end;
			}
		}

		src++;
		dst++;
	}

end:
	__in_debug_might_trap[pid] = 0;
	__in_debug_trapped[pid] = 0;
	__builtin_k1_wpurge ();
	__builtin_k1_fence ();

	return ret;
}




int probe_address_space (unsigned int addr, int size)
{
	unsigned int addr_end = addr + size;
	int ret = 0;

	if (&VIRT_U_MEM_PAG == 0) {
		if (addr >= (unsigned int) &VIRT_U_MEM_START && addr_end < (unsigned int) &VIRT_U_MEM_END)
			ret = 1;
	} else {
		if (addr >= (unsigned int) &VIRT_U_MEM_START && addr_end < (unsigned int) &INTERNAL_RAM_SIZE)
			ret = 1;
	}

	if (addr >= CLUSTER_PERIPH_START && addr_end < CLUSTER_PERIPH_END)
		ret = 1;

#ifdef __k1io__
	if (addr >= (unsigned int) &DDR_START && addr_end < (unsigned int) &DDR_START + (unsigned int) &DDR_SIZE)
		ret = 1;

	if (addr >= IO_PERIPH_START && addr_end < IO_PERIPH_END)
		ret = 1;
#endif
	if (&VIRT_U_MEM_PAG != 0)
	{
		if (addr >= (unsigned int) &VIRT_U_MEM_PAG && addr_end < (unsigned int) &VIRT_U_MEM_END)
			ret = 1;
	}

	return ret;
}

void __vk1_debug_handler (void) {
	struct dsu_os_s *buf_os = get_os_mem();
	buf_os->status = 0;

	mOS_per_cpu_data_t *mos_cpu =
		&_scoreboard_start.SCB_VCORE.PER_CPU[__k1_get_cpu_id ()];
	uint32_t sfr_sspc = mos_cpu->SFR_SSPC;
	uint32_t sfr_ssps = mos_cpu->SFR_SSPS.word;
	uint32_t sfr_sssp = mos_cpu->SFR_SSSP;
	uint32_t sfr_ea = mos_cpu->SFR_EA;

#ifndef __k1a__
	uint32_t sfr_spc = mos_cpu->SFR_SPC;
	uint32_t sfr_sps = mos_cpu->SFR_SPS.word;
	uint32_t sfr_ssp = mos_cpu->SFR_SSP;
#endif

	switch (buf_os->cmd)
	{
	case DSU_OS_READ_CMD:
	{
		//xputs ("§§§read_cmd\n");
		unsigned char *src = (unsigned char *) buf_os->cmds.read_cmd.addr;
		unsigned char *dst =  (unsigned char *) buf_os->cmds.read_cmd.data;
		unsigned char *src_end = (unsigned char *) (src + 4 * buf_os->cmds.read_cmd.size);
		int size = src_end - src;

		if (!probe_address_space ((unsigned int) src, size) ||
		    !__debug_handler_read_write_mem (dst, src, src_end))
		{
			buf_os->status = 1;
		}

		break;
	}
	case DSU_OS_WRITE_CMD:
	{
		//xputs ("§§§write_cmd\n");
		unsigned char *src = buf_os->cmds.write_cmd.data;
		unsigned char *dst = buf_os->cmds.write_cmd.addr;
		unsigned char *src_end = src + buf_os->cmds.write_cmd.size;
		int size = (unsigned int) src_end - (unsigned int) src;

		if (!probe_address_space ((unsigned int) dst, size))
		{
			buf_os->status = 1;
			break;
		}


		if (!__debug_handler_read_write_mem (dst, src, src_end))
		{
			buf_os->status = 1;
		}

		break;
	}
	default:
		xputs ("mOS.debug_handler invalid command\n");
		break;
	}

#ifndef __k1a__
	mos_cpu->SFR_SPC = sfr_spc;
	mos_cpu->SFR_SPS.word = sfr_sps;
	mos_cpu->SFR_SSP = sfr_ssp;
#endif
	mos_cpu->SFR_SSPC = sfr_sspc;
	mos_cpu->SFR_SSPS.word = sfr_ssps;
	mos_cpu->SFR_SSSP = sfr_sssp;
	mos_cpu->SFR_EA = sfr_ea;
}

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

	xputs ("Entering in idle mode!");
	/* go to sleep for a while */
	do {
		mOS_idle1();
	} while (1);
}

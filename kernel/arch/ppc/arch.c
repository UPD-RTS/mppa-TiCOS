/*
 *  Copyright (c) 2012  University of Padua, Department of Mathematics
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
 *  This file incorporates work covered by the following copyright and
 *  permission notice:
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
 *		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *		ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *		POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file	 arch/ppc/arch.c
 * \brief	Provide generic architecture access for PPC architecture
 */

#include <bsp.h>
#include <types.h>
#include <errno.h>
#include <core/partition.h>
#include "msr.h"
#include "hid0.h"
#include <core/time.h>
#ifdef POK_NEEDS_DEBUG
	#include <libc.h>
#endif 

/** Patches for arch.c
 *  [1] pok_arch_init modified to enable FP bit in MSR
 *  [2] pok_arch_idle modified to enable interrupts for the IDLE thread
 *  [3] pok_arch_cache_freeze/unfreeze procedures
 */

extern void pok_arch_space_init (void);
extern unsigned long long time_new;
extern void set_ppc_tb(unsigned long long);

pok_ret_t	pok_arch_cache_enable();
#ifdef POK_NEEDS_DEBUG
pok_ret_t	pok_arch_cache_read_HID0();
#endif

static inline unsigned int get_msr (void)
{
	unsigned int res;
	asm ("mfmsr %0\n" : "=r" (res));	
	return res;
}

static inline void set_msr (unsigned int val)
{
	asm volatile ("mtmsr %0\n" : : "r" (val));
}

#ifdef POK_NEEDS_DEBUG
pok_ret_t	pok_arch_cache_read_HID0 ()
{
	unsigned int hid_reg = 0;
	/* HID0 reg is 1008 */
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	printf("* in read_HID0, after mfspr: hid_reg 0x%x\n", hid_reg);
	return (POK_ERRNO_OK);
}
#endif


pok_ret_t pok_arch_init ()
{
	set_msr (MSR_IP|MSR_FP);
#ifdef POK_NEEDS_DEBUG
  	pok_arch_cache_read_HID0();
#endif
#if POK_NEEDS_PARTITIONS
	pok_arch_space_init();
#endif
	return POK_ERRNO_OK;
}

pok_ret_t pok_arch_preempt_disable()
{
	unsigned int msr;
	msr = get_msr();
	msr &= ~MSR_EE;
	set_msr(msr);
	return (POK_ERRNO_OK);
}

pok_ret_t pok_arch_preempt_enable()
{
	unsigned int msr;
	msr = get_msr();
	msr |= MSR_EE;
	set_msr(msr);

	return (POK_ERRNO_OK);
}

pok_ret_t pok_arch_idle()
{
	/* enable caches */
	pok_arch_cache_enable();
	
	/** 
	  reenable the MSR_EE bit after the first sycall which elects
	  the kernel IDLE thread for execution.
	  This allows to avoid having interrupts enabled during syscalls
	*/
	unsigned int msr;
	msr = get_msr();
	msr |= MSR_EE;
	set_msr(msr);

	while (1)
	{
#ifdef POK_NEEDS_SCHED_O1
		uint64_t now = POK_GETTICK();
	
 #ifdef POK_SKIP_IDLE
		if (POK_CURRENT_PARTITION.head_asynch_queue == NULL)
		{
			set_ppc_tb(time_new);
		}
 #endif
		if (POK_CURRENT_PARTITION.head_asynch_queue != NULL && POK_CURRENT_PARTITION.head_asynch_queue->timestamp <= now)
		{
			//pok_sched_service_asynch_events();
			pok_sched_end_period();
			msr = get_msr();
			msr |= MSR_EE;
			set_msr(msr);
		}
#endif
	}
	return (POK_ERRNO_OK);	
}

uint32_t	 pok_thread_stack_addr	(const uint8_t	partition_id, const uint32_t	local_thread_id)
{
	return pok_partitions[partition_id].size - (local_thread_id * POK_USER_STACK_SIZE);
}


pok_ret_t	pok_arch_cache_enable()
{
	
	unsigned int hid_reg;

	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	hid_reg |= (HID0_ICE|HID0_DCE);
	//asm volatile ("sync");
	asm volatile ("mtspr 1008, %0\n" : : "r" (hid_reg));
	//asm volatile ("sync");

#ifdef POK_NEEDS_DEBUG
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
#endif
	return (POK_ERRNO_OK);
}


pok_ret_t	pok_arch_Icache_enable()
{
	unsigned int hid_reg;

	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));	
	hid_reg |= HID0_ICE;
	//asm volatile ("sync");
	asm volatile ("mtspr 1008, %0\n" : : "r" (hid_reg));
	//asm volatile ("sync");
#ifdef POK_NEEDS_DEBUG
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	printf("**** in Icache enable after set ice 0x%x\n", hid_reg);
#endif
	return POK_ERRNO_OK;
}

pok_ret_t	pok_arch_save_cache_state (unsigned int *cache_state)
{
	unsigned int hid_reg;
	/* HID0 reg is 1008 */
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	*cache_state =  hid_reg;
#ifdef POK_NEEDS_DEBUG
	printf("* in save_cache_state  hid_reg 0x%x\n", hid_reg);
#endif
	return POK_ERRNO_OK;
}

pok_ret_t	pok_arch_restore_cache_state (unsigned int cache_state)
{

	/* HID0 reg is 1008 */
#ifdef POK_NEEDS_DEBUG
	printf("* in is_restore_cache_state  cache_state 0x%x\n", cache_state);
#endif
	//asm volatile ("sync");
	asm volatile ("mtspr 1008, %0\n" : : "r" (cache_state));
	//asm volatile ("sync");
	
#ifdef POK_NEEDS_DEBUG
	unsigned int hid_reg;
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	printf("* in is_restore_cache_state  hid_reg 0x%x\n", hid_reg);
#endif
	return (POK_ERRNO_OK);
}

pok_ret_t	pok_arch_cache_invalidate ()
{
	unsigned int hid_reg;
	/* HID0 reg is 1008 */
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
#ifdef POK_NEEDS_DEBUG
	printf("* in cache invalidation  hid_reg 0x%x\n", hid_reg);
#endif
	hid_reg |= (HID0_ICFI|HID0_DCFI);	/* set ICFI and DCFI*/
#ifdef POK_NEEDS_DEBUG
	printf("**** in cache invalidation  hid_reg 0x%x\n", hid_reg);
#endif
	//asm volatile ("sync");
	asm volatile ("mtspr 1008, %0\n" : : "r" (hid_reg));
	//asm volatile ("sync");
#ifdef POK_NEEDS_DEBUG
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	//asm volatile ("sync");
	printf("***** in cache invalidation  hid_reg 0x%x\n", hid_reg);
#endif
	return (POK_ERRNO_OK);
}

/** 
 * Freeze and enable both caches 
 */
pok_ret_t	pok_arch_cache_freeze_and_enable ()
{
	unsigned int hid_reg;
	/* HID0 reg is 1008 */
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
#ifdef POK_NEEDS_DEBUG
	printf ("* in freeze and enable hid_reg: 0x%x\n",hid_reg);
#endif	
	hid_reg |= (HID0_ILOCK|HID0_DLOCK);	// locking caches
	hid_reg |= (HID0_ICE|HID0_DCE);		// enabling caches
	//asm volatile ("sync");
	asm volatile ("mtspr 1008, %0\n" : : "r" (hid_reg));
	//asm volatile ("sync");

#ifdef POK_NEEDS_DEBUG
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	//asm volatile ("sync");
	printf ("** in freeze and enable hid_reg: 0x%x\n",hid_reg);
#endif 
	return (POK_ERRNO_OK);
}

/** 
 * Freeze both caches (Zero-disturbance)
 */
pok_ret_t	pok_arch_cache_freeze ()
{
	unsigned int hid_reg;
	/* HID0 reg is 1008 */
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
#ifdef POK_NEEDS_DEBUG
	printf ("* in freeze hid_reg: 0x%x\n",hid_reg);
#endif	
	hid_reg |= (HID0_ILOCK|HID0_DLOCK);
	//asm volatile ("sync");
	asm volatile ("mtspr 1008, %0\n" : : "r" (hid_reg));
	//asm volatile ("sync");

#ifdef POK_NEEDS_DEBUG
	printf ("** in freeze hid_reg: 0x%x\n",hid_reg);
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));
	//asm volatile ("sync");
	printf("***in freeze  hid_reg 0x%x\n", hid_reg);
#endif 
	return (POK_ERRNO_OK);
}

/** 
 * Unfreeze both caches (Zero-disturbance)
 */
pok_ret_t	pok_arch_cache_unfreeze ()
{
	unsigned int hid_reg;
	/* HID0 reg is 1008 */
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg));

#ifdef POK_NEEDS_DEBUG
	printf ("* in unfreeze hid_reg: 0x%x\n",hid_reg);
#endif
	hid_reg &= ~(HID0_ILOCK|HID0_DLOCK); // cache unlock
#ifdef POK_NEEDS_DEBUG
	printf ("** in unfreeze hid_reg: 0x%x\n",hid_reg);
#endif
	//asm volatile ("sync");
	asm volatile ("mtspr 1008, %0\n" : : "r" (hid_reg));
	//asm volatile ("sync");

#ifdef POK_NEEDS_DEBUG	
	unsigned int hid_reg_dacanc =0;
	asm volatile ( "mfspr %0, 1008\n" : "=r" (hid_reg_dacanc));
	//asm volatile ("sync");
	printf("***in unfreeze  hid_reg_dacanc 0x%x\n", hid_reg_dacanc);
#endif
	return (POK_ERRNO_OK);
}

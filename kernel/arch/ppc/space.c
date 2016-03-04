/*  
 * Copyright (c) 2012  University of Padua, Department of Mathematics  
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
 *		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *		ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *		POSSIBILITY OF SUCH DAMAGE.
 */


#include <types.h>
#include <errno.h>
#include <libc.h>
#include <bsp.h>
#include <core/sched.h>

#include <arch.h>
#include "thread.h"
#include "msr.h"


#define KERNEL_STACK_SIZE 16384

#define PPC_SR_KP (1 << 29)
#define PPC_SR_Ks (1 << 30)
#define PPC_SR_T  (1 << 31)

#ifdef POK_CONFIG_DEFINE_BASE_VADDR 
uint32_t pok_space_vaddr[POK_CONFIG_NB_PARTITIONS] = (uint32_t[]) POK_CONFIG_PARTITIONS_BASE_VADDR;
#endif

struct pok_space
{
	uint32_t phys_base;
	uint32_t size;
};

struct pok_space spaces[POK_CONFIG_NB_PARTITIONS];

extern void exit(int level);
void pok_init_page_table_entries ( uint8_t, uint32_t, uint32_t);

pok_ret_t pok_create_space (uint8_t partition_id,
									 uint32_t addr,
									 uint32_t size)
{
#ifdef POK_NEEDS_DEBUG
	printf ("pok_create_space: %d: %x %x\n", partition_id, addr, size);
#endif
	spaces[partition_id].phys_base = addr;
	spaces[partition_id].size = size;

#ifdef POK_PREFILL_PTE
 #ifdef POK_NEEDS_DEBUG
 	printf ("[P%d]  pok_init_page_table_entries\n", partition_id);
 #endif
	pok_init_page_table_entries(partition_id,size,addr);
#endif

  return (POK_ERRNO_OK);
}

pok_ret_t pok_space_switch (uint8_t old_partition_id,
									 uint8_t new_partition_id)
{
	(void) old_partition_id;
#ifdef POK_NEEDS_DEBUG
	printf ("space_switch %u -> %u\n", old_partition_id, new_partition_id); 
#endif
	asm volatile ("mtsr %0,%1" : : "r"(0), "r"(PPC_SR_KP | new_partition_id));
	return (POK_ERRNO_OK);
}

#ifdef POK_CONFIG_DEFINE_BASE_VADDR 
uint32_t userdef_base_vaddr (uint8_t partition_id)
{
	return (uint32_t)pok_space_vaddr[partition_id];
}
#endif

uint32_t pok_space_base_vaddr (uint32_t addr)
{
	(void) addr;
	/** 
	 * Modified to skip the first 4KB page 
	 * Also the partition linker script has been modified
	 */
#ifdef POK_CONFIG_DEFINE_BASE_VADDR
	static uint8_t space_id = 0;
	uint32_t vaddr = userdef_base_vaddr(space_id++);
	return vaddr;
#else
	return 4096;
#endif
}

extern void pok_arch_rfi (void);

uint32_t pok_space_context_create (uint8_t partition_id,
											  uint32_t entry_rel,	// for main thread is the address of the main routine (__pok_partition_start) that call the user main
											  uint32_t stack_rel,  // init_stack_addr (points inside the partition memory)
											  uint32_t arg1,
											  uint32_t arg2)
{
  context_t* ctx;
  volatile_context_t* vctx;
  char*		stack_addr;
  (void) partition_id;

  stack_addr = pok_bsp_mem_alloc (KERNEL_STACK_SIZE);

  vctx = (volatile_context_t *)
	 (stack_addr + KERNEL_STACK_SIZE - sizeof (volatile_context_t));
#ifdef POK_NEEDS_DEBUG
  printf ("vctx starts at: 0x%x \n", vctx);
#endif
  ctx = (context_t *)((char *)vctx - sizeof (context_t) + 8);
#ifdef POK_NEEDS_DEBUG
  printf ("ctx starts at: 0x%x \n", ctx);
#endif
  memset (ctx, 0, sizeof (*ctx));
  memset (vctx, 0, sizeof (*vctx));

  vctx->r3	  = arg1;
  vctx->r4	  = arg2;
  vctx->sp	  = stack_rel - 12 ; 

  vctx->srr0	= entry_rel;
  /** enable FP bit in MSR */
  vctx->srr1	= MSR_EE | MSR_IP | MSR_DR | MSR_IR | MSR_PR | MSR_FP;
  ctx->lr		= (uint32_t) pok_arch_rfi;

  ctx->sp		= (uint32_t) &vctx->sp;

#ifdef POK_NEEDS_DEBUG
  printf ("space_context_create %d: entry=%x stack=%x arg1=%x arg2=%x ksp=%x\n",
			 partition_id, entry_rel, stack_rel, arg1, arg2, &vctx->sp);
#endif

  return (uint32_t)ctx;
}

typedef struct
{
  uint32_t vsid_api;
  uint32_t rpn_flags;
} ppc_pte_t;

static uint32_t pt_base;
static uint32_t pt_mask;

#define PPC_PTE_V (1 << 31)
#define POK_PAGE_SIZE (1 << 12)
#define POK_PAGE_MASK (~(POK_PAGE_SIZE - 1))
#define PPC_PTE_H (1 << 6)
#define PPC_PTE_R (1 << 8)
#define PPC_PTE_C (1 << 7)
#define PPC_PTE_W (1 << 6)
#define PPC_PTE_I (1 << 5)
#define PPC_PTE_M (1 << 4)
#define PPC_PTE_G (1 << 3)
#define PPC_PTE_PP_NO 0
#define PPC_PTE_PP_RO 1
#define PPC_PTE_PP_RW 2

void pok_arch_space_init (void)
{
  uint32_t sdr1;

  pt_base = 0;
  pt_mask = 0x3ff;

  sdr1 = pt_base | (pt_mask >> 10);
  asm volatile ("mtsdr1 %0" : : "r"(sdr1));
}

static void pok_insert_pte (uint32_t vsid, uint32_t vaddr, uint32_t pte)
{
/*
	 printf ("------------------------ \n");
	 printf ("current partition id: %d \n", vsid);
	 printf ("virtual address: 0x%x \n", vaddr);
	 printf ("page table entry: 0x%x \n", pte);
	 printf ("------------------------ \n");
*/
  uint32_t hash = (vsid & 0x7ffff) ^ ((vaddr >> 12) & 0xffff);
  ppc_pte_t *pteg;
  int i;

  pteg = (ppc_pte_t *)(pt_base + ((hash & pt_mask) << 6));
  for (i = 0; i < 8; i++)
	 if (!(pteg[i].vsid_api & PPC_PTE_V))
		{
		  pteg[i].rpn_flags = pte;
		  pteg[i].vsid_api = PPC_PTE_V | (vsid << 7) | ((vaddr >> 22) & 0x3f);
		  return;
		}

  /* FIXME: Try secondary hash.  */

#ifdef POK_NEEDS_DEBUG
  printf ("pok_insert_pte: no free entry\n");
#endif

#ifdef POK_NEEDS_QEMU_SETUP
			 while (1)
		  ;
#else
		  exit(-1); 
#endif

}

void pok_init_page_table_entries (uint8_t pid,
											uint32_t psize,
											uint32_t pbase) {

	uint32_t v, vaddr ;

	for ( vaddr=0; vaddr < psize ; vaddr+=POK_PAGE_SIZE) {
		v = (pbase + vaddr) & POK_PAGE_MASK;
 		v |= PPC_PTE_R | PPC_PTE_C | PPC_PTE_PP_RW;
		 /** Define here proper WMIG bits if needed
			E.g., v |= PPC_PTE_W | PPC_PTE_I | PPC_PTE_M | PPC_PTE_G;
			*/
		pok_insert_pte ( pid, vaddr, v);
	}
	return;
}



void pok_arch_isi_int (uint32_t pc, uint32_t msr)
{

#ifdef POK_NEEDS_DEBUG
  printf("isi_int: part=%d, pc=%x msr=%x phys_base=%x\n",
			pok_current_partition, pc, msr, spaces[pok_current_partition].phys_base);

  if (msr & ((1 << 28) | (1 << 27)))
  {
	 printf (" Bad access\n");
  }
#endif

	
  if (msr & (1 << 30))
	 {
		/* Page fault  */
		if (pc < spaces[pok_current_partition].size)
		  {
			 uint32_t vaddr = pc & POK_PAGE_MASK;
			 uint32_t v;
			 v = (spaces[pok_current_partition].phys_base + vaddr) & POK_PAGE_MASK;
			 v |= PPC_PTE_R | PPC_PTE_C | PPC_PTE_PP_RW;
			 /** Define here proper WMIG bits if needed
				  E.g., v |= PPC_PTE_W | PPC_PTE_I | PPC_PTE_M | PPC_PTE_G;
			 */
			 pok_insert_pte (pok_current_partition, vaddr, v);
			 return;
		  }
	 }

	
  
#ifdef POK_NEEDS_DEBUG
	printf("[DEBUG] Infinite loop in pok_arch_isi_int\n");
#endif 
	exit(256);
	while (1)
	;
}

void pok_arch_dsi_int (uint32_t dar, uint32_t dsisr)
{
#ifdef POK_NEEDS_DEBUG
  printf("dsi_int: part=%d, dar=%x dsisr=%x\n",
			pok_current_partition, dar, dsisr);
#endif


  
  if (dsisr & (1 << 30))
	 {
		/* Page fault  */
		if (dar < spaces[pok_current_partition].size)
		  {
			 uint32_t vaddr = dar & POK_PAGE_MASK;
			 uint32_t v;
			 v = (spaces[pok_current_partition].phys_base + vaddr) & POK_PAGE_MASK;
			 v |= PPC_PTE_R | PPC_PTE_C | PPC_PTE_PP_RW;
			 /**  Define here proper WMIG bits if needed
				  E.g., v |= PPC_PTE_W | PPC_PTE_I | PPC_PTE_M | PPC_PTE_G;
			 */
			 pok_insert_pte (pok_current_partition, vaddr, v);
			 return;
		  }
	 }
#ifdef POK_NEEDS_DEBUG
	printf("[DEBUG] Infinite loop in pok_arch_dsi_int\n");
#endif

	exit(255);
	while (1)
	;
}

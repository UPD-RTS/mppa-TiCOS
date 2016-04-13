/*
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
#include <errno.h>
#include <bsp.h>
#include <arch.h>

#ifdef POK_NEEDS_DEBUG
#include <libc.h>
#endif

#include "thread.h"

#define KERNEL_STACK_SIZE 8192

#ifdef POK_CONFIG_DEFINE_BASE_VADDR
uint32_t pok_space_vaddr[POK_CONFIG_NB_PARTITIONS] =
	{ [0 ... POK_CONFIG_NB_PARTITIONS - 1 ] POK_CONFIG_PARTITIONS_BASE_VADDR };
#endif

// Struct representing physical space reserved
struct pok_space
{
	uint32_t phys_base;
	uint32_t size;
};

// Physical space allocation for each partition
struct pok_space spaces[POK_CONFIG_NB_PARTITIONS];

// Extern exit method
extern void exit(int level);
extern void context_restore (uint32_t ctx);

pok_ret_t pok_create_space (uint8_t partition_id,
									 uint32_t addr,
									 uint32_t size)
{
#ifdef POK_NEEDS_DEBUG
	printf ("pok_create_space: %d: %x %x\n", partition_id, addr, size);
#endif
	spaces[partition_id].phys_base = addr;
	spaces[partition_id].size = size;

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


// Creates context's space for the specified thread,
// stack_rel is the pointer to the stack
// shadow_stack_rel is the pointer to the shadow_stack
// entry_rel is the pointer to the entry point of the thread
// for main thread is the address of the main routine (__pok_partition_start)
// that call the user main
uint32_t pok_space_context_create (uint8_t partition_id, uint32_t entry_rel,
				uint32_t stack_rel, uint32_t arg1, uint32_t arg2)
{
	context_t *ctx;
	char *stack_addr;

	(void) partition_id;

	stack_addr = pok_bsp_mem_alloc (KERNEL_STACK_SIZE);
	ctx = (context_t *) (stack_addr + KERNEL_STACK_SIZE - sizeof(context_t));

	memset (ctx, 0, sizeof (*ctx));

	// Return from interrupt address (entry point of the thread) and arguments
	ctx->k1_base_ctx.regs[0] = arg1;
	ctx->k1_base_ctx.regs[1] = arg2;
	ctx->k1_base_ctx.regs[18] = entry_rel;

	// Return address
	ctx->k1_base_ctx.ra = (uint32_t) context_restore;

	// Setting stack pointer
	ctx->k1_base_ctx.regs[12] = (uint32_t) stack_rel;

	// Initial exception state has interrupts enabled
	//ctx->exc    = 0x1;

#ifdef POK_NEEDS_DEBUG
	printf ("[DEBUG]\t Creating context for partition %d, ctx: %p, entry: %x\n",
	        partition_id, ctx, entry_rel);
	printf ("[DEBUG]\t Creating context for partition %d, stack: %x\n",
	        partition_id, stack_rel );
#endif

	return (uint32_t)ctx;
}

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
 * \file	 kernel/include/arch.h
 * \brief	Generic interface to handle architectures
 */

#ifndef __POK_ARCH_H__
#define __POK_ARCH_H__

#include <types.h>
#include <errno.h>

/**
 * Function that initializes architecture concerns.
 */
pok_ret_t	pok_arch_init ();

/**
 * Disable interruptions
 */
pok_ret_t	pok_arch_preempt_disable ();

/**
 * Enable interruptions
 */
pok_ret_t	pok_arch_preempt_enable ();

/** 
 * Freeze I- and D-cache
 */
pok_ret_t	pok_arch_cache_freeze ();

/** 
 * Unfreeze I- and D-cache
 */
pok_ret_t	pok_arch_cache_unfreeze ();

/** 
 * Invalidate I- and D- cache
 */
pok_ret_t	pok_arch_cache_invalidate ();

/** 
 * Enable and freeze I- and D- cache
 */
pok_ret_t	pok_arch_cache_freeze_and_enable ();
/** 
 * Enable I- and D-cache
 */
pok_ret_t	pok_arch_cache_enable();

/** 
 * Enable I-cache
 */
pok_ret_t	pok_arch_Icache_enable();

/** 
 * Save cache state
 */
pok_ret_t	pok_arch_save_cache_state (unsigned int *cache_state);

/** 
 * Restore cache state
 */
pok_ret_t	pok_arch_restore_cache_state (unsigned int cache_state);


#ifdef POK_NEEDS_DEBUG
pok_ret_t	pok_arch_cache_read_HID0 ();
#endif


/**
 * Function used in the idle task 
 */
pok_ret_t	pok_arch_idle ();

uint32_t	pok_context_create (uint32_t thread_id,
								uint32_t stack_size,
								uint32_t entry);

void		pok_context_switch (uint32_t* old_sp, uint32_t new_sp);

pok_ret_t	pok_create_space (uint8_t partition_id, uint32_t addr, uint32_t size);

uint32_t	pok_space_base_vaddr (uint32_t addr);

void		pok_dispatch_space (uint8_t partition_id,
								uint32_t user_pc,
								uint32_t user_sp,
								uint32_t kernel_sp,
								uint32_t arg1,
								uint32_t arg2);

/**
 * Create a new context in the given space
 */
uint32_t		pok_space_context_create (uint8_t partition_id,
										  uint32_t entry_rel,
										  uint32_t stack_rel,
										  uint32_t arg1,
										  uint32_t arg2);


void pok_space_context_restart (uint32_t sp, uint32_t entry, uint32_t user_stack);

/**
 * Switch from one space to another
 */
pok_ret_t	pok_space_switch (uint8_t old_partition_id, uint8_t new_partition_id);

/**
 * Returns the stack address for a the thread number N
 * in a partition.
 *
 * @arg partition_id indicates the partition that contains
 * the thread.
 *
 * @arg local_thread_id the thread-id of the thread
 * inside the partition.
 *
 * @return the stack address of the thread.
 */
uint32_t	 pok_thread_stack_addr	(const uint8_t	 partition_id, const uint32_t	local_thread_id);

#ifdef POK_ARCH_PPC
#include <arch/ppc/spinlock.h>
#endif

#endif /* !__POK_ARCH_H__ */

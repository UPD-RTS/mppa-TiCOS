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

#ifndef __POK_MPPA_THREAD_H__
#define __POK_MPPA_THREAD_H__

#include <types.h>
#ifdef POK_NEEDS_DEBUG
#include <libc.h>
#endif

/**
 * @brief  Context type including mOS additional registers (ssp and sssp)
 *
 * k1_base_ctx is defined as:
 *
 * struct __k1_context {
 *	union {
 *		__k1_uint32_t regs[64]; // 64 GRP registers
 *		__k1_uint64_t force_align[32];
 *	};
 *	// System function registers (There are 64, 50 used and few important for the ctx)
 *	__k1_uint64_t spc; //shadow program counter
 *	__k1_uint64_t sps; //shadow processing status
 *	__k1_uint64_t ra; //return address
 *	__k1_uint32_t cs; //compute status
 *	__k1_uint32_t lc; //loop counter
 *	__k1_uint64_t ps; //processing status
 *	__k1_uint64_t ls; //loop start address
 *	__k1_uint64_t le; //loop exit address
 *
 * };
 *
 */
typedef struct
{
	__k1_context_t k1_base_ctx;
	__k1_uint32_t ssp;
	__k1_uint32_t sssp;

} context_t;

uint32_t pok_context_create(uint32_t id,uint32_t stack_size, uint32_t entry);

/* Interface not yet completely defined */
//void pok_context_switch(uint32_t *old_sp, uint32_t new_sp);
void pok_context_switch(uint32_t *old_sp, uint32_t new_sp);

#ifdef POK_NEEDS_DEBUG
void pok_context_print(context_t *ctx);
#endif

#endif /* __POK_MPPA_THREAD_H__ */

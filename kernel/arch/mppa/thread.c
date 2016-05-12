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

#include <bsp.h>
#include <core/thread.h>
#ifdef POK_NEEDS_DEBUG
#include <libc.h>
#endif

#include "thread.h"

#ifdef POK_NEEDS_THREADS

extern void pok_arch_thread_start(void);

// Used to create special threads context (idle and kernel threads)
uint32_t pok_context_create (uint32_t id, uint32_t stack_size, uint32_t entry)
{
	context_t *ctx;
	char *stack_addr;

	// Allocate stack space
	stack_addr = pok_bsp_mem_alloc (
		sizeof (context_t) 	+
		stack_size);

	// find space for the threads context
	ctx = (context_t *) (stack_addr + stack_size);

	// Set context to 0
	memset (ctx, 0, sizeof (context_t));

	/* Setting temporary entry point and identifier of the
	 * thread, used only before starting it (then can be modified)
	 * r18 and r19 are just GRP */
	ctx->k1_base_ctx.regs[18] = entry;
	ctx->k1_base_ctx.regs[19] = id;

	//initialize ps and sps with default values
	ctx->k1_base_ctx.ps = pok_default_ps.word;
	ctx->k1_base_ctx.sps = pok_default_ps.word;

	// Setting base function address to return to
	ctx->k1_base_ctx.spc = (uint32_t) pok_arch_thread_start;

	// Setting stack pointer
	ctx->ssp = (uint32_t) ctx - 16;

	#ifdef POK_NEEDS_DEBUG
	printf ("[DEBUG]\t Context size %x\n",sizeof (context_t));
	printf ("[DEBUG]\t Creating system context %x, ctx: %p, sp: %x\n",
				(unsigned int)id, ctx, ctx->ssp);
	#endif
	return (uint32_t)ctx;
}

#ifdef POK_NEEDS_DEBUG

#define NB_REGS_PERLINE 4

/* Prints the context */
void pok_context_print(context_t *ctx)
{

	int i = 0, j = 0;

	for(i = 0; i < (64/NB_REGS_PERLINE); i++) {

		for(j = 0; j < NB_REGS_PERLINE; j++) {
			printf("+-----+------------");
		}
		printf("+\n");

		for(j = 0; j < NB_REGS_PERLINE; j++) {
			printf("| r%02d | 0x%08x ", NB_REGS_PERLINE*i + j,
				ctx->k1_base_ctx.regs[NB_REGS_PERLINE*i+j]);
		}
		printf("|\n");
	}
	for(j = 0; j < NB_REGS_PERLINE; j++) {
		printf("+-----+------------");
	}
	printf("+\n");
	printf("| PS  | 0x%08x | RA  | 0x%08x | SPS | 0x%08x | SPC | 0x%08x |\n",
	(unsigned int)ctx->k1_base_ctx.ps, (unsigned int)ctx->k1_base_ctx.ra,
	(unsigned int)ctx->k1_base_ctx.sps, (unsigned int)ctx->k1_base_ctx.spc);
	printf("+-----+------------+-----+------------+-----+------------+-----+------------+\n");
	printf("| SSP  | 0x%08x | SSSP  | 0x%08x | \n",
				(unsigned int)ctx->ssp, (unsigned int)ctx->sssp);
	printf("+-----+------------+-----+------------+-----+------------+-----+------------+\n");

}
#endif /*POK_NEEDS_DEBUG*/
#endif /*POK_NEEDS_THREADS*/

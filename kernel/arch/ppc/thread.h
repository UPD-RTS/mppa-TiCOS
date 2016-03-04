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

#ifndef __POK_PPC_THREAD_H__
#define __POK_PPC_THREAD_H__

#include <types.h>

/** Non-volatile context modified to accomodate FP registers */
typedef struct
{
	uint32_t sp;	/* 0 */
	uint32_t unused_lr;  

	uint32_t cr;	/* 8 */
	uint32_t r2;
	uint32_t r13;	/* 16 */
	uint32_t r14;
	uint32_t r15;	/* 24 */
	uint32_t r16;
	uint32_t r17;	/* 32 */
	uint32_t r18;
	uint32_t r19;	/* 40 */
	uint32_t r20;
	uint32_t r21;	/* 48 */
	uint32_t r22;
	uint32_t r23;	/* 56 */
	uint32_t r24;
	uint32_t r25;	/* 64 */
	uint32_t r26;
	uint32_t r27;	/* 72 */
	uint32_t r28;
	uint32_t r29;	/* 80 */
	uint32_t r30;
	uint32_t r31;	/* 88 */

	uint32_t pad0;	/* 92 */
	/** FPSCR is a 32bit register but must be handled with 64bit op */
	uint32_t fpscr_pad;  /* 96 */
	uint32_t fpscr_val;  /* 100 */
	/** FP registers */
	uint64_t fpr14;	/* 104 */
	uint64_t fpr15;	
	uint64_t fpr16;	/* 120 */
	uint64_t fpr17;
	uint64_t fpr18;	/* 136 */
	uint64_t fpr19;
	uint64_t fpr20;	/* 152 */
	uint64_t fpr21;
	uint64_t fpr22;	/* 168 */
	uint64_t fpr23;
	uint64_t fpr24;	/* 184 */
	uint64_t fpr25;
	uint64_t fpr26;	/* 200 */
	uint64_t fpr27; 
	uint64_t fpr28;	/* 216 */
	uint64_t fpr29;
	uint64_t fpr30;	/* 232 */
	uint64_t fpr31;	/* 240 */

	/** The old padding is not enough for 
	* the struct to be quadword aligned
	*/
	uint32_t pad;	/* 248 */
	uint32_t pad1;	/* 254 */
	/* Previous frame.  */
	uint32_t back_chain;	/* 256 */
	uint32_t lr;	/* 260 */
} context_t;		/* 264 bytes */

/** Volatile context modified to accomodate FP registers */
typedef struct
{
	uint32_t sp;
	uint32_t unused_lr;

	uint32_t cr; 	/* 8 */
	uint32_t r0;
	uint32_t r2; 	/* 16 */
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;	/* 32 */
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10; 	/* 48 */
	uint32_t r11;
	uint32_t r12;
	uint32_t r13;
	uint32_t ctr; 	/* 64 */
	uint32_t xer;
	uint32_t srr0;
	uint32_t srr1; 	/* 76 */
	/** FPSCR is a 32bit register but must be handled with 64bit op */
	uint32_t fpscr_pad;  /* 80 */
	uint32_t fpscr_val;  /* 84 */
	/* Volatile FP registers */
	uint64_t fpr0;	/* 88 */
	uint64_t fpr1;
	uint64_t fpr2;	/* 104 */
	uint64_t fpr3;
	uint64_t fpr4;	/* 120 */
	uint64_t fpr5;
	uint64_t fpr6;	/* 136 */
	uint64_t fpr7;
	uint64_t fpr8;	/* 152 */
	uint64_t fpr9;
	uint64_t fpr10;	/* 168 */
	uint64_t fpr11;
	uint64_t fpr12;	/* 184 */
	uint64_t fpr13;	/* 192 */

	/* Previous frame.  */
	uint32_t back_chain;	/* 200 */
	uint32_t lr;			/* 204 */

} volatile_context_t;		/* 208 bytes */

uint32_t		pok_context_create(uint32_t id,
						uint32_t stack_size,
						uint32_t entry);

void			pok_context_switch(uint32_t* old_sp,
						uint32_t new_sp);


#endif /* !__POK_PPC_THREAD_H__ */


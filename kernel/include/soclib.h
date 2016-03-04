
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
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *         Maintainers: nipo
 * 
 * Copyright (c) Barcelona Supercomputing Center (BSC)
 *         Leonidas Kosmidi, 2011
 */

#ifndef __POK_SOCLIB_H__
#define __POK_SOCLIB_H__

#include <types.h>

#define get_cp2(x, sel)																					  \
({unsigned int __cp2_x;																			\
__asm__("mfc2 %0, $"#x", "#sel:"=r"(__cp2_x));  \
__cp2_x;})

#define DO_MEMORY_SYNC __asm__ volatile("" ::: "memory")
#define MAX(x,y) ( ( (x) >= (y) ) ? (x) : (y) )
/*Digits after radix point of a double to be printed */
#define FP_DIGITS (10)
#define SIZE_OF_BUF MAX( 1024,(FP_DIGITS + 16))

#define	TTY_BASE	0xC0800000 
#define	TTY_SIZE	0x00000040
#define base(x) (void*)(x##_BASE)


#define NaNdMaskF	(0x0007FFFFFFFFFFFFLLU)
#define SNaNdMaskE	(0x7FF8000000000000LLU)
#define PINFId		(0x7FF0000000000000LLU)
#define NINFId		(0xFFF0000000000000LLU)
#define SignMaskd	(0x8000000000000000LLU)

/* Prototypes for ansi compatibility*/
/*
int Is_QNaNd(uint64_t i64);
int Is_NaNd(uint64_t i64);
int Is_PINFd(uint64_t i64);
int Is_NINFd(uint64_t i64);
int Is_INFd(uint64_t i64);
int Is_Negatived(uint64_t i64);
*/

#define dcr_get(x)					\
({unsigned int __val;				\
__asm__("mfdcr %0, "#x:"=r"(__val));\
__val;})


enum SoclibTtyRegisters {
	TTY_WRITE = 0,
	TTY_STATUS = 1,
	TTY_READ  = 2,
	TTY_SPAN  = 4,
};

void soclib_io_write8(void *comp_base, size_t reg, uint8_t val);

static __inline__ int Is_SNaNd(uint64_t i64)
{
	return ((((i64 & SNaNdMaskE)== 0x7FF0000000000000LLU) && (i64 & NaNdMaskF) ) ? 1 :0);
}

static __inline__ int Is_QNaNd(uint64_t i64)
{
	/*I think that we need also && (i64 & NaNdMaskF)*/
	return ((((i64 & SNaNdMaskE)== 0x7FF8000000000000LLU) ) ? 1 : 0);
}

static __inline__ int Is_NaNd(uint64_t i64)
{
	return ((Is_SNaNd(i64) || Is_QNaNd(i64)) ? 1 : 0);
}

static __inline__ int Is_PINFd(uint64_t i64)
{
	return ((i64 == PINFId) ? 1 : 0 );
}
static __inline__ int Is_NINFd(uint64_t i64)
{
	return ((i64 == NINFId) ? 1 : 0 );
}
static __inline__ int Is_INFd(uint64_t i64)
{
	return ((Is_PINFd(i64) || Is_NINFd(i64)) ? 1 : 0);
}
static __inline__ int Is_Negatived(uint64_t i64)
{
	return ((i64 & SignMaskd) ? 1 : 0 );
}

int putchar(const int x);


static __inline__ void soclib_io_set(void *comp_base, size_t reg, uint32_t val)
{
	volatile uint32_t *addr = (uint32_t *)comp_base;

	reg <<= 2;
	__asm__("stwbrx %0, %1, %2":: "b"(val), "b"(addr), "b"(reg) : "memory" );
}

static  __inline__ void trap(){
	__asm__ volatile("trap"); 
}

#endif // SOCLIB


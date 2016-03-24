
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
 *
 */
#ifdef POK_ARCH_PPC

#include "soclib.h"

__asm__(
".globl putchar				\n\t"
".type  putchar, @function   \n\t"
"putchar: \n\t"
	"stwu    1,-32(1) \n\t"
	"mflr    0 \n\t"
	"stw     0,36(1) \n\t"
	"stw     31,28(1) \n\t"
	"mr      31,1 \n\t"
	"stw     3,8(31) \n\t"
	"bl      procnum \n\t"
	"mr      0,3 \n\t"
	"rlwinm  0,0,2,0,29 \n\t"
	"mr      9,0 \n\t"
	"lwz     0,8(31) \n\t"
	"clrlwi  0,0,24 \n\t"
	"lis     3,-16256 \n\t"
	"mr      4,9 \n\t"
	"mr      5,0 \n\t"
	"bl      soclib_io_write8 \n\t"
	"lwz     0,8(31) \n\t"
	"mr      3,0 \n\t"
	"addi    11,31,32 \n\t"
	"lwz     0,4(11) \n\t"
	"mtlr    0 \n\t"
	"lwz     31,-4(11) \n\t"
	"mr      1,11 \n\t"
	"blr \n\t"

".globl soclib_io_write8				\n\t"
".type  soclib_io_write8, @function   \n\t"
	"soclib_io_write8: \n\t"
	"stwu    1,-48(1) \n\t"
	"stw     31,44(1) \n\t"
	"mr      31,1 \n\t"
	"stw     3,24(31) \n\t"
	"stw     4,28(31) \n\t"
	"mr      0,5 \n\t"
	"stb     0,32(31) \n\t"
	"lwz     0,24(31) \n\t"
	"stw     0,8(31) \n\t"
	"lwz     0,28(31) \n\t"
	"rlwinm  0,0,2,0,29 \n\t"
	"lwz     9,8(31) \n\t"
	"add     0,9,0 \n\t"
	"stw     0,8(31) \n\t"
	"lwz     0,8(31) \n\t"
	"lbz     9,32(31) \n\t"
	"mr      11,0 \n\t"
	"stb     9,0(11) \n\t"
	"addi    11,31,48 \n\t"
	"lwz     31,-4(11) \n\t"
	"mr      1,11 \n\t"
	"blr \n\t"

".globl procnum				\n\t"
".type  procnum, @function   \n\t"
	"procnum: \n\t"
	"stwu    1,-32(1) \n\t"
	"stw     31,28(1) \n\t"
	"mr      31,1 \n\t"
	".long 0x7c000286 \n\t"
	"stw     0,8(31) \n\t"
	"lwz     0,8(31) \n\t"
	"mr      3,0 \n\t"
	"addi    11,31,32 \n\t"
	"lwz     31,-4(11) \n\t"
	"mr      1,11 \n\t"
	"blr \n\t"
);
#else //POK_ARCH_MPPA

#include <types.h>
#include <HAL/hal/club_syscall.h>

int putchar(const int x) {
	unsigned long long int ret;

	/* possibly not needed, made in xputs */
	__asm__ __volatile__ ("make $r20 = 1\n\t;;");
	/* syscall 4094 with two args: char (or string) address and size
	 * Other possible syscall values are 4095 and 1104 */
	ret = __k1_club_syscall2 (4094, (volatile int) &x, 1);
	return (int) ret;
}

#endif

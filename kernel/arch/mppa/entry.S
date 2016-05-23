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

#include "entry.h"
#include "mOS_common_types_s_c.h"
#include "HAL/hal/context.h"

# ax: section is allocatable and executable
# progbits: section contains data
.section .my_boot, "ax", @progbits
.align 8

# Global variables
.global pok_arch_init
.global _data_start

.global _vstart
.proc _vstart
.type _vstart, @function
#****************************************************************
_vstart: # _vstart ()
#**************
# Entry point that starts the boot sequence
#
# Register usage
# $ no particular register usage scheme
#**************
	make $r14, _data_start		# r14 is used at some compilation modes
	;;				# ;; ends instruction bundle
	get	$r5 = $pcr		# Get processor id
	;;
	extfz	$r2, $r5, 15, 11	# between 0 - 15 (PE), 16 (RM) in $r2
	;;
	cb.eqz	$r2, __proceed		# if it's PE0 it jumps to proceed
	;;
	scall	MOS_VC_IDLE1		# otherwise the PE waits the end of
	;;				# the booting process
__proceed:				# Defines stack for each PE in the scoreboard
	make	$r3, BOOT_STACK_SIZE	# Normally BOOT_STACK_SIZE = 0x100
	;;
	ctz	$r3, $r3		# 8 stored in $r3
	;;
	sll	$r2, $r2, $r3		# shift proc ID << $r3
	;;
	add	$r2 = $r2, 8
	;;
	neg	$r2, $r2
	;;
	make	$r12 = _scoreboard_boot_stack_start - 8
	;;
	add	$r12 = $r12, $r2 	# move stack down
	;;
	call	pok_boot		# pok_boot and clear_bss before
	;;
	wpurge
	;;
#ifdef __k1a__
	dpurge
	;;
#endif
	make 	$r0 = -1		# should not endup here!
	;;
	scall 	1
	;;
	hfxb 	$cs, $r1
	;;
	idle1
	;;
	goto 	-8			# should never return
	;;
.endp _vstart

#****************************************************************
# Kernel's stack area
#**************
.section ".bss", "aw"
pok_stack:
	.space 8 * 1024
	.globl pok_stack_end
pok_stack_end:

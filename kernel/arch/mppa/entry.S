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

# ax: section is allocatable and executable
# progbits: section contains data
# .section ".start" ?
.section .my_boot, "ax", @progbits
.align 8

# Global stuff
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

.section .locked_text, "ax", @progbits
.align 8
.global _system_call_ISR
.proc _system_call_ISR
.type _system_call_ISR, @function
#****************************************************************
_system_call_ISR: # _system_call_ISR ()
#**************
# Syscall Interrupt Service Routine. In this case, it is not
# called by an interrupt. Caused by SC instruction (such as in
#  powerpc) but is explicitly called upon an API system call.
#
# Register usage
# $ r0-r7 syscall parameters
#**************
#ifdef __k1a__
	add $r12 = $r12, -40
	;;
#else
	add $r12 = $r12, -56
	;;
#endif
	copy $r40, $r0
	copy $r41, $r1
	copy $r42, $r2
	copy $r43, $r3
	;;
	copy $r36, $r4
	copy $r37, $r5
	copy $r38, $r6
	copy $r39, $r7
	;;
	get $r1, $pcr
	make $r0, _scoreboard_start
	;;
	extfz $r1, $r1, 15,11
	;;
	sll $r1, $r1, 8
	;;
	add $r0, $r0, $r1
	;;
#ifndef __k1a__
	lw $r2 = MOS_VC_REG_SPC[ $r0 ]
	;;
	lw $r3 = MOS_VC_REG_PS[ $r0 ]
	;;
	lw $r4 = MOS_VC_REG_SPS[ $r0 ]
	;;
	lw $r5 = MOS_VC_REG_SSP[ $r0 ]
	;;
	sd 32[$r12] = $r2r3
	;;
	sd 40[$r12] = $r4r5
	;;
#else
	lw $r4 = MOS_VC_REG_SPS[ $r0 ]
	;;
#endif
	extfz $r4, $r4, 4, 4
	;;
	cb.eqz $r4, __no_it_enable
	;;
	scall MOS_VC_ENABLE_IT
	;;
__no_it_enable:
	copy $r0, $r40
	copy $r1, $r41
	copy $r2, $r42
	copy $r3, $r43
	;;
	copy $r4, $r36
	copy $r5, $r37
	copy $r6, $r38
	copy $r7, $r39
	;;
	;;
	get $r8 = $ra
	;;
	copy $r10 = $r7
	sw 16 [$r12] = $r10
	;;
	sw 24 [$r12] = $r14
	;;
	sw 20 [$r12] = $r8
	add $r12 = $r12, -16
	call pok_arch_sc_int		# __user_do_scall
	;;				# int ret = -1; mOS_enable_hw_loop();return ret;
	add $r12 = $r12, 16
	;;
#ifndef __k1a__
	scall MOS_VC_DISABLE_IT
	;;
	ld $r36r37 = 32[$r12]
	;;
	ld $r38r39 = 40[$r12]
	;;
	get $r40, $pcr
	make $r41, _scoreboard_start
	;;
	extfz $r40, $r40, 15,11
	;;
	sll $r40, $r40, 8
	;;
	add $r41, $r41, $r40
	;;
	sw MOS_VC_REG_SPC[ $r41 ] = $r36
	;;
	sw MOS_VC_REG_PS[ $r41 ] = $r37
	;;
	sw MOS_VC_REG_SPS[ $r41 ] = $r38
	;;
	sw MOS_VC_REG_SSP[ $r41 ] = $r39
	;;
#endif
	lw $r8 = 20 [$r12]
	;;
	lw $r14 = 24 [$r12]
	;;
	lw $r10 = 16 [$r12]
	;;
#ifdef __k1a__
	add $r12 = $r12, 40
	;;
#else
	add $r12 = $r12, 56
	;;
#endif
	set $ra = $r8
	;;
	scall MOS_VC_RFE
	;;
	ret
	;;
.endp _system_call_ISR


#****************************************************************
# Kernel's stack area
#**************
.section ".bss", "aw"
pok_stack:
	.space 8 * 1024
	.globl pok_stack_end
pok_stack_end:

pok_save_area:
	.space 4 * 8
	.section ".reset", "ax"

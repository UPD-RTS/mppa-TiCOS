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

#include <HAL/hal/hal.h>
#include <HAL/hal/context.h>
#include <mOS_common_types_s_c.h>

.section .locked_text, "ax", @progbits
.align 8
.global _interval_ISR
.proc _interval_ISR
_interval_ISR:
	add	$r12, $r12, -16     		## create scratch area
	;;
	sd 0[$r12] = $p16;
	make $r16 = pok_current_context		# load memory adress of
	;;					# current context
	lw $r17 = 0[$r16]
	;;
	_vk1_context64_save $r17		# save it
	;;
	ld $p16 = 0[$r12]
	make	$r0, _scoreboard_start
	get	$r3, $pcr
	;;
	extfz	$r1, $r3, 15, 11		# retrieve the current PE in $r1
	;;
	sll	$r1, $r1, 8
	;;
	add	$r2, $r2, $r1
	;;
	lw	$r0 = MOS_VC_REG_PS[ $r2 ]	# retrieve the current $ps in $r0
	copy	$r1, $r12			# retrieve the stack pointer in $r1
	;;
	srl	$r0 = $r0, 28
	add	$r12, $r12, -16			## create nested scratch area
	call	__k1_do_int			## jump to C code
	;;
__vk1_asm_return_from_int:
	add	$r12, $r12, 16			## destroy nested scratch area
	;;
	make $r16 = pok_current_context		# retrieve the elected context
	;;
	lw $r13 = 0[$r16]
	;;
	_vk1_context64_restore $r13		# restore this new context
	;;
	scall	MOS_VC_RFE
	;;
.endp _interval_ISR

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
	add $r12 = $r12, -16			# create scratch area
	;;
	copy $r0, $r7				# reswitch the registers (see syscall.S)
	copy $r7, $r0				# $r0 = SN and $r7 = arg
	;;
	copy $r40, $r0				# save registers $r0-$r7
	copy $r41, $r1				# because _vk1_context64_save
	copy $r42, $r2				# modifies them
	copy $r43, $r3
	;;
	copy $r36, $r4
	copy $r37, $r5
	copy $r38, $r6
	copy $r39, $r7
	;;
	make $r16 = pok_current_context		# retrieves current context
	;;
	lw $r17 = 0[$r16]
	;;
	_vk1_context64_save $r17		#saves it
	;;
	make $r12 = _scoreboard_kstack_start;	# stack switch
	;;
	copy $r0, $r40				# restore registers $r0-$r7
	copy $r1, $r41
	copy $r2, $r42
	copy $r3, $r43
	;;
	copy $r4, $r36
	copy $r5, $r37
	copy $r6, $r38
	copy $r7, $r39
	;;
	add $r12 = $r12, -16			# create nested scratch area
	call pok_arch_sc_int			# jump to C code
	;;
__vk1_asm_return_from_sc:
	add $r12 = $r12, 16			# destroy nested scratch area
	;;
	make $r16 = pok_current_context
	;;
	lw $r13 = 0[$r16]
	;;
	sw 0[$r13] = $r0
	;;
	_vk1_context64_restore $r13		# restore context
	;;
	add $r12 = $r12, 16			# destroy scratch area
	;;
	scall MOS_VC_RFE
	;;
.endp _system_call_ISR

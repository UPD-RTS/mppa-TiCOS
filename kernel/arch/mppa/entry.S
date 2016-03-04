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

#include "entry.h"
#include "mOS_common_types_s_c.h"

# ax: section is allocatable and executable
# progbits: section contains data
# .section ".start" ?
.section ".boot", "ax", @progbits
.align 8

# Global variables
.global __proceed
.global _entry_point
.global _vstart

# vstart function definition
.proc _vstart
.type _vstart, @function
_vstart:
	make	$r14, _data_start
	;;				# ends instruction bundle
	get	$r5 = $pcr
	;;
	extfz	$r2, $r5, 15, 11	# Get processor id
	;;				# between 0 - 15 (PE), 16 (RM) in $r2
	cb.eqz	$r2, __proceed		# if it's PE0 it jumps to proceed
	;;				# otherwise the PE waits the end of
	scall	MOS_VC_IDLE1		# the booting process
	;;
__proceed:
	make $r3, BOOTSTACK_SIZE	# Normally BOOTSTACK_SIZE = 0x100
	;;
	ctz $r3, $r3			# 8 stored in $r3
	;;
	sll $r2, $r2, $r3		# shift proc ID << $r3
	;;
	add $r2 = $r2, 8
	;;
	neg $r2, $r2
	;;
	make $r12 = _scoreboard_boot_stack_start - 8
	;;
	add $r12 = $r12, $r2 # move stack down
	;;
	call _entry_point
	;;
	wpurge
	;;
#ifdef __k1a__
	dpurge
	;;
#endif
	make $r0 = -1 # should not endup here!
	;;
	scall 1
	;;
	hfxb $cs, $r1
	;;
	idle1
	;;
	goto -8 # should never return
	;;
	.endp

_entry_point:
	idle1
	;;

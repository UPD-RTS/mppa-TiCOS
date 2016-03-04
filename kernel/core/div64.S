
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

/********************************************************************************/
/* Divide a 64-bits unsigned number by a 64-bits unsigned number. 				*/
/* Author F.WARTEL :  Dec-09-2011 					 							*/
/********************************************************************************/
#
# C prototype :
# TIME_TYPE DIV64(
#		TIME_TYPE dvd ,
#		TIME_TYPE dvs)
#	
#
#
#
#  (r3:r4) = (r3:r4) / (r5:r6) (64b) = (64b / 64b) 		
#	 quo		 dvd			dvs				 	
#  Remainder is stored in r5:r6, but not returned					
#  Code comment notation:						
# 	 msw = most-significant (high-order) word, i.e. bits 0..31
# 	 lsw = least-significant (low-order) word, i.e. bits 32..63
#	 LZ = Leading Zeroes
#	 SD = Significant Digits				
#  r3:r4 = dvd (input dividend parameter from r3)
#			 quo (returned output quotient to r3,r4)
#  r5:r6 = dvs (input divisor from r4)
#		rem (output remainder, lost)
#  r7:r8 = tmp

 .file	  "DIV64.s"
  .text
  .align	 4
  .globl	 DIV64

DIV64:
	mr 	%r11,%r3
	mr	%r12,%r4
	lwz	%r3, 0(%r11)
	lwz	%r4, 4(%r11)
	lwz	%r5, 0(%r12)
	lwz	%r6, 4(%r12)


	# count the number of leading 0s in the dividend
	cmpwi 	%cr0,%r3,0 	# dvd.msw == 0?
 	cntlzw 	%r0,%r3 	# r0 = dvd.msw.LZ
	cntlzw 	%r9,%r4		# r9 = dvd.lsw.LZ
	bne 	%cr0,1f 	# if(dvd.msw == 0) dvd.LZ = dvd.msw.LZ
	addi 	%r0,%r9,32 	# dvd.LZ = dvd.lsw.LZ + 32

	# count the number of leading 0s in the divisor
1:	cmpwi 	%cr0,%r5,0	# dvd.msw == 0?
	cntlzw 	%r9,%r5 	# %r9 = dvs.msw.LZ
	cntlzw 	%r10,%r6 	# %r10 = dvs.lsw.LZ
	bne 	%cr0,2f 	# if(dvs.msw == 0) dvs.LZ = dvs.msw.LZ
	addi 	%r9,%r10,32 # dvs.LZ = dvs.lsw.LZ + 32
	
	# determine shift amounts to minimize the number of iterations
2:	cmpw 	%cr0,%r0,%r9 # compare dvd.LZ to dvs.LZ
	subfic 	%r10,%r0,64 # %r10 = dvd.SD
	bgt 	%cr0,9f 	# if(dvs > dvd) quotient = 0
	addi 	%r9,%r9,1 	# ++dvs.LZ (or --dvs.SD)
	subfic 	%r9,%r9,64 	# %r9 = dvs.SD
	add	%r0,%r0,%r9 	# (dvd.LZ + dvs.SD) = left shift of dvd for
	# initial dvd
	subf 	%r9,%r9,%r10 # (dvd.SD - dvs.SD) = right shift of dvd for
	# initial tmp
	mtctr 	%r9 		# number of iterations = dvd.SD - dvs.SD
	# %r7:%r8 = %r3:%r4 >> %r9
	cmpwi 	%cr0,%r9,32 # compare %r9 to 32
	addi	%r7,%r9,-32
	blt 	%cr0,3f		# if(%r9 < 32) jump to lab3
	srw 	%r8,%r3,%r7 # tmp.lsw = dvd.msw >> (%r9 - 32)
	li	%r7,0			# tmp.msw = 0
	b 	4f
3:	srw 	%r8,%r4,%r9	# %r8 = dvd.lsw >> %r9
	subfic  %r7,%r9,32
	slw 	%r7,%r3,%r7 # %r7 = dvd.msw << 32 - %r9
	or 	%r8,%r8,%r7 	# tmp.lsw = %r8 | %r7
	srw 	%r7,%r3,%r9 # tmp.msw = dvd.msw >> %r9

4:	# %r3:%r4 = %r3:%r4 << %r0
	cmpwi 	%cr0,%r0,32 # compare %r0 to 32
	addic 	%r9,%r0,-32
	blt 	%cr0,5f 	# if(%r0 < 32) jump to lab5
	slw 	%r3,%r4,%r9 # dvd.msw = dvd.lsw << %r9
	li 	%r4,0 			# dvd.lsw = 0
	b 	6f

5:	slw 	%r3,%r3,%r0 # %r3 = dvd.msw << %r0
	subfic 	%r9,%r0,32
	srw 	%r9,%r4,%r9 # %r9 = dvd.lsw >> 32 - %r0
	or 	%r3,%r3,%r9		# dvd.msw = %r3 | %r9
	slw 	%r4,%r4,%r0 # dvd.lsw = dvd.lsw << %r0

	# restoring division shift and subtract loop
6:	li 	%r10,-1 		# %r10 = -1
	addic 	%r7,%r7,0 	# clear carry bit before loop starts
7:
	# tmp:dvd is considered one large register
	# each portion is shifted left 1 bit by adding it to itself
	# adde sums the carry from the previous and creates a new carry
	adde 	%r4,%r4,%r4 # shift dvd.lsw left 1 bit
	adde 	%r3,%r3,%r3 # shift dvd.msw to left 1 bit
	adde 	%r8,%r8,%r8 # shift tmp.lsw to left 1 bit
	adde 	%r7,%r7,%r7 # shift tmp.msw to left 1 bit
	subfc 	%r0,%r6,%r8 # tmp.lsw - dvs.lsw
	subfe. 	%r9,%r5,%r7 # tmp.msw - dvs.msw
	blt 	%cr0,8f 	# if(result < 0) clear carry bit
	mr 	%r8,%r0 		# move lsw
	mr 	%r7,%r9 		# move msw
	addic	%r0,%r10,1 	# set carry bit
8:	bdnz 	7b
	
	# write quotient and remainder
	adde 	%r4,%r4,%r4 # quo.lsw (lsb = CA)
	adde 	%r3,%r3,%r3	# quo.msw (lsb from lsw)
	mr 	%r6,%r8 		# rem.lsw
	mr 	%r5,%r7 		# rem.msw
	b	10f				# return

9:	#Quotient is 0 (dvs > dvd)
	mr 	%r6,%r4 		# rmd.lsw = dvd.lsw
	mr	%r5,%r3 		# rmd.msw = dvd.msw
	li 	%r4,0 			# dvd.lsw = 0
	li 	%r3,0 			# dvd.msw = 0

10:	blr 				# return

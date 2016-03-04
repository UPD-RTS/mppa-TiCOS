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

/** Patches for cswitch.S
 *  [1] pok_context_switch changed to accomodate a larger context augmented with FP registers;
 *		Include volatile FP registers form FPR14 to FPR31 (64bit each) plus FPSCR.
 *		The thread context has been modified accordingly (see kernel/arch/ppc/thread.h).
 */
		.text
		.globl pok_context_switch
pok_context_switch:
		/* r3: *old_sp, r4: new_sp */

		/* Save registers.  */
		mflr	 %r0
		stw	  %r0,4(%r1)
		/* stwu	 %r1,-96(%r1) */
		stwu	 %r1,-256(%r1)
		mfcr	 %r0
		stw	  %r0,8(%r1)	 /* cr  */
		stw	  %r2,12(%r1)	/* r2  */
		stw	  %r13,16(%r1)  /* r13 */
		stw	  %r14,20(%r1)
		stw	  %r15,24(%r1)
		stw	  %r16,28(%r1)
		stw	  %r17,32(%r1)
		stw	  %r18,36(%r1)
		stw	  %r19,40(%r1)
		stw	  %r20,44(%r1)
		stw	  %r21,48(%r1)
		stw	  %r22,52(%r1)
		stw	  %r23,56(%r1)
		stw	  %r24,60(%r1)
		stw	  %r25,64(%r1)
		stw	  %r26,68(%r1)
		stw	  %r27,72(%r1)
		stw	  %r28,76(%r1)
		stw	  %r29,80(%r1)
		stw	  %r30,84(%r1)
		stw	  %r31,88(%r1)  /* r31 */
		/* Assuming FPSCR need to be saved (not sure about it) */
		mffs	 %f14;
		stfs	 %f14,96(%r1)  /* fpscr_pad + fpscr_val */ 
		stfs	 %f14,104(%r1) /* fpr14 */ 
		stfs	 %f15,112(%r1) /* fpr15 */
		stfs	 %f16,120(%r1) /* fpr16 */
		stfs	 %f17,128(%r1) /* fpr17 */
		stfs	 %f18,136(%r1) /* fpr18 */
		stfs	 %f19,144(%r1) /* fpr19 */
		stfs	 %f20,152(%r1) /* fpr20 */
		stfs	 %f21,160(%r1) /* fpr21 */ 
		stfs	 %f22,168(%r1) /* fpr22 */
		stfs	 %f23,176(%r1) /* fpr23 */
		stfs	 %f24,184(%r1) /* fpr24 */
		stfs	 %f25,192(%r1) /* fpr25 */
		stfs	 %f26,200(%r1) /* fpr26 */
		stfs	 %f27,208(%r1) /* fpr27 */
		stfs	 %f28,216(%r1) /* fpr28 */
		stfs	 %f29,224(%r1) /* fpr29 */
		stfs	 %f30,232(%r1) /* fpr30 */
		stfs	 %f31,240(%r1) /* fpr31 */

		/* Switch stack. */
		stw	  %r1,0(%r3)
		mr		%r1,%r4

		/* Restore registers.						*/
		/* Assuming FPSCR need to be restored		*/
		lfs	  %f14,96(%r1) /* fpscr_pad + fpscr_val */ 
		mtfsf	0xff, %f14				  
		lfs	  %f14,104(%r1) /* fpr14 */ 
		lfs	  %f15,112(%r1) /* fpr15 */
		lfs	  %f16,120(%r1) /* fpr16 */
		lfs	  %f17,128(%r1) /* fpr17 */
		lfs	  %f18,136(%r1) /* fpr18 */
		lfs	  %f19,144(%r1) /* fpr19 */
		lfs	  %f20,152(%r1) /* fpr20 */
		lfs	  %f21,160(%r1) /* fpr21 */ 
		lfs	  %f22,168(%r1) /* fpr22 */
		lfs	  %f23,176(%r1) /* fpr23 */
		lfs	  %f24,184(%r1) /* fpr24 */
		lfs	  %f25,192(%r1) /* fpr25 */
		lfs	  %f26,200(%r1) /* fpr26 */
		lfs	  %f27,208(%r1) /* fpr27 */
		lfs	  %f28,216(%r1) /* fpr28 */
		lfs	  %f29,224(%r1) /* fpr29 */
		lfs	  %f30,232(%r1) /* fpr30 */
		lfs	  %f31,240(%r1) /* fpr31 */

		lwz	  %r31,88(%r1) 
		lwz	  %r30,84(%r1)
		lwz	  %r29,80(%r1)
		lwz	  %r28,76(%r1)
		lwz	  %r27,72(%r1)
		lwz	  %r26,68(%r1)
		lwz	  %r25,64(%r1)
		lwz	  %r24,60(%r1)
		lwz	  %r23,56(%r1)
		lwz	  %r22,52(%r1)
		lwz	  %r21,48(%r1)
		lwz	  %r20,44(%r1)
		lwz	  %r19,40(%r1)
		lwz	  %r18,36(%r1)
		lwz	  %r17,32(%r1)
		lwz	  %r16,28(%r1)
		lwz	  %r15,24(%r1)
		lwz	  %r14,20(%r1)
		lwz	  %r13,16(%r1)
		lwz	  %r2,12(%r1)
		lwz	  %r0,8(%r1)
		mtcr	 %r0
		lwz	  %r1,0(%r1)
		lwz	  %r0,4(%r1)
		mtlr	 %r0
		blr

		.size pok_context_switch, . - pok_context_switch

		.globl pok_arch_thread_start
pok_arch_thread_start:
		mr		%r3, %r14	 /* IDLE thread entry */
		mr		%r4, %r15	 /* IDLE thread ID	 */

		xor	  %r0,%r0,%r0
		stw	  %r0,4(%r1)	/* Invalidates lr	 */
		bl		pok_thread_start_execution

		.size pol_arch_thread_start, . - pok_arch_thread_start

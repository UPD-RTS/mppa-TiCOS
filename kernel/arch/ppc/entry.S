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

#include "msr.h"
#include "hid0.h"
/** Patches for entry.S
 *  [1] Modify the exception handling code for unrelevant/unsupported exceptions so
 *		that so that some informations are saved before entering an infinite loop in
 *		reserved_0000, reserved_2F00;
 *  [2] Changed FRAME_SIZE and SAVE_REGS to accomodate a larger context augmented with FP registers;
 *		Include non-volatile FP registers form FPR0 to FPR13 (64bit each) plus FPSCR  
 *		To this extent we need to grow the FRAME_SIZE to 80+(14*8)+8=200 
 *		and modify the thread context accordingly (see kernel/arch/ppc/thread.h).
 */

		  .section ".start", "ax"
/* #define FRAME_SIZE 80 */
#define FRAME_SIZE 200
		/*	r4 is available, r1 is set, r3 contains cr,
			sprg3 contains r3 and sprg2 contains r2.  */
#define SAVE_REGS										  \
		/** Set the FP bit in MSR  */		  \
		mfmsr	%r4;									  \
		ori	  %r4, %r4, MSR_FP;					 \
		mtmsr	%r4;									  \
		/* Establish new frame.  */				  \
		mflr	 %r4;									  \
		stw	  %r4,FRAME_SIZE+4(%r1);  /* lr */ \
		mfctr	%r4;									  \
		stw	  %r4,64(%r1);  /* ctr */			 \
		mfxer	%r4;									  \
		stw	  %r4,68(%r1);  /* xer */			 \
		stw	  %r3,8(%r1);	/* cr */			  \
		stw	  %r0,12(%r1);  /* r0 */			  \
		stw	  %r2,16(%r1);  /* r2 */			  \
		mfsprg  %r0,3;									\
		stw	  %r0,20(%r1);  /* r3 */			  \
		mfsprg  %r0,2;									\
		stw	  %r0,24(%r1);  /* r4 */			  \
		stw	  %r5,28(%r1);  /* r5 */			  \
		stw	  %r6,32(%r1);  /* r6 */			  \
		stw	  %r7,36(%r1);  /* r7 */			  \
		stw	  %r8,40(%r1);  /* r8 */			  \
		stw	  %r9,44(%r1);  /* r9 */			  \
		stw	  %r10,48(%r1); /* r10 */			 \
		stw	  %r11,52(%r1); /* r11 */			 \
		stw	  %r12,56(%r1); /* r12 */			 \
		stw	  %r13,60(%r1); /* r13 */			 \
		mfsrr0  %r3;									  \
		stw	  %r3,72(%r1);  /* srr0 */			\
		mfsrr1  %r4;									  \
		stw	  %r4,76(%r1);  /* srr1 */			\
		/* Include FP registers form FPR0 to FPR13 plus FPSCR  */ \
		stfs	 %f0, 88(%r1);  /* fpr0 */ \
		stfs	 %f1, 96(%r1);  /* fpr1 */ \
		stfs	 %f2, 104(%r1); /* fpr2 */ \
		stfs	 %f3, 112(%r1); /* fpr3 */ \
		stfs	 %f4, 120(%r1); /* fpr4 */ \
		stfs	 %f5, 128(%r1); /* fpr5 */ \
		stfs	 %f6, 136(%r1); /* fpr6 */ \
		stfs	 %f7, 144(%r1); /* fpr7 */  \
		stfs	 %f8, 152(%r1); /* fpr8 */ \
		stfs	 %f9, 160(%r1); /* fpr9 */ \
		stfs	 %f10,168(%r1); /* fpr10 */ \
		stfs	 %f11,176(%r1); /* fpr11 */ \
		stfs	 %f12,184(%r1); /* fpr12 */ \
		stfs	 %f13,192(%r1); /* fpr13 */ \
		/* Assuming FPSCR need to be saved (not sure about it) */ \
		mffs	 %f13;												 \
		stfs	 %f13,80(%r1)  /* fpscr_pad + fpscr_val */ \

base = .
		  
reserved_0000:
		/* Reserved for implementation specific exceptions						*/
		/* Modified to save info on SRR0 and SRR1									*/
		/* fails */
		/* b failed */
		/* Set %r4 to ctr to save information in case of fault */
		mfctr	%r4
		bl failed

		/* Restore registers */
		.globl pok_arch_rfi
pok_arch_rfi:
		lwz	  %r13,60(%r1)
		lwz	  %r12,56(%r1)
		lwz	  %r11,52(%r1)
		lwz	  %r10,48(%r1)
		lwz	  %r9,44(%r1)
		lwz	  %r8,40(%r1)
		lwz	  %r7,36(%r1)
		lwz	  %r6,32(%r1)
		lwz	  %r5,28(%r1)
		lwz	  %r4,24(%r1)
		lwz	  %r3,20(%r1)
		lwz	  %r2,16(%r1)
		/* Assuming FPSCR need to be restored (not sure about it) */
		lfs	 %f0, 80(%r1)  /* fpscr_pad + fpscr_val */
		mtfsf  0xff, %f0;												 
		lfs	 %f0, 88(%r1);  /* fpr0 */ 
		lfs	 %f1, 96(%r1);  /* fpr1 */
		lfs	 %f2, 104(%r1); /* fpr2 */
		lfs	 %f3, 112(%r1); /* fpr3 */
		lfs	 %f4, 120(%r1); /* fpr4 */
		lfs	 %f5, 128(%r1); /* fpr5 */
		lfs	 %f6, 136(%r1); /* fpr6 */
		lfs	 %f7, 144(%r1); /* fpr7 */ 
		lfs	 %f8, 152(%r1); /* fpr8 */
		lfs	 %f9, 160(%r1); /* fpr9 */
		lfs	 %f10,168(%r1); /* fpr10 */
		lfs	 %f11,176(%r1); /* fpr11 */
		lfs	 %f12,184(%r1); /* fpr12 */
		lfs	 %f13,192(%r1); /* fpr13 */

		lwz	  %r0,204(%r1) 
		mtlr	 %r0
		lwz	  %r0,64(%r1)
		mtctr	%r0
		lwz	  %r0,68(%r1)
		mtxer	%r0
		lwz	  %r0,72(%r1)
		mtsrr0  %r0
		lwz	  %r0,76(%r1)
		mtsrr1  %r0
		andi.	%r0,%r0,MSR_PR
		beq	  1f
		addi	 %r0,%r1,FRAME_SIZE
		mtsprg  1, %r0
1:		lwz	  %r0,8(%r1)
		mtcr	 %r0
		lwz	  %r0,12(%r1)
		lwz	  %r1,0(%r1)
		rfi

		/* Common label used in POK for intercepting illegal execution privileges */
		/* Modified to save info on SRR0 and SRR1											*/
failed: 
		/* b failed */
		/* %r4 holds the EA of the instruction that branched here */
		/* Save %r4 to SRR0 */
		/* Save MSR to SRR1 */
		mtsrr0  %r4;
		mfmsr	%r3;
		mtsrr1  %r3;
		li	%r3,-1; 
		b 	exit;
//		b reserved_2F00;

		. = base + 0x100
		.globl _pok_reset
system_reset_0100:
_pok_reset:
		/* Clear BAT */
		xor %r0, %r0, %r0;

		/* Just clear the Vs and Vp bits in the upper BAT registers */
		mtspr 528, %r0;
		mtspr 530, %r0; 
		mtspr 532, %r0;
		mtspr 534, %r0;
		mtspr 536, %r0;
		mtspr 538, %r0;
		mtspr 540, %r0;
		mtspr 542, %r0;

		/* Invalidate all segment registers */
		/*	  Temporary solution a long as only sr1 is currently used */
		/* %r0 is zero */
		//isync
		mtsr 0, %r0;
		mtsr 1, %r0;
		mtsr 2, %r0;
		mtsr 3, %r0;
		mtsr 4, %r0;
		mtsr 5, %r0;
		mtsr 6, %r0;
		mtsr 7, %r0;
		mtsr 8, %r0;
		mtsr 9, %r0;
		mtsr 10,%r0;
		mtsr 11, %r0;
		mtsr 12, %r0;
		mtsr 13, %r0;
		mtsr 14, %r0;
		mtsr 15, %r0;
		//isync

		/* Init HID0 register to enable Icache and Dcache */
		/* Enabling and invalidating both */
		xor %r1, %r1, %r1;
		ori %r1, %r1, HID0_ICE; /* set ICE */
		ori %r1, %r1, HID0_ICFI; /* set ICFI */
		//isync;
		mtspr HID0, %r1; /* Enable ICache and Invalidate Icache */
		//isync
		ori %r1, %r1, HID0_DCE; /* set DCE */
		ori %r1, %r1, HID0_DCFI; /* set DCFI */
		//isync
		mtspr HID0, %r1; /* Enable Dcache and Invalidate Dcache */
		//isync

		/* Setup stack */
		lis %r1,(pok_stack_end-8)@h		// ERROR FIXED: stack alignment needs to be 8 bytes
		ori %r1,%r1,(pok_stack_end-8)@l

		/* Clear back chain */
		xor %r0,%r0,%r0
		stw %r0,4(%r1)

		mtmsr %r0
		li    %r3, MSR_IP
		mtmsr %r3

		/* Copy .data */
		b copy_data

end_copy_data:

		/* clear .bss */
		b clear_bss

end_clear_bss:
	
		/* Call C function */
		bl pok_boot
1:		b  1b

		. = base + 0x200
machine_check_0200:
		/* fails */
		/* SRRO <- EA of some instruction that was executing of about to be	*/
		/*			In QEMU implementation it holds the NIP					*/
		/* SRR1 <- MSR[RI] if recoverable, imp-dependent otherwise			*/
		b reserved_2F00

		. = base + 0x300
dsi_0300:
		mtsprg 3,%r3
		mtsprg 2,%r4
		mfcr   %r3
		mfsrr1 %r4
		andi.  %r4,%r4,MSR_PR
		/* DSI musn't appear in supervisor mode.  				*/
		/* Set %r4 to ctr to save information in case of fault	*/
		mfctr  %r4
		beq    failed
		/* DSI musn't appear in supervisor mode.  				*/

		/* Switch to kernel stack.  							*/
		mr     %r4,%r1
		mfsprg %r1,1
		stwu   %r4,-FRAME_SIZE(%r1)

		SAVE_REGS
		mfdar   %r3
		mfdsisr %r4

		bl pok_arch_dsi_int
		b  pok_arch_rfi

		. = base + 0x400
isi_0400:
		mtsprg 3,%r3
		mtsprg 2,%r4
		mfcr   %r3
		mfsrr1 %r4
		andi.  %r4,%r4,MSR_PR
		/* Set %r4 to ctr to save information in case of fault	*/
		mfctr  %r4
		beq    failed 
		/* ISI musn't appear in supervisor mode.  				*/
		/* beql	  failed										*/
		/* ISI musn't appear in supervisor mode.				*/

		/* Switch to kernel stack.								*/
		mr     %r4,%r1
		mfsprg %r1,1
		stwu   %r4,-FRAME_SIZE(%r1)

		SAVE_REGS

		bl pok_arch_isi_int
		b  pok_arch_rfi

		. = base + 0x500
ext_interrupt_0500:
		/* SRRO <- EA of the instruction that would have been executed next */
		/*			In QEMU implementation it holds the NIP-4				*/
		/* SRR1 <- MSR[16-31]												*/
		b reserved_2F00
		. = base + 0x600
alignment_0600:
		/* SRRO <- EA of the instruction that caused the exception			*/
		/*			In QEMU implementation it holds the NIP					*/
		/* SRR1 <- MSR[16-31]												*/
		/* DSISR<- instruction encoding									!Q 	*/
		/* DAR  <- EA of data access computed by the faulty instruction !Q 	*/
		b reserved_2F00

		. = base + 0x700
program_0700:
		/* SRRO <- (a) EA of the instruction that caused the exception	*/
		/*			(b) of the instruction that would have been executed next*/
		/*			In QEMU implementation it holds the NIP				*/
		/* SRR1 <- [11] floating-point enabled exception				*/
		/*			[12] illegal instruction exception					*/
		/*			[13] privileged instruction exception				*/
		/*			[14] trap exception									*/
		/*			[15] SRR0 contains next instruction (a)				*/
		/*			MSR[16-31]											*/
		b reserved_2F00

		. = base + 0x800
fp_unavailable_0800:
		/* Raised on an attempt to execute a fp instruction when MSR[FP]=0  */ 
		/* SRRO <- (a) EA of the instruction that caused the exception		*/
		/*			In QEMU implementation it holds the NIP						*/
		/* SRR1 <- MSR[16-31]															  */
		b reserved_2F00

		. = base + 0x900
decrementer_0900:
		/* PC saved in srr0,
		MSR saved in srr1.  */
		/* Can use sprg.  */
		mtsprg 3,%r3
		mtsprg 2,%r4
		mfcr   %r3
		mfsrr1 %r4
		andi.  %r4,%r4,MSR_PR
		beq    1f  
		/* Coming from user land, switch to system stack */
		mr     %r4,%r1
		mfsprg %r1,1
		stwu   %r4,-FRAME_SIZE(%r1)
		b      2f
		/* Coming from super , continue on kernel stack */
1:		stwu   %r1,-FRAME_SIZE(%r1)
2:		SAVE_REGS
		bl pok_arch_decr_int
		b  pok_arch_rfi

        . = base + 0xa00
reserved_0A00:
		/* Reserved for implementation specific exceptions					  */
		b reserved_2F00

		. = base + 0xb00
reserved_0B00:
		/* Reserved for implementation specific exceptions					  */
		b reserved_2F00

		. = base + 0xc00
system_call_0C00:
		mtsprg 3,%r3
		mtsprg 2,%r4
		mfcr   %r3
		mfsrr1 %r4
		andi.  %r4,%r4,MSR_PR
		/* Set %r4 to ctr to save information in case of fault */
		mfctr  %r4
		beq    failed
		/* SC musn't appear in supervisor mode.  */
		/* beql	  failed */
		/* SC musn't appear in supervisor mode.  */

		/* Switch to kernel stack.  */
		mr     %r4,%r1
		mfsprg %r1,1
		stwu   %r4,-FRAME_SIZE(%r1)

		SAVE_REGS
		lwz   %r3,20(%r1)
		lwz   %r4,24(%r1)
		lwz   %r5,28(%r1)
		lwz   %r6,32(%r1)
		lwz   %r7,36(%r1)
		lwz	  %r8,40(%r1)

		/* Enable interrupt during syscalls.  */
		/* mfmsr	%r0
		ori	  %r0,%r0,MSR_EE
		mtmsr	%r0 */

		bl  pok_arch_sc_int
		stw	%r3,20(%r1)

		b   pok_arch_rfi

		. = base + 0xd00
trace_0D00:
		/* SRRO <- EA of the instruction that would have been executed next */
		/*			In QEMU implementation it holds the NIP-4					 */
		/* SRR1 <- MSR[16-31]															  */
		b reserved_2F00
		. = base + 0xe00

reserved_0E00:
		/* Should be Floating_point Assist exception  (optional)				*/
		b reserved_2F00
		. = base + 0xf00

reserved_0F00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1000:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1100:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1200:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1300:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1400:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1500:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1600:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1700:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1800:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1900:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1A00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1B00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1C00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1D00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1E00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_1F00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2000:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2100:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2200:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2300:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2400:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2500:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2600:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2700:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2800:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2900:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2A00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2B00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2C00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2D00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00 
		.p2align 8

reserved_2E00:
		/* Reserved for implementation specific exceptions						*/
		b reserved_2F00
		.p2align 8

reserved_2F00:
		/* set a breakpoint to the following label to intercept all unhandled exceptions */

unhandled_exception:
		/* Reserved for implementation specific exceptions						*/
		bl failed
		.p2align 8

reserved_end:


clear_bss:

		/* clear .sbss */
		xor %r0,%r0,%r0
		lis %r3,(__sbss_start-4)@h
		ori %r3,%r3,(__sbss_start-4)@l
		lis %r5,__sbss_end@h
		ori %r5,%r5,__sbss_end@l
		cmplw %r3,%r5
		bge 2f
1:		stwu %r0,4(%r3)
		cmplw %r3,%r5
		blt 1b
2:

		/* clear .sbss2 */
		xor %r0,%r0,%r0
		lis %r3,__sbss2_start@h
		ori %r3,%r3,__sbss2_start@l
		lis %r5,__sbss2_end@h
		ori %r5,%r5,__sbss2_end@l
		cmplw %r3,%r5 /* check if sbss2 section has dimension not null*/
		bge 2f

		lis %r3,(__sbss2_start-4)@h
		ori %r3,%r3,(__sbss2_start-4)@l
		cmplw %r3,%r5
		bge 2f
1:		stwu %r0,4(%r3)
		cmplw %r3,%r5
		blt 1b
2:
		/* clear .bss */
		lis %r3,(__bss_start-4)@h
		ori %r3,%r3,(__bss_start-4)@l
		lis %r5,__bss_end@h
		ori %r5,%r5,__bss_end@l
		cmplw %r3,%r5
		bge 2f
1:		stwu %r0,4(%r3)
		cmplw %r3,%r5
		blt 1b
2:
		b end_clear_bss

copy_data:
		/* Copy .data */
		lis %r3,(__data_start-4)@h
		ori %r3,%r3,(__data_start-4)@l
		lis %r4,(__data_load-4)@h
		ori %r4,%r4,(__data_load-4)@l
		lis %r5,__data_end@h
		ori %r5,%r5,__data_end@l
		cmplw %r3,%r5
		bge 2f
1:		lwzu %r6,4(%r4)
		stwu %r6,4(%r3)
		cmplw %r3,%r5
		blt 1b
2:
	 	/* copy .sdata2 */
		lis %r3,__sdata2_start@h
		ori %r3,%r3,__sdata2_start@l
		lis %r5,__sdata2_end@h
		ori %r5,%r5,__sdata2_end@l
		cmplw %r3,%r5 /* check if sdata2 section has dimension not null*/
		beq 2f

		lis %r3,(__sdata2_start-4)@h
		ori %r3,%r3,(__sdata2_start-4)@l
		lis %r4,(__sdata2_load-4)@h
		ori %r4,%r4,(__sdata2_load-4)@l
		cmplw %r3,%r5
		bge 2f
1:		lwzu %r6,4(%r4)
		stwu %r6,4(%r3)
		cmplw %r3,%r5
		blt 1b
2:
		b end_copy_data
		.section ".bss", "aw"

pok_stack:
		.space 8 * 1024

pok_stack_end:

pok_save_area:
		.space 4 * 8
		.section ".reset", "ax"

reset_FFFC:
		b system_reset_0100

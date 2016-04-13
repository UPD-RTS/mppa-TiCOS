#
# Copyright (C) 2009-2016 Kalray SA.
#
# All rights reserved.
#

#include <HAL/hal/hal.h>
#include "HAL/hal/context.h"
#include "mOS_common_types_s_c.h"

# Helper macro to save a full context including mOS virtual sfr registers

.section .locked_text, "ax", @progbits
.align 8
.global context_save
.proc context_save
.type context_save, @function
#****************************************************************
context_save: # context_save (__k1_uint32_t register)
#**************
# Helper function to save a full context including mOS virtual
# sfr registers
#
# Register usage
# $ r13 - where to save the context
#**************
	sd _K1_CONTEXT_P0[$r13], $p0        ## $r13 - $r1
	;;
	sd _K1_CONTEXT_P2[$r13], $p2        ## $r2 - $r3
	;;
	get $r1, $pcr
	make $r0, _scoreboard_start
	;;
	extfz $r2, $r1, 15, 11
	;;
	sll $r2, $r2, 8
	;;
	add $r0, $r0, $r2
	;;
	sd _K1_CONTEXT_P4[$r13], $p4       ## $r4 - $r5
	;;
	sd _K1_CONTEXT_P6[$r13], $p6       ## $r6 - $r7
	get $r4 = $ra
	;;
	sd _K1_CONTEXT_P8[$r13], $p8       ## $r8 - $r9
	get $r5 = $cs
	;;
	sd _K1_CONTEXT_P10[$r13], $p10      ## $r10 - $r11
	;;
	sd _K1_CONTEXT_P12[$r13], $p12      ## $r12 - $r13
	get $r7 = $lc
	;;
	sd _K1_CONTEXT_P14[$r13], $p14      ## $r14 - $r15
	get $r8 = $ls
	;;
	sd _K1_CONTEXT_P16[$r13], $p16      ## $r16 - $r17
	get $r9 = $le
	;;
	sd _K1_CONTEXT_P18[$r13], $p18      ## $r18 - $r19
	;;
	sd _K1_CONTEXT_P20[$r13], $p20      ## $r20 - $r21
	;;
	sd _K1_CONTEXT_P22[$r13], $p22      ## $r22 - $r23
	;;
	lw $r20 = MOS_VC_REG_SSP[$r0]
	;;
	sd _K1_CONTEXT_P24[$r13], $p24      ## $r24 - $r25
	;;
	sd _K1_CONTEXT_P26[$r13], $p26      ## $r26 - $r27
	;;
	sd _K1_CONTEXT_P28[$r13], $p28      ## $r28 - $r29
	;;
	sd _K1_CONTEXT_P30[$r13], $p30      ## $r30 - $r31
	;;
	sd _K1_CONTEXT_P32[$r13], $p32      ## $r32 - $r33
	;;
	sd _K1_CONTEXT_P34[$r13], $p34      ## $r34 - $r35
	;;
	sd _K1_CONTEXT_P36[$r13], $p36      ## $r36 - $r37
	;;
	sd _K1_CONTEXT_P38[$r13], $p38      ## $r38 - $r39
	;;
	sd _K1_CONTEXT_P40[$r13], $p40      ## $r40 - $r41
	;;
	sd _K1_CONTEXT_P42[$r13], $p42      ## $r42 - $r43
	;;
	sd _K1_CONTEXT_P44[$r13], $p44      ## $r44 - $r45
	;;
	sd _K1_CONTEXT_P46[$r13], $p46      ## $r46 - $r47
	;;
	sd _K1_CONTEXT_P48[$r13], $p48      ## $r48 - $r49
	;;
	sd _K1_CONTEXT_P50[$r13], $p50      ## $r50 - $r51
	;;
	sd _K1_CONTEXT_P52[$r13], $p52      ## $r52 - $r53
	;;
	sd _K1_CONTEXT_P54[$r13], $p54      ## $r54 - $r55
	;;
	sd _K1_CONTEXT_P56[$r13], $p56      ## $r56 - $r57
	;;
	sd _K1_CONTEXT_P58[$r13], $p58      ## $r58 - $r59
	;;
	sd _K1_CONTEXT_P60[$r13], $p60      ## $r60 - $r61
	;;
	sd _K1_CONTEXT_P62[$r13], $p62      ## $r62 - $r63
	;;
	sw _K1_CONTEXT_SSP[$r13], $r20
	;;
	sd _K1_CONTEXT_RA_CS[$r13], $p4     ## $ra - $cs
	;;
	sd _K1_CONTEXT_PS_LC[$r13], $p6     ## $ps - $lc
	;;
	sd _K1_CONTEXT_LS_LE[$r13], $p8     ## $ls - $le
	;;
	ret
	;;
.endp context_save

.section .locked_text, "ax", @progbits
.align 8
.global context_restore
.proc context_restore
.type context_restore, @function
#****************************************************************
context_restore: # context_restore (__k1_uint32_t register)
#**************
# Helper function to restore a full context including mOS virtual
# sfr registers
#
# Register usage
# $ r13 - from where restore the context
#**************
	ld $p8 = _K1_CONTEXT_LS_LE[$r13]   ## $ls - $le
	;;
	ld $p6 = _K1_CONTEXT_PS_LC[$r13]   ## $ps - $lc
	set $ls = $r8
	;;
	ld $p4 = _K1_CONTEXT_RA_CS[$r13]   ## $ra - $cs
	set $le = $r9
	;;
	make $r40, _scoreboard_start
	get $r41, $pcr
	;;
	extfz $r62, $r41, 15, 11
	;;
	sll $r62, $r62, 8
	;;
	add $r40, $r40, $r62
	;;
	set $lc = $r7
	;;
	lw $r20 = _K1_CONTEXT_SSP[$r13]
	;;
	ld $p62 = _K1_CONTEXT_P62[$r13]
	set $cs = $r5
	;;
	sw MOS_VC_REG_SSP[$r40], $r20
	;;
	ld $p60 = _K1_CONTEXT_P60[$r13]
	set $ra = $r4
	;;
	ld $p58 = _K1_CONTEXT_P58[$r13]
	;;
	ld $p56 = _K1_CONTEXT_P56[$r13]
	;;
	ld $p54 = _K1_CONTEXT_P54[$r13]
	;;
	ld $p52 = _K1_CONTEXT_P52[$r13]
	;;
	ld $p50 = _K1_CONTEXT_P50[$r13]
	;;
	ld $p48 = _K1_CONTEXT_P48[$r13]
	;;
	ld $p46 = _K1_CONTEXT_P46[$r13]
	;;
	ld $p44 = _K1_CONTEXT_P44[$r13]
	;;
	ld $p42 = _K1_CONTEXT_P42[$r13]
	;;
	ld $p40 = _K1_CONTEXT_P40[$r13]
	;;
	ld $p38 = _K1_CONTEXT_P38[$r13]
	;;
	ld $p36 = _K1_CONTEXT_P36[$r13]
	;;
	ld $p34 = _K1_CONTEXT_P34[$r13]
	;;
	ld $p32 = _K1_CONTEXT_P32[$r13]
	;;
	ld $p30 = _K1_CONTEXT_P30[$r13]
	;;
	ld $p28 = _K1_CONTEXT_P28[$r13]
	;;
	ld $p26 = _K1_CONTEXT_P26[$r13]
	;;
	ld $p24 = _K1_CONTEXT_P24[$r13]
	;;
	ld $p22 = _K1_CONTEXT_P22[$r13]
	;;
	ld $p20 = _K1_CONTEXT_P20[$r13]
	;;
	ld $p18 = _K1_CONTEXT_P18[$r13]
	;;
	ld $p16 = _K1_CONTEXT_P16[$r13]
	;;
	ld $p14 = _K1_CONTEXT_P14[$r13]
	;;
	ld $p10 = _K1_CONTEXT_P10[$r13]
	;;
	ld $p8 = _K1_CONTEXT_P8[$r13]
	;;
	ld $p6 = _K1_CONTEXT_P6[$r13]
	;;
	ld $p4 = _K1_CONTEXT_P4[$r13]
	;;
	ld $p2 = _K1_CONTEXT_P2[$r13]
	;;
	ld $p0 = _K1_CONTEXT_P0[$r13]
	;;
	ld $p12 = _K1_CONTEXT_P12[$r13]
	;;
	ret
	;;
.endp context_restore

 #
 # Context switch fucntion, r0 holds a pointer to the new context
 #
.global pok_context_switch
.proc pok_context_switch
.type pok_context_switch,@function
pok_context_switch:
	# add $r12, $r12, -16
	# ;;
	# sd 0[$r12] = $p0
	# ;;
	# copy $r17, $r0
	# ;;
	# call context_save
	# ;;
	# ld $p0 = 0[$r12]
	;;
	copy $r13, $r1
	;;
	call context_restore
	;;
	add   $r12, $r12, _K1_VCONTEXT64_SIZE
	;;
	ret
	;;
.endp pok_context_switch

#
# Copyright (C) 2009-2016 Kalray SA.
#
# All rights reserved.
#

#include <HAL/hal/hal.h>
#include "HAL/hal/context.h"
#include "mOS_common_types_s_c.h"

# Helper macro to save a full context including mOS virtual sfr registers
.macro context_save to
	sd _K1_CONTEXT_P0[\ to], $p0        ## $r0 - $r1
	;;
	sd _K1_CONTEXT_P2[\ to], $p2        ## $r2 - $r3
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
	sd _K1_CONTEXT_P4[\ to], $p4       ## $r4 - $r5
	;;
	sd _K1_CONTEXT_P6[\ to], $p6       ## $r6 - $r7
	get $r4 = $ra
	;;
	sd _K1_CONTEXT_P8[\ to], $p8       ## $r8 - $r9
	get $r5 = $cs
	;;
	sd _K1_CONTEXT_P10[\ to], $p10      ## $r10 - $r11
	;;
	sd _K1_CONTEXT_P12[\ to], $p12      ## $r12 - $r13
	get $r7 = $lc
	;;
	sd _K1_CONTEXT_P14[\ to], $p14      ## $r14 - $r15
	get $r8 = $ls
	;;
	sd _K1_CONTEXT_P16[\ to], $p16      ## $r16 - $r17
	get $r9 = $le
	;;
	sd _K1_CONTEXT_P18[\ to], $p18      ## $r18 - $r19
	;;
	sd _K1_CONTEXT_P20[\ to], $p20      ## $r20 - $r21
	;;
	sd _K1_CONTEXT_P22[\ to], $p22      ## $r22 - $r23
	;;
	lw $r20 = MOS_VC_REG_SSP[$r0]
	;;
	sd _K1_CONTEXT_P24[\ to], $p24      ## $r24 - $r25
	;;
	sd _K1_CONTEXT_P26[\ to], $p26      ## $r26 - $r27
	;;
	sd _K1_CONTEXT_P28[\ to], $p28      ## $r28 - $r29
	;;
	sd _K1_CONTEXT_P30[\ to], $p30      ## $r30 - $r31
	;;
	sd _K1_CONTEXT_P32[\ to], $p32      ## $r32 - $r33
	;;
	sd _K1_CONTEXT_P34[\ to], $p34      ## $r34 - $r35
	;;
	sd _K1_CONTEXT_P36[\ to], $p36      ## $r36 - $r37
	;;
	sd _K1_CONTEXT_P38[\ to], $p38      ## $r38 - $r39
	;;
	sd _K1_CONTEXT_P40[\ to], $p40      ## $r40 - $r41
	;;
	sd _K1_CONTEXT_P42[\ to], $p42      ## $r42 - $r43
	;;
	sd _K1_CONTEXT_P44[\ to], $p44      ## $r44 - $r45
	;;
	sd _K1_CONTEXT_P46[\ to], $p46      ## $r46 - $r47
	;;
	sd _K1_CONTEXT_P48[\ to], $p48      ## $r48 - $r49
	;;
	sd _K1_CONTEXT_P50[\ to], $p50      ## $r50 - $r51
	;;
	sd _K1_CONTEXT_P52[\ to], $p52      ## $r52 - $r53
	;;
	sd _K1_CONTEXT_P54[\ to], $p54      ## $r54 - $r55
	;;
	sd _K1_CONTEXT_P56[\ to], $p56      ## $r56 - $r57
	;;
	sd _K1_CONTEXT_P58[\ to], $p58      ## $r58 - $r59
	;;
	sd _K1_CONTEXT_P60[\ to], $p60      ## $r60 - $r61
	;;
	sd _K1_CONTEXT_P62[\ to], $p62      ## $r62 - $r63
	;;
	sw _K1_CONTEXT_SSP[\ to], $r20
	;;
	sd _K1_CONTEXT_RA_CS[\ to], $p4     ## $ra - $cs
	;;
	sd _K1_CONTEXT_PS_LC[\ to], $p6     ## $ps - $lc
	;;
	sd _K1_CONTEXT_LS_LE[\ to], $p8     ## $ls - $le
	;;
.endm


.macro restore_context from
	ld $p8 = _K1_CONTEXT_LS_LE[\ from]   ## $ls - $le
	;;
	ld $p6 = _K1_CONTEXT_PS_LC[\ from]   ## $ps - $lc
	set $ls = $r8
	;;
	ld $p4 = _K1_CONTEXT_RA_CS[\ from]   ## $ra - $cs
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
	lw $r20 = _K1_CONTEXT_SSP[\ from]
	;;
	ld $p62 = _K1_CONTEXT_P62[\ from]
	set $cs = $r5
	;;
	sw MOS_VC_REG_SSP[$r40], $r20
	;;
	ld $p60 = _K1_CONTEXT_P60[\ from]
	set $ra = $r4
	;;
	ld $p58 = _K1_CONTEXT_P58[\ from]
	;;
	ld $p56 = _K1_CONTEXT_P56[\ from]
	;;
	ld $p54 = _K1_CONTEXT_P54[\ from]
	;;
	ld $p52 = _K1_CONTEXT_P52[\ from]
	;;
	ld $p50 = _K1_CONTEXT_P50[\ from]
	;;
	ld $p48 = _K1_CONTEXT_P48[\ from]
	;;
	ld $p46 = _K1_CONTEXT_P46[\ from]
	;;
	ld $p44 = _K1_CONTEXT_P44[\ from]
	;;
	ld $p42 = _K1_CONTEXT_P42[\ from]
	;;
	ld $p40 = _K1_CONTEXT_P40[\ from]
	;;
	ld $p38 = _K1_CONTEXT_P38[\ from]
	;;
	ld $p36 = _K1_CONTEXT_P36[\ from]
	;;
	ld $p34 = _K1_CONTEXT_P34[\ from]
	;;
	ld $p32 = _K1_CONTEXT_P32[\ from]
	;;
	ld $p30 = _K1_CONTEXT_P30[\ from]
	;;
	ld $p28 = _K1_CONTEXT_P28[\ from]
	;;
	ld $p26 = _K1_CONTEXT_P26[\ from]
	;;
	ld $p24 = _K1_CONTEXT_P24[\ from]
	;;
	ld $p22 = _K1_CONTEXT_P22[\ from]
	;;
	ld $p20 = _K1_CONTEXT_P20[\ from]
	;;
	ld $p18 = _K1_CONTEXT_P18[\ from]
	;;
	ld $p16 = _K1_CONTEXT_P16[\ from]
	;;
	ld $p14 = _K1_CONTEXT_P14[\ from]
	;;
	ld $p10 = _K1_CONTEXT_P10[\ from]
	;;
	ld $p8 = _K1_CONTEXT_P8[\ from]
	;;
	ld $p6 = _K1_CONTEXT_P6[\ from]
	;;
	ld $p4 = _K1_CONTEXT_P4[\ from]
	;;
	ld $p2 = _K1_CONTEXT_P2[\ from]
	;;
	ld $p0 = _K1_CONTEXT_P0[\ from]
	;;
	ld $p12 = _K1_CONTEXT_P12[\ from]
	;;
.endm

 #
 # Context switch fucntion, r0 holds a pointer to the new context
 #
.global pok_context_switch
.type pok_context_switch,@function
pok_context_switch:
	copy $r13, $r0
	;;
	restore_context $r13
	;;
	add   $r12, $r12, _K1_VCONTEXT64_SIZE
	;;
	ret
	;;
.endp pok_context_switch

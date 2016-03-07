#
# Copyright (C) 2009-2014 Kalray SA.
#
# All rights reserved.
#

#include <types.h>
.section .locked_text, "ax", @progbits
.align 8
.global __user_do_scall


.global __user_do_scall_w
.proc __user_do_scall_w
.type	__user_do_scall_w, @function
__user_do_scall_w:
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
call __user_do_scall
;;
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
.endp

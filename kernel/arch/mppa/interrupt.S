#
# Copyright (C) 2009-2014 Kalray SA.
#
# All rights reserved.
#

#include <HAL/hal/hal.h>
#include "HAL/hal/context.h"
#include "mOS_common_types_s_c.h"

.section .locked_text, "ax", @progbits
.align 8
.global __vk1_asm_interrupt_handler
.proc __vk1_asm_interrupt_handler
__vk1_asm_interrupt_handler:
	add   $r12, $r12, -_K1_VCONTEXT64_SIZE     ## space for the regist ers
	;;
	_vk1_context64_save $r12
	;;
	make $r2, _scoreboard_start
	;;
	get $r3, $pcr
	;;
	extfz $r1, $r3, 15, 11
	;;
	sll $r1, $r1, 8
	;;
	add $r2, $r2, $r1
	;;
	lw $r0 = MOS_VC_REG_PS[ $r2 ]
	copy   $r1, $r12
	;;
	srl $r0 = $r0, 28
	add   $r12, $r12, -16        ## create scratch area
	call  __k1_do_int            ## jump to C code
	;;
	__vk1_asm_return_from_int:
	add   $r12, $r12, 16          ## destroy scratch area
	;;
	_vk1_context64_restore $r12
	;;
	add   $r12, $r12, _K1_VCONTEXT64_SIZE
	;;
	scall MOS_VC_RFE
	;;
	.endp __vk1_asm_interrupt_handler

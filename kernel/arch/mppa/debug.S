#
# Copyright (C) 2009-2014 Kalray SA.
#
# All rights reserved.
#

#include <HAL/hal/hal.h>
#include "HAL/hal/context.h"
#include "mOS_common_types_s_c.h"

.type __vk1_debug_handler, @function
.globl __vk1_debug_handler

.section .locked_text, "ax", @progbits
.align 8
.global __vk1_asm_debug_handler
.proc __vk1_asm_debug_handler
__vk1_asm_debug_handler:
	break	0x1ffff
	;;
	add	$r12, $r12, -16			## create scratch area
	call	__vk1_debug_handler		## jump to C code
	;;
	add	$r12, $r12, 16			## destroy scratch area
	;;
	scall	MOS_DEBUG_RETURN_TO_DEBUG
	;;
	.endp	__vk1_asm_debug_handler

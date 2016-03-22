#
# Copyright (C) 2009-2014 Kalray SA.
#
# All rights reserved.
#

#include <HAL/hal/hal.h>
#include "HAL/hal/context.h"
#include "mOS_common_types_s_c.h"

 #
 # Context switch fucntion, r3 holds a pointer to the old context
 #
.global      pok_context_switch
.type       pok_context_switch,@function
pok_context_switch:
	copy $r13, $r0
	;;
	_vk1_context64_restore $r13
	;;
	add   $r12, $r12, _K1_VCONTEXT64_SIZE
	;;
	scall MOS_VC_RFE
	;;
.endp pok_context_switch

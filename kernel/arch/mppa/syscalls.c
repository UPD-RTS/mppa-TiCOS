/*
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


#include <errno.h>
#include <core/debug.h>
#include <core/syscall.h>
#include <core/partition.h>

#include <types.h>

pok_ret_t pok_arch_sc_int(	uint32_t num,
						uint32_t arg1,
						uint32_t arg2,
						uint32_t arg3,
						uint32_t arg4,
						uint32_t arg5,
						uint32_t arg6,
						uint32_t arg7) __attribute__((used));

pok_ret_t pok_arch_sc_int(	uint32_t num,
						uint32_t arg1,
						uint32_t arg2,
						uint32_t arg3,
						uint32_t arg4,
						uint32_t arg5,
						uint32_t arg6,
						uint32_t arg7)

{


	// uint8_t			part_id;
	// pok_syscall_info_t	syscall_info;
	pok_ret_t		syscall_ret;
	// pok_syscall_args_t	syscall_args;
	// pok_syscall_id_t	syscall_id;

	// part_id  = 0; /*currently only PE0 --- pok_current_partition;*/
	//
	// /* prepare syscall_info */
	// syscall_info.partition	= part_id;
	// syscall_info.base_addr	= 0; /* pok_partitions[part_id].base_addr; */
	// syscall_info.thread	= 0; /* currently monothread POK_SCHED_CURRENT_THREAD; */
	//
	// /* prepare syscall_args */
	// syscall_args.arg1 = arg1;
	// syscall_args.arg2 = arg2;
	// syscall_args.arg3 = arg3;
	// syscall_args.arg4 = arg4;
	// syscall_args.arg5 = arg5;
	// syscall_args.arg6 = arg6;
	// syscall_args.arg7 = arg7;
	// syscall_args.nargs = 7;
	//
	// /* prepare syscall_id */
	// syscall_id = (pok_syscall_id_t) num;
	(void)syscall_ret;
	/* perform syscall */
	/*syscall_ret = pok_core_syscall (syscall_id, &syscall_args, &syscall_info);*/
	syscall_ret = POK_ERRNO_OK;
	return syscall_ret;
}

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


#ifndef __POK_SYSCALL_H__
#define __POK_SYSCALL_H__

#include <types.h>
#include <errno.h>

typedef enum
{
	POK_SYSCALL_CONSWRITE		=  10,
	POK_SYSCALL_GETTICK			=  20,

#if defined (POK_NEEDS_ARINC653)
	POK_SYSCALL_GET_TIME		=  21,
#endif

	POK_SYSCALL_THREAD_CREATE	=  50,
	POK_SYSCALL_THREAD_SLEEP	=  52,
//#if defined (POK_NEEDS_THREAD_SUSPEND) || defined (POK_NEEDS_ERROR_HANDLING)
	POK_SYSCALL_THREAD_SUSPEND	=  53,
//#endif
//	POK_SYSCALL_THREAD_RESTART	=  54,
	POK_SYSCALL_THREAD_STOP		=  55,
	POK_SYSCALL_THREAD_PERIOD	=  56,
	POK_SYSCALL_THREAD_STOPSELF	=  57,
	POK_SYSCALL_THREAD_ID		=  58,

//#if defined (POK_NEEDS_THREAD_SUSPEND) || defined (POK_NEEDS_ERROR_HANDLING)
	POK_SYSCALL_THREAD_SUSPEND_OTHER	= 67,
//#endif
	POK_SYSCALL_THREAD_RESUME		=  68,
	POK_SYSCALL_THREAD_START		=  69,
	POK_SYSCALL_THREAD_DELAYED_START	=  70,


#ifdef POK_NEEDS_PORTS_SAMPLING
	POK_SYSCALL_MIDDLEWARE_SAMPLING_ID		= 101,
	POK_SYSCALL_MIDDLEWARE_SAMPLING_READ	= 102,
//	POK_SYSCALL_MIDDLEWARE_SAMPLING_STATUS	= 103,
	POK_SYSCALL_MIDDLEWARE_SAMPLING_WRITE	= 104,
	POK_SYSCALL_MIDDLEWARE_SAMPLING_CREATE	= 105,
#endif
#ifdef POK_NEEDS_PORTS_QUEUEING
	POK_SYSCALL_MIDDLEWARE_QUEUEING_CREATE	= 110,
	POK_SYSCALL_MIDDLEWARE_QUEUEING_SEND	= 111,
	POK_SYSCALL_MIDDLEWARE_QUEUEING_RECEIVE	= 112,
//	POK_SYSCALL_MIDDLEWARE_QUEUEING_ID		= 113,
//	POK_SYSCALL_MIDDLEWARE_QUEUEING_STATUS	= 114,
#endif

#if defined (POK_NEEDS_LOCKOBJECTS) || defined (POK_NEEDS_EVENTS) || defined (POK_NEEDS_BUFFERS) || defined (POK_NEEDS_BLACKBOARDS)
	POK_SYSCALL_LOCKOBJ_CREATE				= 201,
	POK_SYSCALL_LOCKOBJ_OPERATION			= 202,
#endif
#ifdef POK_NEEDS_ERROR_HANDLING
	POK_SYSCALL_ERROR_HANDLER_CREATE		= 301,
	POK_SYSCALL_ERROR_HANDLER_SET_READY		= 302,
	POK_SYSCALL_ERROR_RAISE_APPLICATION_ERROR = 303,
	POK_SYSCALL_ERROR_GET					= 304,
#endif
#ifdef POK_NEEDS_PARTITIONS
	POK_SYSCALL_PARTITION_SET_MODE			= 404,
#endif
} pok_syscall_id_t;

typedef struct
{
	uint32_t	nargs;
	uint32_t	arg1;
	uint32_t	arg2;
	uint32_t	arg3;
	uint32_t	arg4;
	uint32_t	arg5; 
	uint32_t	arg6;
	uint32_t	arg7;
} pok_syscall_args_t;

typedef struct
{
	pok_partition_id_t	partition;
	uint32_t			thread;
	uint32_t			base_addr;
}pok_syscall_info_t;


/**
 *  Function that performs the syscall. It is called by the 
 *  architecture interruption handler.
 *
 *  @param syscall_id
 *  This param correspond to the syscal which should be performed.
 *  The list of available syscalls is available in
 *  the definition of the pok_syscall_id_t type
 *
 *  @param args
 *  Arguments of the syscall. It corresponds to data useful
 *  to perform the syscall.
 *
 *  @param infos
 *  Informations about the syscall: which partition/thread
 *  initiates the syscall, etc ...
 *
 *  @return
 *  Returns an error code, which is defined in include/errno.h
 */

pok_ret_t	pok_core_syscall (const pok_syscall_id_t	  syscall_id,
					const pok_syscall_args_t*  args,
					const pok_syscall_info_t*  infos);

/*
 * Initiate syscalls.
 * This part is defined in low-level layers, each architecture/bsp
 * initiate the syscall in a different way.
 */
pok_ret_t pok_syscall_init();

#define POK_CHECK_PTR_OR_RETURN(pid,ptr)		\
	if (!POK_CHECK_PTR_IN_PARTITION(pid,ptr))	\
{												\
	return POK_ERRNO_EINVAL;					\
}

#endif /* __POK_SYSCALL_H__ */

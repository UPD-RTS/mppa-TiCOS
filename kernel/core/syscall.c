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


#include <bsp.h>
#include <types.h>
#include <libc.h>

#include <errno.h>
#include <core/debug.h>
#include <core/sched.h>
#include <core/syscall.h>
#include <core/partition.h>
#include <core/thread.h>
#include <core/lockobj.h>
#include <core/time.h>
#include <core/error.h>

#include <middleware/port.h>


pok_ret_t pok_core_syscall (	const pok_syscall_id_t		 syscall_id,
				const pok_syscall_args_t*	 args,
				const pok_syscall_info_t*	 infos)
{
	switch (syscall_id)
	{
#if defined (POK_NEEDS_CONSOLE) || defined (POK_NEEDS_DEBUG)
		case POK_SYSCALL_CONSWRITE:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1 + infos->base_addr)
			if (pok_cons_write ((const char*)args->arg1 + infos->base_addr, args->arg2))
			{
				return POK_ERRNO_OK;
			}
			else
			{
				return POK_ERRNO_EINVAL;
			}
			break;
#endif

		case POK_SYSCALL_GETTICK:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1 + infos->base_addr)
			return pok_get_system_time ((uint64_t*) (args->arg1 + infos->base_addr));
			break;

#if defined (POK_NEEDS_ARINC653)
		case POK_SYSCALL_GET_TIME:
			return pok_get_HD_clock ((uint64_t*) (args->arg1 + infos->base_addr));
			break;
#endif

		case POK_SYSCALL_THREAD_CREATE:
			return pok_partition_thread_create  ((uint32_t*) (args->arg1 + infos->base_addr),
												(pok_thread_attr_t*) (args->arg2 + infos->base_addr),
												(uint8_t) infos->partition);
			break;

#ifdef POK_NEEDS_SCHED_O1
		case POK_SYSCALL_THREAD_SLEEP:
			return pok_thread_sleep (args->arg1);
			break;
#endif

/*	NOT USED at this time
#ifdef POK_NEEDS_THREAD_SLEEP_UNTIL
		case POK_SYSCALL_THREAD_SLEEP_UNTIL:
			return pok_thread_sleep_until (args->arg1);
			break;
#endif
*/
		case POK_SYSCALL_THREAD_PERIOD:
			return pok_sched_end_period ();
			break;

//#if defined (POK_NEEDS_THREAD_SUSPEND) || defined (POK_NEEDS_ERROR_HANDLING)
		case POK_SYSCALL_THREAD_SUSPEND:
			return pok_thread_suspend ();
			break;
		case POK_SYSCALL_THREAD_SUSPEND_OTHER:
			return pok_thread_suspend_other((uint32_t) (args->arg1 + infos->base_addr));
			break;
//#endif
		case POK_SYSCALL_THREAD_START:
			return pok_thread_start((uint32_t*) (args->arg1 + infos->base_addr));
			break;

		case POK_SYSCALL_THREAD_RESUME:
			return pok_thread_resume((uint32_t*) (args->arg1 + infos->base_addr));
			break;
#ifdef POK_NEEDS_SCHED_O1
		case POK_SYSCALL_THREAD_DELAYED_START:
			return pok_thread_delayed_start(((uint32_t*) (args->arg1 + infos->base_addr)),(uint32_t)args->arg2);
			break;
#endif

		case POK_SYSCALL_THREAD_ID:
			return pok_sched_get_current ((uint32_t*) (args->arg1 + infos->base_addr));
			break;

		case POK_SYSCALL_THREAD_STOPSELF:
			pok_sched_stop_self ();
			pok_sched();
			return POK_ERRNO_OK;
			break;

		case POK_SYSCALL_THREAD_STOP:
			return pok_partition_stop_thread (args->arg1);
			break;

#ifdef POK_NEEDS_PARTITIONS
		case POK_SYSCALL_PARTITION_SET_MODE:
			return pok_partition_set_mode_current ((pok_partition_mode_t)args->arg1);
			break;
#endif


#ifdef POK_NEEDS_ERROR_HANDLING

//		case POK_SYSCALL_ERROR_HANDLER_CREATE:
//			return pok_error_thread_create (args->arg1 , (void*) (args->arg2));
//			break;

	  	case POK_SYSCALL_ERROR_RAISE_APPLICATION_ERROR:
 # if defined (POK_NEEDS_ARINC653)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
			pok_error_raise_application_error (args->arg1, (char*) (args->arg2 + infos->base_addr), args->arg3);
 # else
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1 + infos->base_addr)
			pok_error_raise_application_error ((char*) (args->arg1 + infos->base_addr), args->arg2);
 # endif

			return POK_ERRNO_OK;
			break;

#endif	// POK_NEEDS_ERROR_HANDLING

			/* Middleware syscalls */
#ifdef POK_NEEDS_PORTS_SAMPLING
		case POK_SYSCALL_MIDDLEWARE_SAMPLING_CREATE:
			if (args->arg6 != POK_NULL){
 #ifdef POK_SD
				printf("in syscall.c base addr: 0x%x, args->arg6 0x%x,(args->arg6 + infos->base_addr) 0x%x\n",
				infos->base_addr, args->arg6,(args->arg6 + infos->base_addr));
 #endif
				POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg6 + infos->base_addr)
			}
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1 + infos->base_addr)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg5 + infos->base_addr)

			return pok_port_sampling_create ((char*)(args->arg1 + infos->base_addr),
							(pok_port_size_t) args->arg2,
							(pok_port_direction_t) args->arg3,
							(uint64_t) args->arg4,
							(pok_port_id_t*) (args->arg5 + infos->base_addr),
							(void*) args->arg6);
			break;

		case POK_SYSCALL_MIDDLEWARE_SAMPLING_WRITE:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
			return pok_port_sampling_write ((const pok_port_id_t)args->arg1,
														(const void*) ((void*)args->arg2 + infos->base_addr),
														(const uint8_t) args->arg3);
			break;

		case POK_SYSCALL_MIDDLEWARE_SAMPLING_READ:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg4 + infos->base_addr)
			return pok_port_sampling_read ((const pok_port_id_t)args->arg1,
													(void*) args->arg2 + infos->base_addr,
													(pok_port_size_t*) (args->arg3 + infos->base_addr),
													(bool_t*) (args->arg4 + infos->base_addr));
			break;

		case POK_SYSCALL_MIDDLEWARE_SAMPLING_ID:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1 + infos->base_addr)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
			return pok_port_sampling_id	((char*)(args->arg1 + infos->base_addr),
								(pok_port_id_t*)(args->arg2 + infos->base_addr));
			break;

/* NOT USED at the moment
		case POK_SYSCALL_MIDDLEWARE_SAMPLING_STATUS:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2+infos->base_addr)
			return pok_port_sampling_status ((const pok_port_id_t)args->arg1,
														(pok_port_sampling_status_t*) (args->arg2 + infos->base_addr));
			break;
*/
#endif /* POK_NEEDS_PORTS_SAMPLING */


#ifdef POK_NEEDS_PORTS_QUEUEING
		case POK_SYSCALL_MIDDLEWARE_QUEUEING_CREATE:
			if (args->arg7 != POK_NULL){
#ifdef POK_QD
				printf("in syscall.c base addr: 0x%x, args->arg6 0x%x,(args->arg6 + infos->base_addr) 0x%x\n",
				infos->base_addr, args->arg7,(args->arg7 + infos->base_addr));
#endif
				POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg7 + infos->base_addr)
			}
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1 + infos->base_addr)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg6 + infos->base_addr)
			return pok_port_queueing_create ((char*) (args->arg1 + infos->base_addr),
											(pok_port_size_t)					args->arg2,
											(pok_range_t)						args->arg3,
											(pok_port_direction_t)				args->arg4,
											(pok_port_queueing_discipline_t)	args->arg5,
											(pok_port_id_t*)					(args->arg6 + infos->base_addr),
											(void*)								args->arg7);
			break;

		case POK_SYSCALL_MIDDLEWARE_QUEUEING_SEND:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
			return pok_port_queueing_send ((const pok_port_id_t)		args->arg1,
											(const void*)		((void*)args->arg2 + infos->base_addr),
											(const uint8_t)		(args->arg3),
											(const uint64_t)	args->arg4);
			break;

		case POK_SYSCALL_MIDDLEWARE_QUEUEING_RECEIVE:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg4 + infos->base_addr)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg5 + infos->base_addr)
			return pok_port_queueing_receive(	(const pok_port_id_t)	args->arg1,
												(uint64_t)				args->arg2,
												(pok_port_size_t)		args->arg3,
												(void*)					((void*)args->arg4 + infos->base_addr),
												(pok_port_size_t*)		(args->arg5 + infos->base_addr));
			break;

/* NOT USED at the moment
		case POK_SYSCALL_MIDDLEWARE_QUEUEING_ID:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1 + infos->base_addr)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
			return pok_port_queueing_id ((char*)			 (args->arg1 + infos->base_addr),
												(pok_port_id_t*)  (args->arg2 + infos->base_addr));
			break;
*/

/* NOT USED at the moment
		case POK_SYSCALL_MIDDLEWARE_QUEUEING_STATUS:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
			return pok_port_queueing_status	  ((const pok_port_id_t)			  args->arg1,
															(pok_port_queueing_status_t*)	 (args->arg2 + infos->base_addr));
			break;
*/

#endif /* POK_NEEDS_PORTS_QUEUEING */

#ifdef POK_NEEDS_LOCKOBJECTS
		case POK_SYSCALL_LOCKOBJ_CREATE:
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2+infos->base_addr)
			POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg1+infos->base_addr)
			return pok_lockobj_partition_create	 ((pok_lockobj_id_t*)	 (args->arg1 + infos->base_addr),
					(pok_lockobj_attr_t*)	(args->arg2 + infos->base_addr));
			break;

		case POK_SYSCALL_LOCKOBJ_OPERATION:
			if (args->arg2 == POK_NULL)
			{
				return pok_lockobj_partition_wrapper ((const uint8_t) args->arg1, POK_NULL);
			}
			else
			{
				POK_CHECK_PTR_OR_RETURN(infos->partition, args->arg2 + infos->base_addr)
				return pok_lockobj_partition_wrapper	((const uint8_t) args->arg1,
														(pok_lockobj_lockattr_t*) (args->arg2 + infos->base_addr));
			}
			break;
#endif /* POK_NEEDS_LOCKOBJECTS */

		/**
		 * Here is the default syscall handler. In this case, the syscall
		 * ID was not properly identified and thus, we should return an
		 * error. If error management is activated, we raise an error
		 * in kernel of partitions, calling the error handler.
		 */
		default:
#ifdef POK_NEEDS_ERROR_HANDLING
			pok_error_declare (POK_ERROR_KIND_ILLEGAL_REQUEST);
			pok_sched_activate_error_thread ();
#else
			#ifdef POK_NEEDS_DEBUG
			printf ("Tried to use syscall %d\n", syscall_id);
			#endif
			POK_FATAL ("Unknown syscall");
#endif
			break;
	}
	return POK_ERRNO_EINVAL;
}

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



#ifndef __POK_THREAD_H__
#define __POK_THREAD_H__


#ifdef POK_NEEDS_THREADS

#include <types.h>
#include <errno.h>
#include <core/sched.h>


/*
 * In POK, we add a kernel thread and an idle thread. The kernel
 * thread is used to execute kernel code while the idle thread
 * is used to save processor resources.
 */

#define KERNEL_THREAD	POK_CONFIG_NB_THREADS -2
#define IDLE_THREAD		POK_CONFIG_NB_THREADS -1

#define POK_THREAD_DEFAULT_TIME_CAPACITY 10

#define POK_THREAD_MAX_PRIORITY  63

#define  INFINITE_SYSTEM_TIME_VALUE	 		4294967295uL			 /* (2**32)-1 */
#define  APERIODIC_PROCESS					INFINITE_SYSTEM_TIME_VALUE

/*
 * IDLE_STACK_SIZE is the stack size of the idle thread
 */

#ifndef	IDLE_STACK_SIZE
	#define IDLE_STACK_SIZE		8192
#endif


#ifndef POK_USER_STACK_SIZE
	#define POK_USER_STACK_SIZE 4096
#endif


typedef struct pok_thread_t
{

	uint8_t		priority;			/* base priority */
	uint8_t		current_priority;	/* current priority */
	uint64_t	period;
	uint64_t	deadline;
	uint64_t	time_capacity;
	uint64_t	remaining_time_capacity;
	uint64_t	next_activation;
	pok_state_t	state;
	uint64_t	end_time;
	uint64_t	wakeup_time;
#ifdef POK_NEEDS_SCHED_HFPPS
	uint64_t	payback; /**< Payback for HFPPS scheduling algorithm */
#endif /* POK_NEEDS_SCHED_HFPPS */
/* pok_state_t  state; */
	void		*entry;
	uint8_t		partition;
	uint32_t	sp;
	uint32_t	init_stack_addr;
	bool_t 		suspended;
	bool_t 		suspended_self;

#if defined POK_NEEDS_SCHED_FPPS || defined POK_NEEDS_SCHED_O1 || defined POK_NEEDS_SCHED_O1_SPLIT
	/* previous and next threads in the thread queue (needed for FPPS scheduling) */
	struct pok_thread_t* 	previous;
	/* next thread in the thread queue (needed for FPPS scheduling) */
	struct pok_thread_t* 	next;
	uint32_t				id;
#endif

#if defined POK_NEEDS_SCHED_O1 || defined POK_NEEDS_SCHED_O1_SPLIT
	uint32_t pos;
	struct pok_sched_asynch_event_t* timeout;
#endif

} pok_thread_t;

typedef struct
{
	uint8_t		priority;	/* Priority is from 0 to 255 */
	void*		entry;		/* entrypoint of the thread  */
	uint64_t	period;
	uint64_t	deadline;
	uint64_t	time_capacity;
	uint32_t	stack_size;
} pok_thread_attr_t;

/*
 * Attributes given to create a thread
 */

void		pok_thread_init (void);
pok_ret_t	pok_thread_create (uint8_t* thread_id, const pok_thread_attr_t* attr);
pok_ret_t	pok_thread_sleep (const uint32_t ms);
//pok_ret_t	pok_thread_sleep_until (const uint32_t ms);
//pok_ret_t	pok_thread_suspend (void);
//void		pok_thread_start (void (*entry)(), unsigned int id);
//pok_ret_t	pok_thread_restart (const uint32_t tid);
pok_ret_t	pok_thread_start (uint32_t *tid);

#ifdef POK_ARCH_MPPA
void pok_thread_start_execution(void (*entry)(), unsigned int id);
#endif

pok_ret_t	pok_thread_delayed_start (uint32_t *id, uint32_t delay_time);
bool_t 		pok_thread_aperiodic(pok_thread_t	  pok_thread);
bool_t 		pok_thread_preemption_enabled(pok_thread_t	  pok_thread);
void 		pok_thread_reset_stack(uint32_t init_stack_addr);
void 		pok_thread_reset_context(uint32_t sp);
bool_t 		pok_thread_is_suspended(pok_thread_t	  pok_thread);
pok_ret_t	pok_thread_resume (const uint32_t* tid);
//#if defined (POK_NEEDS_THREAD_SUSPEND) || defined (POK_NEEDS_ERROR_HANDLING)
pok_ret_t	pok_thread_suspend_other (uint32_t tid);
pok_ret_t	pok_thread_suspend (void);
//#endif

#ifdef POK_NEEDS_PARTITIONS
pok_ret_t	pok_partition_thread_create (uint32_t* thread_id,
										const pok_thread_attr_t* attr,
										const uint8_t  partition_id);
#endif

extern pok_thread_t	pok_threads[POK_CONFIG_NB_THREADS];

#define POK_CURRENT_THREAD pok_threads[POK_SCHED_CURRENT_THREAD]

#endif /* __POK_NEEDS_THREADS */

#endif /* __POK_THREAD_H__ */

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


/**
 * \file	 core/thread.c
 * \date	 2008-2009
 * \brief	Thread management in kernel
 */

#include <types.h>

#include <arch.h>
#include <core/debug.h>
#include <core/error.h>
#include <core/thread.h>
#include <core/sched.h>
#include <core/partition.h>
#include <core/time.h>


#include <dependencies.h>
#if defined (POK_NEEDS_CONSOLE) || defined (POK_NEEDS_DEBUG) 
 #include <libc.h>
#endif


#ifdef POK_NEEDS_THREADS

/**
 * We declare an array of threads. The amount of threads
 * is fixed by the software developper and we add two theads
 *	 - one for the kernel thread (this code)
 *	 - one for the idle task
 */
pok_thread_t	pok_threads[POK_CONFIG_NB_THREADS];
#ifdef POK_NEEDS_SCHED_O1
uint32_t		pok_thread_ids_to_pos_map[POK_CONFIG_NB_THREADS];
#endif

#ifdef POK_NEEDS_SCHED_O1_SPLIT
/* we store the deadline of each thread */
uint64_t pok_threads_deadline[POK_CONFIG_NB_THREADS-2] = ((uint64_t[]) POK_CONFIG_SPORADIC_DEADLINE);
/* we store a bitmask for every thread with:
 * - all 0s if the thread is not a predecessor
 * - a 1 in the position corresponding to the successor otherwise
 * the array is indexed with thread id
 */
uint32_t successors_bitmasks[POK_CONFIG_NB_THREADS] = {0};
/* we also store a bitmask for every thread with:
 * - all 0s if the thread is not a predecessor
 * - all 1s if the thread is a predecessor
 * the array is indexed with thread id
 */
uint32_t is_predecessor[POK_CONFIG_NB_THREADS] = {0};
/* we store an array of booleans to say if a thread has tardiness; the array is indexed
 * with the thread id
 */
bool_t has_tardiness[POK_CONFIG_NB_THREADS] = ((bool_t[]) POK_CONFIG_SPORADIC_TARDINESS);
/* we store an array of events id to keep the mapping between predecessors threads and
 * their events
 */
uint8_t predecessors_events_id[POK_CONFIG_NB_THREADS-2] = ((uint8_t[])POK_CONFIG_PREDECESSORS_EVENTS);
#endif

#ifdef POK_NEEDS_SCHED_FPPS
// pointers needed by FPPS
pok_thread_t* priority_heads[POK_CONFIG_NB_PARTITIONS][POK_CONFIG_PRIORITY_LEVELS];
pok_thread_t* priority_tails[POK_CONFIG_NB_PARTITIONS][POK_CONFIG_PRIORITY_LEVELS];
pok_thread_t* heads[POK_CONFIG_NB_PARTITIONS];

extern void insert_in_queue();
#endif

extern pok_partition_t	  	pok_partitions[POK_CONFIG_NB_PARTITIONS];

#ifdef POK_NEEDS_SCHED_O1
/**
 * Sorts the threads according to their priority (descending).
 * This part is dedicated to the O1 scheduling algorithm.
 */
void pok_thread_insert_sort_by_priority(uint16_t index_low, uint16_t index_high)
{
	uint32_t i=index_high,j=0;
	pok_thread_t val;

	val=pok_threads[i]; //val is the new thread being inserted
	j=i-1;
	while ( j>= index_low && pok_threads[j].priority <= val.priority)
	{
		pok_threads[j+1] = pok_threads[j];
		pok_thread_ids_to_pos_map[pok_threads[j].id]++;
		pok_threads[j+1].pos++;
		j--;
		val.pos--;
		pok_thread_ids_to_pos_map[val.id]--;
	}
	pok_threads[j+1]=val;
}
#endif

/**
 * Initialize thread array, put their default values
 * and so on
 */
void pok_thread_init(void)
{
	uint32_t i;

#ifdef POK_NEEDS_PARTITIONS
	uint32_t total_threads;
	uint8_t  j;

	total_threads = 0;

	for (j = 0 ; j < POK_CONFIG_NB_PARTITIONS ; j++)
	{
		total_threads = total_threads + pok_partitions[j].nthreads;
	}

 #if defined (POK_NEEDS_DEBUG) || defined (POK_NEEDS_ERROR_HANDLING)
	if (total_threads != (POK_CONFIG_NB_THREADS - 2))
	{
  #ifdef POK_NEEDS_DEBUG
		printf ("Error in configuration, bad number of threads\n");
  #endif
  #ifdef POK_NEEDS_ERROR_HANDLING
		pok_kernel_error (POK_ERROR_KIND_KERNEL_CONFIG);
  #endif
	}
 #endif
#endif /* POK_NEEDS_PARTITIONS */

	pok_threads[KERNEL_THREAD].priority			= pok_sched_get_priority_min(0);
	pok_threads[KERNEL_THREAD].state			= POK_STATE_RUNNABLE;
	pok_threads[KERNEL_THREAD].next_activation	= 0;

	pok_threads[IDLE_THREAD].period				= 0;
	
#ifdef POK_NEEDS_SCHED_O1_SPLIT
	pok_threads[IDLE_THREAD].deadline			= 1000000; // +INF
	pok_threads[IDLE_THREAD].time_capacity		= 1000000; // +INF
	pok_threads[IDLE_THREAD].pos				= IDLE_THREAD;
	pok_threads[IDLE_THREAD].id					= IDLE_THREAD;
#else
	pok_threads[IDLE_THREAD].deadline			= 0;
	pok_threads[IDLE_THREAD].time_capacity		= 0;
#endif
	pok_threads[IDLE_THREAD].next_activation	= 0;
	pok_threads[IDLE_THREAD].remaining_time_capacity = 0;
	pok_threads[IDLE_THREAD].wakeup_time		= 0;
	pok_threads[IDLE_THREAD].entry				= pok_arch_idle;
	pok_threads[IDLE_THREAD].priority			= pok_sched_get_priority_min(0);
	pok_threads[IDLE_THREAD].state				= POK_STATE_RUNNABLE;
	pok_threads[IDLE_THREAD].sp					= pok_context_create (IDLE_THREAD,IDLE_STACK_SIZE,(uint32_t)pok_arch_idle);

	// KERNEL_THREAD and IDLE_THREAD must not be initialized again
	for (i = 0; i < POK_CONFIG_NB_THREADS-2; ++i)
	{
		pok_threads[i].period					= 0;
		pok_threads[i].deadline					= 0;
		pok_threads[i].time_capacity			= 0;
		pok_threads[i].remaining_time_capacity	= 0;
		pok_threads[i].next_activation			= 0;
		pok_threads[i].wakeup_time				= 0;
		pok_threads[i].state					= POK_STATE_STOPPED;
#ifdef POK_NEEDS_SCHED_O1_SPLIT
		/* if the thread is a predecessor set all 1s in its bitmask in the is_predecessor array */
		if (((bool_t[])POK_CONFIG_PREDECESSOR_THREADS)[i] == TRUE)
		{
			is_predecessor[i] = ~(0);
		}
#endif
	}
	
#ifdef POK_NEEDS_SCHED_FPPS
	// FPPS pointers initialization
	for (i = 0; i < POK_CONFIG_NB_PARTITIONS; ++i)
	{
		heads[i] = NULL;
 #ifdef POK_NEEDS_DEBUG_HEAD_TAIL
		printf("heads[%d] configured\n",i);
 #endif
		for (j = 0; j < POK_CONFIG_PRIORITY_LEVELS; ++j)
		{
			priority_heads[i][j] = NULL;
			priority_tails[i][j] = NULL;
 #ifdef POK_NEEDS_DEBUG_HEAD_TAIL
			printf("priority_heads[%d][%d] configured\n",i,j);
			printf("priority_tails[%d][%d] configured\n",i,j);
 #endif
	  	}
	}
#endif /* POK_NEEDS_SCHED_FPPS */

}

#ifdef POK_NEEDS_PARTITIONS
/**
 * Create a thread inside a partition
 * Return POK_ERRNO_OK if no error.
 * Return POK_ERRNO_TOOMANY if the partition cannot contain
 * more threads.
 */
pok_ret_t pok_partition_thread_create (	uint32_t*					thread_id,
										const pok_thread_attr_t*	attr,
										const uint8_t				partition_id)
{
	uint32_t id;
	uint32_t stack_vaddr;

	/**
	* We can create a thread only if the partition is in INIT mode
	*/
	if (  (pok_partitions[partition_id].mode != POK_PARTITION_MODE_INIT_COLD) &&
		(pok_partitions[partition_id].mode != POK_PARTITION_MODE_INIT_WARM) )
	{
		return POK_ERRNO_MODE;
	}

	if (pok_partitions[partition_id].thread_index >= pok_partitions[partition_id].thread_index_high)
	{
 #ifdef POK_NEEDS_ERROR_HANDLING
		POK_ERROR_CURRENT_PARTITION (POK_ERROR_KIND_PARTITION_CONFIGURATION);
 #endif
		return POK_ERRNO_TOOMANY;
	}

	id = pok_partitions[partition_id].thread_index_low +  pok_partitions[partition_id].thread_index;
	
	if ((attr->priority <= pok_sched_get_priority_max (pok_partitions[partition_id].sched)) 
		&& (attr->priority >= pok_sched_get_priority_min (pok_partitions[partition_id].sched)))
	{
		pok_threads[id].priority	= attr->priority;
	}

	if (attr->period > 0)
	{
		pok_threads[id].period		= attr->period;
		pok_threads[id].next_activation = attr->period;
	}

	if (attr->deadline > 0)
	{
		pok_threads[id].deadline	= attr->deadline;
	}

	if (attr->time_capacity > 0)
	{
		pok_threads[id].time_capacity = attr->time_capacity;
		pok_threads[id].remaining_time_capacity = attr->time_capacity;
	}
	else
	{
		pok_threads[id].remaining_time_capacity	= POK_THREAD_DEFAULT_TIME_CAPACITY;
		pok_threads[id].time_capacity = POK_THREAD_DEFAULT_TIME_CAPACITY;
	}

	// allocate memory for the user stack (the memory is inside the partition area)
	stack_vaddr = pok_thread_stack_addr (partition_id, pok_partitions[partition_id].thread_index);

	pok_threads[id].state = POK_STATE_RUNNABLE;
	pok_threads[id].wakeup_time	= 0;

	// allocate memory for the kernel stack (the memory is outside the partition area, it is in the heap)
	pok_threads[id].sp = pok_space_context_create (partition_id,
							(uint32_t)attr->entry,
							stack_vaddr,
							0xdead,
							0xbeaf);

	pok_threads[id].partition		= partition_id; 
	pok_threads[id].entry			= attr->entry;
	pok_threads[id].init_stack_addr = stack_vaddr;
 # if defined POK_NEEDS_SCHED_FPPS || defined POK_NEEDS_SCHED_O1
	pok_threads[id].id				= id;
	pok_threads[id].next			= NULL;
	pok_threads[id].previous		= NULL;
 # endif

 # ifdef POK_NEEDS_SCHED_O1
	pok_threads[id].pos				= id;
 #endif

	pok_threads[id].next_activation = 0;
	pok_threads[id].state			= POK_STATE_STOPPED;
	*thread_id 						= id;

 #ifdef POK_NEEDS_SCHED_O1
	if ((pok_partitions[partition_id].sched == POK_SCHED_O1 || pok_partitions[partition_id].sched == POK_SCHED_O1_SPLIT) && (id > pok_partitions[partition_id].thread_index_low))
	{
		pok_thread_insert_sort_by_priority(pok_partitions[partition_id].thread_index_low+1,id);

  #ifdef POK_NEEDS_DEBUG_O1
		printf("THREAD IDs ORDERED BY PRIORITY:");
		int zzz=0;
		for(;zzz<POK_CONFIG_NB_THREADS;zzz++)
			printf(" %d",pok_threads[zzz].id);
		printf("\n");
		printf("THREAD PRIORITYs ORDERED BY PRIORITY:");
		for(zzz=0;zzz<POK_CONFIG_NB_THREADS;zzz++)
			printf(" %d",pok_threads[zzz].priority);
		printf("\n");
		printf("THREAD POSITIONs ORDERED BY PRIORITY:");
		for(zzz=0;zzz<POK_CONFIG_NB_THREADS;zzz++)
			printf(" %d",pok_threads[zzz].pos);
		printf("\n");
		printf("ORIGINAL POSITIONs:");
		for(zzz=0;zzz<POK_CONFIG_NB_THREADS;zzz++)
			printf(" %d->%d\t",zzz,pok_thread_ids_to_pos_map[zzz]);
		printf("\n");
  #endif
	}
 #endif /* ifdef POK_NEEDS_SCHED_O1 */

 #ifdef POK_NEEDS_DEBUG
	printf("DEBUG::pok_thread %d created in partition %d\n",*thread_id,partition_id);
	printf("DEBUG::partition %d: index_low %d, index_high %d\n",partition_id,pok_partitions[partition_id].thread_index_low,pok_partitions[partition_id].thread_index_high);
 #endif

	pok_partitions[partition_id].thread_index =  pok_partitions[partition_id].thread_index + 1;
	return POK_ERRNO_OK;
}

#endif /* POK_NEEDS_PARTITIONS */

/**
 * Start a thread, giving its entry call with \a entry
 * and its identifier with \a id
 * DO NO REMOVE - This function is called in cswitch in order to start the 
 * idle thread
 * MINOR CHANGE: function original name: pok_thread_start
 * the name has been changed in order to avoid ambiguity with the next function
 */
void pok_thread_start_execution(void (*entry)(), unsigned int id)
{
	(void) id;
	entry();
}


pok_ret_t	pok_thread_start (uint32_t *id)
{
#ifdef POK_NEEDS_SCHED_O1
	uint32_t thread_id = pok_thread_ids_to_pos_map[*id];
#else
	uint32_t thread_id = *id;
#endif
	pok_ret_t ret =  POK_ERRNO_OK;
	// if (the process is APERIODIC) then
	if (pok_thread_aperiodic(pok_threads[thread_id]))
	{
		// set the current priority of specified process to its base priority;
		pok_threads[thread_id].current_priority =  pok_threads[thread_id].priority;
	
		//reset context and stack; Not necessary at this time
	
		// if (operating mode is NORMAL) then		
		// the partition mode is set to NORMAL by the user code after the thread creation, not by the kernel
		if (pok_partitions[pok_threads[thread_id].partition].mode ==  POK_PARTITION_MODE_NORMAL) 
		{
			// set the specified process state to RUNNABLE
			pok_threads[thread_id].state = POK_STATE_RUNNABLE;
#ifdef POK_NEEDS_SCHED_FPPS	
			insert_in_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[thread_id]);
#endif
			// set the DEADLINE_TIME value for the specified process to (current system clock plus TIME_CAPACITY);
			pok_threads[thread_id].deadline = POK_GETTICK() + pok_threads[thread_id].time_capacity;
			// if (preemption is enabled) then
/*			if (pok_thread_preemption_enabled(pok_threads[thread_id]))
			{
				// ask for process scheduling;
				// commented for RUN TO COMPLETION semantics
				//pok_sched ();	
			}
*/
		}else{
		  	// the mode of the partition is COLD_START or WARM_START
		  	// set the specified process state to WAITING;
		  	// the process will wait until partition become in NORMAL mode
			pok_threads[thread_id].state = POK_STATE_WAITING;
		}
		ret =  POK_ERRNO_OK;
		// At the end of the initialization phase (when the partition mode becomes NORMAL) the process will be set to RUNNABLE
	}else{
		// PERIODIC process
		// set the current priority of specified process to its base priority;
		pok_threads[thread_id].current_priority =  pok_threads[thread_id].priority;

		// if (operating mode is NORMAL) then
		if (pok_partitions[pok_threads[thread_id].partition].mode ==  POK_PARTITION_MODE_NORMAL)
		{
			// set the specified process state to WAITING;
			pok_threads[thread_id].state = POK_STATE_WAITING;
			// set the first release point of the specified process;
			pok_threads[thread_id].next_activation = pok_partitions[pok_threads[thread_id].partition].activation + 
				get_partition_period(pok_threads[thread_id].partition);
			// set the DEADLINE_TIME value for the specified process to (current system clock plus TIME_CAPACITY);
			// process has to wait for its release point
			pok_threads[thread_id].deadline = POK_GETTICK() + pok_threads[thread_id].time_capacity;
		}else{
			// the mode of the partition is COLD_START or WARM_START)
			// set the specified process state to WAITING;
			// the specified process will wait until partition become in NORMAL mode
			pok_threads[thread_id].state = POK_STATE_WAITING;
		}
		ret =  POK_ERRNO_OK;
	}
	return ret;
}

#ifdef POK_NEEDS_SCHED_O1
pok_ret_t	pok_thread_delayed_start (uint32_t *id, uint32_t delay_time)
{
	// WARNING: currently works only for the O1 scheduler
	uint32_t thread_id = pok_thread_ids_to_pos_map[*id];
	pok_ret_t ret =  POK_ERRNO_OK;

	if (pok_thread_aperiodic(pok_threads[thread_id]))
	{
		// set the current priority of specified process to its base priority;
		pok_threads[thread_id].current_priority =  pok_threads[thread_id].priority;
	
		// reset context and stack -> Not necessary at this time
	
		// if (operating mode is NORMAL) then		
		// the partition mode is set to NORMAL by the user code after the thread creation, not by the kernel
		if (pok_partitions[pok_threads[thread_id].partition].mode ==  POK_PARTITION_MODE_NORMAL) 
		{
			if (delay_time == 0)
			{
				// set the specified process state to RUNNABLE
				pok_threads[thread_id].state = POK_STATE_RUNNABLE;

				// set the DEADLINE_TIME value for the specified process to (current system clock plus TIME_CAPACITY);
				pok_threads[thread_id].deadline = POK_GETTICK() + pok_threads[thread_id].time_capacity;
			}
			else // DELAY > 0
			{
				//set the specified process state to WAITING
				pok_threads[thread_id].state = POK_STATE_WAITING;
				// set the DEADLINE_TIME value for the specified process to (current system clock plus TIME_CAPACITY plus DELAY_TIME);
				pok_threads[thread_id].deadline = POK_GETTICK() + pok_threads[thread_id].time_capacity + delay_time;
				// initiate a time counter with duration DELAY_TIME;
				pok_sched_set_asynch_event(thread_id, delay_time, POK_EVENT_DELAYED_START);
			}
			// if (preemption is enabled) then
			if (pok_thread_preemption_enabled(pok_threads[thread_id]))
			{
				// ask for process scheduling;
				// The calling process may be preempted
				// commented for RUN TO COMPLETION semantics
				//pok_sched ();	
			}
		}
		else
		{
			// the mode of the partition is COLD_START or WARM_START
			// set the specified process state to WAITING;
			// the specified process will wait until partition become in NORMAL mode
			pok_threads[thread_id].state = POK_STATE_WAITING;
			pok_sched_set_asynch_event(thread_id, delay_time, POK_EVENT_DELAYED_START);
		}
		ret =  POK_ERRNO_OK;
		// At the end of the initialization phase (when the partition mode becomes NORMAL) the process will be set to RUNNABLE
	}else{
		// Periodic process
		// set the current priority of specified process to its base priority;
		pok_threads[thread_id].current_priority =  pok_threads[thread_id].priority;

		// if (operating mode is NORMAL) then
		if (pok_partitions[pok_threads[thread_id].partition].mode ==  POK_PARTITION_MODE_NORMAL)
		{
			// set the specified process state to WAITING;
			pok_threads[thread_id].state = POK_STATE_WAITING;
			// set the first release point of the specified process including the delay time;
			pok_threads[thread_id].next_activation = pok_partitions[pok_threads[thread_id].partition].activation + 
				get_partition_period(pok_threads[thread_id].partition) + delay_time;
			// set the DEADLINE_TIME value for the specified process to (current system clock plus TIME_CAPACITY);
			// specified process has to wait for its release point
			pok_threads[thread_id].deadline = POK_GETTICK() + pok_threads[thread_id].time_capacity + delay_time;
			pok_sched_set_asynch_event(thread_id, delay_time, POK_EVENT_DELAYED_START);
		}else{
			// the mode of the partition is COLD_START or WARM_START)
			// set the specified process state to WAITING;
			// the specified process will wait until partition become in NORMAL mode
			pok_threads[thread_id].state = POK_STATE_WAITING;
			pok_sched_set_asynch_event(thread_id, delay_time, POK_EVENT_DELAYED_START);
		}
		ret =  POK_ERRNO_OK;
	}
	return ret;
}
#endif

pok_ret_t pok_thread_resume (const uint32_t* tid)
{
#ifdef POK_NEEDS_SCHED_O1
	uint32_t thread_pos = pok_thread_ids_to_pos_map[*tid];
	if ((! pok_thread_aperiodic(pok_threads[thread_pos])) || (pok_threads[thread_pos].state != POK_STATE_SUSPENDED))
	{
		printf("WARNING: Invoked RESUME on periodic or non-suspended thread\n");
		return POK_ERRNO_EINVAL;
	}
	pok_threads[thread_pos].state = POK_STATE_RUNNABLE;
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
	/* add the thread bitmask to the tail of the (FIFO) ready queue */
	pok_sched_insert_sporadic_in_queue(1 << (thread_pos - POK_CURRENT_PARTITION.thread_index_low));
	/* update its deadline */
	uint64_t current_time = POK_CLOCK();
	pok_threads[thread_pos].deadline = current_time + pok_threads_deadline[pok_threads[thread_pos].id];
 #else
	POK_CURRENT_PARTITION.runnables |= (1 << (thread_pos - POK_CURRENT_PARTITION.thread_index_low));
 #endif /* POK_NEEDS_SCHED_O1_SPLIT */
 #ifdef POK_NEEDS_DEBUG_O1
	printf("DEBUG_O1::Invoked RESUME on thread %u in position %u\n",*tid,thread_pos);
 #endif
#else /* ! POK_NEEDS_SCHED_O1 */
	pok_threads[*tid].state = POK_STATE_RUNNABLE;
 #ifdef POK_NEEDS_SCHED_FPPS	
	// We just insert it in the ready queue
	insert_in_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[*tid]);
 #endif
#endif /* ! POK_NEEDS_SCHED_O1 */

	return POK_ERRNO_OK;
}

void pok_thread_reset_stack(uint32_t init_stack_addr)
{
	init_stack_addr++;
}

bool_t pok_thread_aperiodic(pok_thread_t pok_thread)
{
	return (pok_thread.period == INFINITE_SYSTEM_TIME_VALUE);
}

bool_t pok_thread_preemption_enabled(pok_thread_t pok_thread)
{
	return (pok_partitions[pok_thread.partition].lock_level == 0);
}

bool_t	pok_thread_is_suspended(pok_thread_t pok_thread)
{ 
	return (pok_thread.suspended || pok_thread.suspended_self);
}

#ifdef POK_NEEDS_SCHED_O1
pok_ret_t pok_thread_sleep (uint32_t time)
{
	// WARNING: currently works only with the O1 scheduler

	if (time == 0)
	{
		// Nothing to be done in O1 implementation
	}
	else
	{
		// Set the current process state to WAITING;
		// Initiate a time counter with duration DELAY_TIME;
		// Ask for process scheduling;
		pok_threads[POK_SCHED_CURRENT_THREAD].state = POK_STATE_WAITING;
		pok_sched_set_asynch_event(POK_SCHED_CURRENT_THREAD, time, POK_EVENT_TIMED_WAIT);
		POK_CURRENT_PARTITION.runnables &= ~(1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low));
		// Reorder asynch events of the partition and activate those that have timed out
		pok_sched_reorder_asynch_list();
		pok_sched_service_asynch_events();
		// elect next thread
		uint32_t elected = POK_CURRENT_PARTITION.sched_func (POK_CURRENT_PARTITION.thread_index_low,
																POK_CURRENT_PARTITION.thread_index_high);
		pok_sched_context_switch(elected);

	}
	return POK_ERRNO_OK;
}
#endif

/*	NOT USED at this time
#ifdef POK_NEEDS_THREAD_SLEEP_UNTIL
pok_ret_t pok_thread_sleep_until (uint32_t time)
{
	pok_sched_lock_current_thread_timed ((uint64_t)time);
	pok_sched ();
	return POK_ERRNO_OK;
}
#endif
*/
pok_ret_t pok_thread_suspend (void)
{
	pok_sched_suspend_self ();
	return POK_ERRNO_OK;
}

pok_ret_t pok_thread_suspend_other(uint32_t tid)
{
	pok_threads[tid].state = POK_STATE_SUSPENDED;
	// Thread tid != current thread
	// No need to reschedule
	return POK_ERRNO_OK;
}


/*	NOT USED at this time
#ifdef POK_NEEDS_ERROR_HANDLING
pok_ret_t pok_thread_restart (const uint32_t tid)
{
	pok_threads[tid].remaining_time_capacity	= pok_threads[tid].time_capacity;
	pok_threads[tid].state						= POK_STATE_WAIT_NEXT_ACTIVATION;
	pok_threads[tid].wakeup_time				= 0;

	
	// At this time, we build a new context for the thread.
	// It is not the best solution but it works at this time
	pok_threads[tid].sp	= pok_space_context_create (	
								pok_threads[tid].partition,
								(uint32_t)pok_threads[tid].entry,
								pok_threads[tid].init_stack_addr,
								0xdead,
								0xbeaf);

	return POK_ERRNO_OK;
}
#endif *//* POK_NEEDS_ERROR_HANDLING */

#endif /* POK_NEEDS_THREADS (whole file) */

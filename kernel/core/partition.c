
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
 * \file partition.c
 * \brief This file provides functions for partitioning services
 *
 * The definition of useful structures can be found in partition.h
 * header file. To enable partitioning services, you must set the
 * POK_NEEDS_PARTITIONS maccro.
 */

#ifdef POK_NEEDS_PARTITIONS

#include <arch.h>
#include <bsp.h>
#include <errno.h>
#include <dependencies.h>
#include <core/sched.h>
#include <core/error.h>
#include <core/debug.h>
#include <core/thread.h>
#include <core/loader.h>
#include <core/partition.h>

#if defined (POK_NEEDS_CONSOLE) || defined (POK_NEEDS_DEBUG)
#include <libc.h>
#endif

#ifdef POK_NEEDS_SCHED_FPPS
extern void insert_in_queue();
#endif

 #ifdef POK_NEEDS_SCHED_O1_SPLIT
extern uint32_t successors_bitmasks[];
 #endif


/**
 * \brief The array that contains ALL partitions in the system.
 */
pok_partition_t pok_partitions[POK_CONFIG_NB_PARTITIONS];


uint8_t	pok_partitions_index = 0;


/**
 **\brief Setup the scheduler used in partition pid
 */
void pok_partition_setup_scheduler (const uint8_t pid)
{
#ifdef POK_CONFIG_PARTITIONS_SCHEDULER
	switch (((pok_sched_t[])POK_CONFIG_PARTITIONS_SCHEDULER)[pid])
	{

#ifdef POK_NEEDS_SCHED_FPPS
		case POK_SCHED_FPPS:
			pok_partitions[pid].sched_func  = &pok_sched_part_fpps;
			break;
#endif

#ifdef POK_NEEDS_SCHED_O1
		case POK_SCHED_O1:
			pok_partitions[pid].sched_func  = &pok_sched_part_o1;
			break;
   #ifdef POK_NEEDS_SCHED_O1_SPLIT
		case POK_SCHED_O1_SPLIT:
			pok_partitions[pid].sched_func  = &pok_sched_part_o1_split;
			break;
   #endif /* end ifdef POK_NEEDS_SCHED_O1_SPLIT */
  #endif /* end ifdef POK_NEEDS_SCHED_O1 */

		// Default scheduling algorithm is Round Robin
	         default:
			pok_partitions[pid].sched_func  = &pok_sched_part_rr;
			break;
	}
#else /* ! POK_CONFIG_PARTITIONS_SCHEDULER */
	pok_partitions[pid].sched_func  = &pok_sched_part_rr;
#endif
}

/* #ifdef POK_NEEDS_ERROR_HANDLING */
/* NOT USED at the moment */
/**
 * \brief Reinitialize a partition from scratch
 *
 * This service is only used when we have to retrieve
 * and handle errors.
 */

/*
void pok_partition_reinit (const uint8_t pid)
{
	uint32_t tmp;

	 // FIXME: reset queueing/sampling ports too

	pok_partition_setup_scheduler (pid);
	pok_partitions[pid].thread_index = 0;
	pok_partitions[pid].current_thread = pok_partitions[pid].thread_index_low;
	pok_partitions[pid].prev_current_thread =  IDLE_THREAD;
	pok_partitions[pid].thread_error = 0;
	pok_partitions[pid].error_status.failed_thread = 0;
	pok_partitions[pid].error_status.failed_addr   = 0;
	pok_partitions[pid].error_status.error_kind    = POK_ERROR_KIND_INVALID;
	pok_partitions[pid].error_status.msg_size      = 0;
	pok_loader_load_partition (pid, pok_partitions[pid].base_addr - pok_partitions[pid].base_vaddr, &tmp);
	pok_partitions[pid].thread_main_entry = tmp;
	pok_partition_setup_main_thread (pid);
}
#endif
*/

/**
 * Setup the main thread of partition with number \a pid
 */
void pok_partition_setup_main_thread (const uint8_t pid)
{
	uint32_t __attribute__((unused)) size ;
	uint32_t main_thread;
	pok_thread_attr_t attr;

	size = pok_partitions[pid].size;
	attr.entry = (uint32_t*)pok_partitions[pid].thread_main_entry;
	attr.priority = 1;
	attr.deadline = 0;
	attr.period   = 0;
	attr.time_capacity = 0;
	pok_partition_thread_create (&main_thread, &attr, pid);
	pok_partitions[pid].thread_main = main_thread;
}

/**
 * \brief Initialize all partitions.
 *
 * It initializes everything, load the program, set thread
 * and lockobjects bounds.
 */
pok_ret_t pok_partition_init ()
{
	uint8_t     i;
	uint32_t    threads_index = 0;

	const uint32_t	partition_size[POK_CONFIG_NB_PARTITIONS] = POK_CONFIG_PARTITIONS_SIZE;
#if defined (POK_CONFIG_PARTITIONS_LOADADDR) && defined (POK_SKIP_LOADER)
	const uint32_t	program_loadaddr[POK_CONFIG_NB_PARTITIONS] = POK_CONFIG_PARTITIONS_LOADADDR;
#endif

#ifdef POK_NEEDS_LOCKOBJECTS
	uint8_t lockobj_index = 0;
#endif

	for (i = 0 ; i < POK_CONFIG_NB_PARTITIONS ; i++)
	{
		uint32_t size = partition_size[i];
		uint32_t base_addr = (uint32_t)pok_bsp_mem_alloc(partition_size[i]);
#ifdef POK_ARCH_PPC
		uint32_t base_vaddr           = pok_space_base_vaddr(base_addr);
#endif
		uint32_t program_entry;
#if defined (POK_CONFIG_PARTITIONS_LOADADDR) && defined (POK_SKIP_LOADER)
		// One may want to check that consistent addresses were specified
		printf ("Partition base addr phys=|%x| (user-defined |%x|)",base_addr, program_loadaddr[i]);
#endif
		pok_partitions[i].base_addr   = base_addr;
		pok_partitions[i].size        = size;
		pok_partitions[i].sched       = POK_SCHED_RR;
		pok_partitions[i].partition_id = i;
#ifdef POK_NEEDS_DEBUG
#ifdef POK_ARCH_PPC
		printf ("[DEBUG] Partition %d loaded at addr virt=|%x|, phys=|%x|\n", i, base_vaddr, base_addr);
#else
		printf ("[DEBUG] Partition %d loaded at addr phys=|%x|\n", i, base_addr);
#endif /*POK_ARCH_PPC*/
#endif /*POK_NEEDS_DEBUG*/
		pok_partition_setup_scheduler (i);

		pok_create_space (i, base_addr, size);

#ifdef POK_ARCH_PPC
		pok_partitions[i].base_vaddr = base_vaddr; /* Set the memory space and so on */
#endif /* POK_ARCH_PPC */
		pok_partitions[i].thread_index_low    = threads_index;
		pok_partitions[i].nthreads            = ((uint32_t[]) POK_CONFIG_PARTITIONS_NTHREADS) [i];

#ifdef POK_NEEDS_ERROR_HANDLING
		if (pok_partitions[i].nthreads <= 1)
		{
			pok_partition_error (i, POK_ERROR_KIND_PARTITION_CONFIGURATION);
		}
#endif

#ifdef POK_CONFIG_PARTITIONS_SCHEDULER
      		pok_partitions[i].sched               = ((pok_sched_t[]) POK_CONFIG_PARTITIONS_SCHEDULER) [i];
#endif

		pok_partitions[i].thread_index_high   = pok_partitions[i].thread_index_low + ((uint32_t[]) POK_CONFIG_PARTITIONS_NTHREADS) [i];
		pok_partitions[i].activation 		= 0;
#ifdef POK_CONFIG_PARTITIONS_PERIOD
		pok_partitions[i].period = ((uint32_t[]) POK_CONFIG_PARTITIONS_PERIOD[i]);
#else
       		pok_partitions[i].period = POK_CONFIG_SCHEDULING_MAJOR_FRAME;
#endif
		pok_partitions[i].thread_index        = 0;
		pok_partitions[i].thread_main         = 0;
		pok_partitions[i].current_thread      = IDLE_THREAD;
		pok_partitions[i].prev_current_thread = IDLE_THREAD;

		threads_index                         = threads_index + pok_partitions[i].nthreads;
		/* Initialize the threading stuff */
		pok_partitions[i].mode                = POK_PARTITION_MODE_INIT_WARM;

#ifdef POK_NEEDS_LOCKOBJECTS
		pok_partitions[i].lockobj_index_low   = lockobj_index;
		pok_partitions[i].lockobj_index_high  = lockobj_index + ((uint8_t[]) POK_CONFIG_PARTITIONS_NLOCKOBJECTS[i]);
		pok_partitions[i].nlockobjs           = ((uint8_t[]) POK_CONFIG_PARTITIONS_NLOCKOBJECTS[i]);
		lockobj_index                         = lockobj_index + pok_partitions[i].nlockobjs;
#endif

#ifdef POK_NEEDS_ERROR_HANDLING
		pok_partitions[i].thread_error                  = 0;
		pok_partitions[i].error_status.failed_thread    = 0;
		pok_partitions[i].error_status.failed_addr      = 0;
		pok_partitions[i].error_status.error_kind       = POK_ERROR_KIND_INVALID;
		pok_partitions[i].error_status.msg_size         = 0;
#endif

#ifndef POK_ARCH_MPPA
		pok_loader_load_partition (i, base_addr, &program_entry);
#else
		/* Partitions are always compiled with a base addr of 0 */
		pok_loader_load_partition (i, 0, &program_entry);
#endif
		/*
		 * Load the partition in its address space
		 */
		pok_partitions[i].thread_main_entry = program_entry;

 #ifdef POK_NEEDS_SCHED_O1_SPLIT
		/* initialize the successors bitmask with all zeros; we set to 1 the bits
		 * corresponding to successor threads when the partition changes its mode to NORMAL.
		 */
		pok_partitions[i].successors = 0;
		/* initialize the executed_predecessors with all 1s; this because a thread that is
		 * not a successor has its bit set to 1 in this bitmask (i.e. it can always execute).
		 * We set to 0 the bits corresponding to successor threads when the partition change
		 * its mode to NORMAL.
		 */
		pok_partitions[i].executed_predecessors = 1;
		/* initialize head and tail indexes of sporadic threads FIFO queue */
		pok_partitions[i].head_index = 0;
		pok_partitions[i].tail_index = -1;
		/* initialize the FIFO queue; the queue is initialized with all bitmasks corresponding
		 * to the idle thread
		 */
		int sporadic_index = 0;
		for (;sporadic_index<POK_CONFIG_NB_SPORADIC_THREADS;sporadic_index++)
		{
			pok_partitions[i].sporadic_ready_queue[sporadic_index] = 1 << (IDLE_THREAD - pok_partitions[i].thread_index_low);
		}
 #endif /* end ifdef POK_NEEDS_SCHED_O1_SPLIT */

	} // end for

	for (i = 0 ; i < POK_CONFIG_NB_PARTITIONS ; i++)
	{
#if POK_NEEDS_DEBUG
		printf("[DEBUG] Setting up main thread of partition: %d, addr: %x\n",
					i, pok_partitions[i].thread_main_entry);
#endif
		pok_partition_setup_main_thread (i);
	}

	return POK_ERRNO_OK;
}


/**
 * Change the current mode of the partition. Possible mode
 * are describe in core/partition.h. Returns
 * POK_ERRNO_PARTITION_MODE when requested mode is invalid.
 * Else, returns POK_ERRNO_OK
 */

pok_ret_t pok_partition_set_mode (const uint8_t pid, const pok_partition_mode_t mode)
{
	pok_ret_t ret = POK_ERRNO_OK;
	uint32_t thread_id;
  uint32_t indexLow, indexHigh;

	switch (mode)
	{
		case POK_PARTITION_MODE_NORMAL:
#ifdef POK_NEEDS_DEBUG
   printf ("SET_PARTITON_MODE : partition[%d] MODE = NORMAL  \n", pid );
#endif

			indexLow  = pok_partitions[pid].thread_index_low;
			indexHigh = pok_partitions[pid].thread_index_high;
			// Update the thread states pok_threads in indexLow - indexHigh (excluded)
			for (thread_id = indexLow ; thread_id < indexHigh ; thread_id++)
			{
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
				/* idle thread is always ready in the periodic threads queue */
				pok_partitions[pid].runnables |= (1 << (IDLE_THREAD - pok_partitions[pid].thread_index_low));
 #endif
				// Check thread is not idle, kernel or (partition main) current thread and each thread belongs to the current partition
				if ((thread_id != IDLE_THREAD) && (thread_id != KERNEL_THREAD) && (thread_id != POK_CURRENT_PARTITION.current_thread)
					&& !pok_thread_is_suspended(pok_threads[thread_id])
					&& pok_threads[thread_id].state == POK_STATE_WAITING) { // the thread has been started

 #ifdef POK_NEEDS_SCHED_O1_SPLIT
					/* if the thread is a successor set its bit in the successor bitmask and
	 				 * reset its bit in the executed_predecessors bitmask
	 				 */
					if ( ((bool_t[])POK_CONFIG_SUCCESSOR_THREADS)[pok_threads[thread_id].id] == TRUE)
					{
						pok_partitions[pid].successors |= (1 << (pok_threads[thread_id].pos - pok_partitions[pid].thread_index_low));
						pok_partitions[pid].executed_predecessors &=
									~(1 << (pok_threads[thread_id].pos - pok_partitions[pid].thread_index_low));
					}
					/* if the thread is a predecessor set the bitmask corresponding to its successor in the successors_bitmasks */
					if (((bool_t[])POK_CONFIG_PREDECESSOR_THREADS)[pok_threads[thread_id].id] == TRUE)
					{
						int successor_id = ((int[])POK_CONFIG_SUCCESSORS_ID)[pok_threads[thread_id].id] + pok_partitions[pid].thread_index_low;
						successors_bitmasks[pok_threads[thread_id].id] =
									(1 << (pok_thread_ids_to_pos_map[successor_id] - pok_partitions[pid].thread_index_low));
					}
 #endif /* end ifdef POK_NEEDS_SCHED_O1_SPLIT */

					if (pok_thread_aperiodic(pok_threads[thread_id])){
 #ifdef POK_NEEDS_SCHED_O1
  #ifdef POK_NEEDS_SCHED_O1_SPLIT
						/* set the thread bit in the sporadic_mask bitmask */
						pok_partitions[pid].sporadic_bitmask |= (1 << (pok_threads[thread_id].pos - pok_partitions[pid].thread_index_low));
  #endif
						if(pok_threads[thread_id].timeout != POK_NULL)
							// initiate a time counter with duration DELAY_TIME;
							pok_sched_set_asynch_event(thread_id,
										pok_threads[thread_id].timeout->timer,
										POK_EVENT_DELAYED_START);
						else
						{
  #ifdef POK_NEEDS_SCHED_O1_SPLIT
							/* insert the thread bitmask in the ready (FIFO) queue;
							 * this because at the beginning all sporadic threads are ready */
							pok_sched_insert_sporadic_in_queue(1 << (pok_threads[thread_id].pos - pok_partitions[pid].thread_index_low));
  #else /* POK_NEEDS_SCHED_O1_SPLIT is not defined */
							// set to READY all previously started (not delayed) aperiodic processes (unless the process was suspended);
							pok_partitions[pid].runnables |= (1 << (pok_threads[thread_id].pos -
												pok_partitions[pok_threads[thread_id].partition].thread_index_low));
  #endif /* end ifdef POK_NEEDS_SCHED_O1_SPLIT */
 #endif /* end ifdef POK_NEEDS_SCHED_O1 */
							pok_threads[thread_id].state = POK_STATE_RUNNABLE;
 #ifdef POK_NEEDS_SCHED_FPPS
							insert_in_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[thread_id]);
 #endif

 #ifdef POK_NEEDS_SCHED_O1
						}
 #endif
					}else{
						// periodic thread
						// set first release points of all previously started (not delayed) periodic processes to
						// their next partition period;
						// The partition period is unused
						// check if  the next activation is set to the beginning of the next time slot of the partition
						pok_threads[thread_id].next_activation = pok_partitions[pok_threads[thread_id].partition].activation
								+ get_partition_period(pok_threads[thread_id].partition);
 #ifdef POK_NEEDS_SCHED_O1

						if(pok_threads[thread_id].timeout != POK_NULL)
						{
							pok_threads[thread_id].next_activation += pok_threads[thread_id].timeout->timer;
							pok_sched_set_asynch_event(thread_id,pok_threads[thread_id].next_activation,POK_EVENT_DELAYED_START);
						}
						else
						{
 #endif
							pok_threads[thread_id].state = POK_STATE_WAIT_NEXT_ACTIVATION;
 #ifdef POK_NEEDS_SCHED_O1
							uint32_t the_mask = (1 << (pok_threads[thread_id].pos - pok_partitions[pok_threads[thread_id].partition].thread_index_low));

							uint32_t time, limit, period;
							limit = ((uint32_t)pok_partitions[pok_threads[thread_id].partition].activation) + POK_CONFIG_SCHEDULING_MAJOR_FRAME;
							period = get_partition_period(pok_threads[thread_id].partition);
							for (time =pok_threads[thread_id].next_activation; time <= limit; time += period)
							{
								int position = time % POK_CONFIG_SCHEDULING_MAJOR_FRAME;
								masks[position][POK_STATE_RUNNABLE] |= the_mask;
  #ifdef POK_NEEDS_DEBUG_O1
								printf ("Thread %d will be next activated at %d (= %d + %d * k)\n",
									thread_id, time,
									(int)pok_partitions[pok_threads[thread_id].partition].activation,
									period);
								printf("Adding to mask %d.. %u\n",position,the_mask);
  #endif
							}
						}
 #endif
					}
					// calculate the deadline time of all no dormant processe in the partition
					if (pok_threads[thread_id].state != POK_STATE_STOPPED){
						pok_threads[thread_id].end_time = pok_threads[thread_id].next_activation + pok_threads[thread_id].time_capacity;
					}
				}
			}// end for

			// set the partition lock level to 0
			pok_partitions[pid].lock_level = 0;

			// setting the partition mode
			pok_partitions[pid].mode = mode;

			// stop the calling thread
			pok_sched_stop_thread(pok_partitions[pid].thread_main);

			// activate the process scheduling
#ifdef POK_NEEDS_SCHED_O1
			pok_sched_end_period();
#else
			pok_sched();
#endif

		break;

		default:
			return POK_ERRNO_PARTITION_MODE;
			break;
	} // case

	return ret;
}


/**
 * Change the mode of the current partition (the partition being executed)
 */
pok_ret_t pok_partition_set_mode_current (const pok_partition_mode_t mode)
{

#ifdef POK_NEEDS_ERROR_HANDLING
	if ((POK_SCHED_CURRENT_THREAD != POK_CURRENT_PARTITION.thread_main) &&
		(POK_SCHED_CURRENT_THREAD != POK_CURRENT_PARTITION.thread_error))
#else
	if (POK_SCHED_CURRENT_THREAD != POK_CURRENT_PARTITION.thread_main)
#endif
	{
		return POK_ERRNO_THREAD;
	}
	/*
	 * Here, we check which thread call this function.
	 * In fact, only two threads can change the partition mode : the init thread
	 * and the error thread. If ANY other thread try to change the partition
	 * mode, this is an error !
	 */
	return (pok_partition_set_mode (POK_SCHED_CURRENT_PARTITION, mode));
}



/**
 * Stop a thread inside a partition.
 * The \a tid argument is relative to the partition, meaning
 * that it corresponds to a number which bounds are
 * 0 .. number of tasks inside the partition.
 */
pok_ret_t pok_partition_stop_thread (const uint32_t tid)
{
	if (POK_SCHED_CURRENT_THREAD != POK_CURRENT_PARTITION.thread_error)
	{
		return POK_ERRNO_THREAD;
	}

	/*
	 * We check which thread try to call this function. Only the error handling
	 * thread can stop other threads.
	 */
	pok_sched_stop_thread (tid + POK_CURRENT_PARTITION.thread_index_low);
	return (POK_ERRNO_OK);
}


#endif /* POK_NEEDS_PARTITIONS (whole file) */

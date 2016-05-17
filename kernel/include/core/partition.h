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
 * \file	 partition.h
 * \brief	Definition of structure for partitioning services.
 */

#ifndef __POK_PARTITION_H__
#define __POK_PARTITION_H__

#ifdef POK_NEEDS_PARTITIONS

#include <types.h>
#include <errno.h>
#include <core/error.h>
#include <core/thread.h>
#include <core/sched.h>


#ifdef POK_NEEDS_SCHED_O1_SPLIT
/* inclusion from thread.c */
extern uint32_t pok_thread_ids_to_pos_map[POK_CONFIG_NB_THREADS];
#endif

/**
 * \enum pok_partition_mode_t
 * \brief The different modes of a partition
 */
typedef enum
{
	POK_PARTITION_MODE_INIT_COLD = 1, /**< Partition is in cold mode (init mode),
													only the main thread is active */
	POK_PARTITION_MODE_INIT_WARM = 2, /**< Partition is in warm init mode,
													only the main thread is active */
	POK_PARTITION_MODE_NORMAL	 = 3, /**< Partition is currently being executed,
													main thread is disabled, other threads
													are running
												 */
	POK_PARTITION_MODE_IDLE		= 4, /**< Idle mode, the partition is not executed */
	POK_PARTITION_MODE_RESTART	= 5, /**< Currently being restarted */
	POK_PARTITION_MODE_STOPPED	= 6, /**< Partition is stopped, it will no longer be executed */
}pok_partition_mode_t;

/*!
 * \struct pok_partition_t
 * \brief This structure contains all needed information for partition management
 */
typedef struct
{

	uint32_t	partition_id;
	uint32_t	base_addr;	/**< The base address inside the whole memory (where the segment is in the whole memory ?) */
#ifdef POK_ARCH_PPC
	uint32_t	base_vaddr;	/**< The virtual address of the partition. The address the threads sees when they are
												 executed */
#endif /*POK_ARCH_PPC*/
	uint32_t	size;		/**< Size of the allocated memory segment */

	const char	*name;		/**< Name of the partition */

	uint32_t	nthreads;	/**< Number of threads inside the partition */

	uint8_t		priority;	/**< Priority of the partition (unused at this time */
	uint32_t	period;		/**< Period of the partition, unused at this time */

	pok_sched_t	sched;		/**< The associated for the partition to schedule its threads */

	uint32_t (*sched_func)(uint32_t low, uint32_t high); /**< Scheduling function to scheduler threads */

	uint64_t	activation; /**< Last activation time of the partition */
	uint32_t	prev_current_thread;/**< member for the scheduler (previous scheduled thread inside the partition */
	uint32_t	current_thread;		/**< member for the scheduler (current executed thread inside the partition */

	uint32_t	thread_index_low;	/**< The low index in the threads table */
	uint32_t	thread_index_high;	/**< The high index in the threads table */
	uint32_t	thread_index;		/**< The thread index */

#if defined(POK_NEEDS_LOCKOBJECTS) || defined(POK_NEEDS_ERROR_HANDLING)
	uint8_t		lockobj_index_low;	/**< The low bound in the lockobject array. */
	uint8_t		lockobj_index_high; /**< The high bound in the lockobject array */
	uint8_t		nlockobjs;			/**< The amount of lockobjects reserved for the partition */
#endif

#ifdef POK_NEEDS_SCHED_HFPPS
	uint64_t		payback; /**< Payback for HFPPS scheduling algorithm */
#endif /* POK_NEEDS_SCHED_HFPPS */

//#ifdef POK_NEEDS_ERROR_HANDLING
	uint32_t		thread_error;		/**< The thread identifier used for error handling */
	pok_error_status_t	error_status;	/**< A pointer used to store information about errors */
//#endif
	uint32_t		thread_main;		/**< The thread identifier of the main thread (initialization thread) */
	uint32_t		thread_main_entry;	/**< The entry-point of the main thread (useful for re-init) */
	pok_partition_mode_t 	mode;			/**< Current mode of the partition */
	uint32_t		lock_level;

#ifdef POK_NEEDS_SCHED_O1
	uint32_t		runnables;
	// *** ASYNCH EVENTS QUEUES *** //
	pok_sched_asynch_event_t* head_asynch_empty;
	pok_sched_asynch_event_t* head_asynch_queue;
	pok_sched_asynch_event_t* head_asynch_temporary;
#endif

#ifdef POK_NEEDS_SCHED_O1_SPLIT
  /* Bitmask used to store which threads are successors; it is ordered like runnebles bitmask */
  uint32_t		successors;
  /* Bitmask used to store which predecessor threads have executed; it is used by
	* successor threads to know if their predecessor has executed
	*/
  uint32_t		executed_predecessors;
  /* Array of bitmask used to implement the ready (FIFO) queue for sporadic threads */
  uint32_t		sporadic_ready_queue[POK_CONFIG_NB_SPORADIC_THREADS];
  uint32_t		head_index;
  uint32_t		tail_index;
  /* a bitmask that say which threads are sporadic */
  uint32_t		sporadic_bitmask;
#endif
} pok_partition_t;

extern pok_partition_t pok_partitions[POK_CONFIG_NB_PARTITIONS];

/**
 * Access to the current partition variable.
 * With that, you can do POK_CURRENT_PARTITION.nthreads of POK_CURRENT_PARTITION.mode
 * It avoids tedious syntax like pok_partitions[my_partition].blablabla
 */
#define POK_CURRENT_PARTITION pok_partitions[POK_SCHED_CURRENT_PARTITION]

/**
 * Chech that pointer \a ptr is located in the address space of partition
 * \a pid
 */
#define POK_CHECK_PTR_IN_PARTITION(pid,ptr) (\
	((((uint32_t)ptr)>=pok_partitions[pid].base_addr)&& \
	(((uint32_t)ptr)<=(pok_partitions[pid].base_addr+pok_partitions[pid].size)))?1:0\
)

/**
 * Initialize all partitions
 */
pok_ret_t pok_partition_init();

pok_ret_t pok_partition_set_mode (const uint8_t pid, const pok_partition_mode_t mode);
pok_ret_t pok_partition_set_mode_current (const pok_partition_mode_t mode);

pok_ret_t pok_partition_stop_thread (const uint32_t tid);

void pok_partition_reinit (const uint8_t);

void pok_partition_setup_main_thread (const uint8_t);

void pok_partition_setup_scheduler (const uint8_t pid);

#endif /* __POK_NEEDS_PARTITIONS */

#endif /* __POK_PARTITION_H__ */

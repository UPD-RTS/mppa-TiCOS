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
 ** \file   sched.c
 ** \brief  Function for partitions and kernel scheduling
 */

#if defined (POK_NEEDS_SCHED) || defined (POK_NEEDS_THREADS)

#include <types.h>
#include <arch.h>
#include <bsp.h>

#include <core/time.h>
#include <core/sched.h>
#include <core/thread.h>

#ifdef POK_NEEDS_PARTITIONS
#include <core/partition.h>
#endif

#if defined (POK_NEEDS_PORTS_QUEUEING) || defined (POK_NEEDS_PORTS_SAMPLING)
#include <middleware/port.h>
#endif

#ifdef POK_NEEDS_SCHED_O1_SPLIT
#include <core/lockobj.h>
#endif

#include <dependencies.h>

#include <libc.h>
#include <core/debug.h>
#include <core/error.h>
#include <arch.h>

#ifdef POK_NEEDS_SCHED_O1_SPLIT
extern uint32_t successors_bitmasks[];
extern bool_t has_tardiness[];
extern pok_lockobj_t pok_partitions_lockobjs[];
extern uint64_t pok_threads_deadline[];
extern uint8_t predecessors_events_id[];
#endif

extern pok_thread_t pok_threads[];

#ifdef POK_NEEDS_PARTITIONS
extern pok_partition_t  pok_partitions[];

/**
 * \brief The variable that contains the value of partition currently being executed
 */
uint8_t pok_current_partition;

void pok_sched_partition_switch();
#endif

#if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)
extern void pok_port_flushall (void);
extern void pok_slot_read_prefetch (uint8_t pid);
extern void pok_slot_write_flush (uint8_t pid);
#endif

/* Indicates how long every partition should execute */
//uint64_t pok_sched_slots[POK_CONFIG_SCHEDULING_NBSLOTS]            = (uint64_t[]) POK_CONFIG_SCHEDULING_SLOTS;
uint64_t pok_sched_slots[POK_CONFIG_SCHEDULING_NBSLOTS]            = POK_CONFIG_SCHEDULING_SLOTS;
//uint8_t  pok_sched_slots_allocation[POK_CONFIG_SCHEDULING_NBSLOTS] = (uint8_t[]) POK_CONFIG_SCHEDULING_SLOTS_ALLOCATION;
uint8_t  pok_sched_slots_allocation[POK_CONFIG_SCHEDULING_NBSLOTS] = POK_CONFIG_SCHEDULING_SLOTS_ALLOCATION;
#ifdef POK_NEEDS_SCHED_O1
//uint8_t  pok_asynch_events[POK_CONFIG_NB_PARTITIONS] = ((uint8_t[])POK_CONFIG_PARTITIONS_NB_ASYNCH_EVENTS);
uint8_t  pok_asynch_events[POK_CONFIG_NB_PARTITIONS] = POK_CONFIG_PARTITIONS_NB_ASYNCH_EVENTS;
 #if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)
uint8_t  pok_postwrite_times[POK_CONFIG_SCHEDULING_NBSLOTS] = POK_CONFIG_PARTITIONS_POSTWRITE_TIME;
 #endif
#endif

uint32_t prev_current_thread;
pok_sched_t pok_global_sched;
uint64_t pok_sched_next_deadline;
uint64_t pok_sched_next_major_frame;
int pok_sched_current_slot = 0; /* Which slot are we executing at this time ?*/
uint32_t current_thread = KERNEL_THREAD;

/**
 * Pointers to the current executing thread's context
 * and the next executing threads context
 */
#ifdef POK_ARCH_MPPA
uint32_t pok_current_context __attribute__((used));
#endif

#ifdef POK_NEEDS_SCHED_FPPS
void insert_in_queue (uint16_t, pok_thread_t*);
void remove_from_queue (uint16_t, pok_thread_t*);
#endif

/**
 * \brief Init scheduling service
 */
void pok_sched_init (void)
{
#ifdef POK_NEEDS_PARTITIONS
 #if defined (POK_NEEDS_ERROR_HANDLING) || defined (POK_NEEDS_DEBUG)
	/*
	 * We check that the total time of time frame
	 * corresponds to the sum of each slot
	 */
	uint64_t total_time;
	uint8_t slot;

	total_time = 0;

	for (slot = 0 ; slot < POK_CONFIG_SCHEDULING_NBSLOTS ; slot++)
	{
		total_time = total_time + pok_sched_slots[slot];
	}

	if (total_time != POK_CONFIG_SCHEDULING_MAJOR_FRAME)
	{
  #ifdef POK_NEEDS_DEBUG
		printf ("Major frame is not compliant with all time slots\n");
  #endif
  #ifdef POK_NEEDS_ERROR_HANDLING
		pok_kernel_error (POK_ERROR_KIND_KERNEL_CONFIG);
  #endif
	}
 #endif /* POK_NEEDS_ERROR_HANDLING) || defined (POK_NEEDS_DEBUG) */
#endif /* POK_NEEDS_PARTITIONS */

	pok_sched_current_slot        = 0;
	/* POK_CONFIG_SCHEDULING_MAJOR_FRAME = sum of all scheduling slots */
	pok_sched_next_major_frame    = POK_CONFIG_SCHEDULING_MAJOR_FRAME;
	pok_sched_next_deadline       = pok_sched_slots[0];		// start to execute the first partition
	pok_current_partition         = pok_sched_slots_allocation[0];

#ifdef POK_NEEDS_SCHED_O1
	uint32_t i;
	for(i=0; i<POK_CONFIG_NB_THREADS; i++)
		pok_thread_ids_to_pos_map[i]=i;
	// *************************** ASYNCH EVENTS QUEUES **************************************** //
	// adjust initial slot and major frame values to work with interval timer scheduling (start-up)
	pok_sched_current_slot--;
	pok_sched_next_major_frame++;

	uint8_t part=0;
	uint8_t index=0;
	uint8_t index_low=0;
	uint8_t counter;
	while (part < POK_CONFIG_NB_PARTITIONS)
	{
		index = pok_asynch_events[part];

		//init pointers of queue elements
		for (counter = index_low; counter < index_low + index; counter++)
		{
			if (counter -1 >= index_low)
				asynch_queue[counter].previous = &(asynch_queue[counter-1]);
			else
				asynch_queue[counter].previous = POK_NULL;

			if (counter +1 < index)
				asynch_queue[counter].next = &(asynch_queue[counter+1]);
			else
				asynch_queue[counter].next = POK_NULL;
		}
		if (index > 0)
		{
			pok_partitions[part].head_asynch_queue = POK_NULL;
			pok_partitions[part].head_asynch_empty = &(asynch_queue[index_low]);
			pok_partitions[part].head_asynch_temporary = POK_NULL;
		}
 #ifdef POK_NEEDS_DEBUG_O1
		printf("DEBUG_O1::QUEUE for partition %d starts from index %d\n",part,index_low);
		print_queue(pok_partitions[part].head_asynch_empty);
 #endif

		index_low += index;
		part++;
	}
#endif
}

#ifdef POK_NEEDS_DEBUG_O1
void print_queue(pok_sched_asynch_event_t* head)
{
	pok_sched_asynch_event_t* current_asynch = head;
	while (current_asynch != POK_NULL){
		print_long(current_asynch->timestamp);
		printf(" -> ");
		current_asynch = current_asynch->next;
	}
	printf("END_QUEUE\n");
}
#endif

#ifdef POK_NEEDS_SCHED_O1
void pok_sched_set_asynch_event(uint32_t thread_id, uint64_t time, pok_event_type_t type)
{
	if ((pok_threads[thread_id].timeout != POK_NULL) && (pok_threads[thread_id].timeout->type == POK_EVENT_DELAYED_START))
	{
		// overwrite of the previously created DELAYED_START event on NORMAL partition mode
		pok_threads[thread_id].timeout-> timestamp = time;
 #ifdef POK_NEEDS_DEBUG_O1
		printf("DEBUG_O1::UPDATED ASYNCH EVENT: thread %d to be activated at time ",thread_id);print_long(pok_threads[thread_id].timeout-> timestamp);printf("\n");
 #endif
		return;
	}
	uint64_t now = POK_GETTICK();
	pok_sched_asynch_event_t* new_event = POK_CURRENT_PARTITION.head_asynch_empty;
	uint32_t the_mask = (1 << (pok_threads[thread_id].pos - pok_partitions[pok_threads[thread_id].partition].thread_index_low));
	new_event->pos = thread_id;
	new_event->timer = time;
	new_event->timestamp = now + time;
	new_event->mask = the_mask;
	new_event->type = type;
	if (new_event->next != POK_NULL)
		new_event->next->previous = POK_NULL;
	POK_CURRENT_PARTITION.head_asynch_empty = new_event->next;
	// add to temporary queue
	new_event->next = POK_CURRENT_PARTITION.head_asynch_temporary; //insert in head
	if (new_event->next != POK_NULL)
		new_event->next->previous = new_event;
	POK_CURRENT_PARTITION.head_asynch_temporary = new_event;
	pok_threads[thread_id].timeout = new_event;

/*#include <stddef.h>
printf("Sizeof my struct: %x (%x + %x + %x + %x + %x)\n",sizeof(pok_sched_asynch_event_t),sizeof(uint64_t),sizeof(uint64_t),sizeof(uint32_t),sizeof(pok_sched_asynch_event_t*),sizeof(pok_sched_asynch_event_t*));
printf("Sizeof my enum: %x\n",sizeof(pok_event_type_t));
printf("Addresses: struct: %x timer: %x time: %x mask: %x previous: %x next: %x\n",&(POK_CURRENT_PARTITION.head_asynch_queue),&(POK_CURRENT_PARTITION.head_asynch_queue->timer),&(POK_CURRENT_PARTITION.head_asynch_queue->timestamp),&(POK_CURRENT_PARTITION.head_asynch_queue->mask),&(POK_CURRENT_PARTITION.head_asynch_queue->previous),&(POK_CURRENT_PARTITION.head_asynch_queue->next));
printf("Offsets: %d %d %d %d %d\n", offsetof(pok_sched_asynch_event_t, timer),offsetof(pok_sched_asynch_event_t, timestamp),offsetof(pok_sched_asynch_event_t, mask),offsetof(pok_sched_asynch_event_t, previous),offsetof(pok_sched_asynch_event_t, next));
printf("TIME: ");print_long(time);printf("\tNOW+TIME: ");print_long(now+time);printf("\tTIMER: ");print_long(new_event->timer);printf("\tTIMESTAMP: ");print_long(new_event->timestamp);printf("\tMASK: %d",new_event->mask);printf("\n");
//printf("by byte: %d %d\n",(uint32_t)(((new_event->timestamp) << 32)>>32),(uint32_t)(((new_event->timestamp) >> 32)<<32));*/
 #ifdef POK_NEEDS_DEBUG_O1
	if (POK_CURRENT_PARTITION.head_asynch_empty != POK_NULL ||
	POK_CURRENT_PARTITION.head_asynch_temporary != POK_NULL ||
	POK_CURRENT_PARTITION.head_asynch_queue != POK_NULL)
	{
		printf("**********************************************************************\n");
		printf("DEBUG_O1::CREATED ASYNCH EVENT: thread %d to be activated at time ",thread_id);print_long(new_event->timestamp);printf("\n");
		printf("** Empty queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_empty);
		printf("** Temporary queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_temporary);
		printf("** Actual queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_queue);
		printf("**********************************************************************\n");
	}
 #endif
}

void pok_sched_reorder_asynch_list()
{
	pok_sched_asynch_event_t* current_temporary = POK_CURRENT_PARTITION.head_asynch_temporary;
	pok_sched_asynch_event_t* current_asynch = POK_CURRENT_PARTITION.head_asynch_queue;
	uint64_t current_timestamp;
	while (current_temporary != POK_NULL)
	{
		current_timestamp = current_temporary->timestamp;
		pok_sched_asynch_event_t* prev = POK_NULL;
		while (current_asynch != POK_NULL && current_asynch->timestamp < current_timestamp)
		{
			prev = current_asynch;
			current_asynch = current_asynch->next;
		}
		current_temporary->previous = prev;
		current_temporary->next = current_asynch;
		if (prev != POK_NULL)
			prev->next = current_temporary;
		else
			POK_CURRENT_PARTITION.head_asynch_queue = current_temporary; //update head of queue
		if (current_asynch != POK_NULL)
			current_asynch->previous = current_temporary;

		current_temporary = current_temporary->next;
	}
	POK_CURRENT_PARTITION.head_asynch_temporary = POK_NULL;
 #ifdef POK_NEEDS_DEBUG_O1
	if (POK_CURRENT_PARTITION.head_asynch_empty != POK_NULL
		|| POK_CURRENT_PARTITION.head_asynch_temporary != POK_NULL
		|| POK_CURRENT_PARTITION.head_asynch_queue != POK_NULL)
	{
		printf("**********************************************************************\n");
		printf("DEBUG_O1::REORDERED ASYNCH QUEUES:\n");
		printf("** Empty queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_empty);
		printf("** Temporary queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_temporary);
		printf("** Actual queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_queue);
		printf("**********************************************************************\n");
	}
 #endif
}

void pok_sched_service_asynch_events()
{
	uint64_t now = POK_GETTICK();
	pok_sched_asynch_event_t* current_asynch = POK_CURRENT_PARTITION.head_asynch_queue;
	while (current_asynch != POK_NULL && current_asynch->timestamp <= now)
	{
		POK_CURRENT_PARTITION.runnables |= current_asynch->mask;
		POK_CURRENT_PARTITION.head_asynch_queue = current_asynch->next;
		if (pok_threads[current_asynch->pos].timeout->type == POK_EVENT_DELAYED_START)
		{
			current_asynch->next = POK_CURRENT_PARTITION.head_asynch_temporary; //put the event back in the (head of) temporary queue
			if (POK_CURRENT_PARTITION.head_asynch_temporary != POK_NULL)
				POK_CURRENT_PARTITION.head_asynch_temporary->previous = current_asynch;
			POK_CURRENT_PARTITION.head_asynch_temporary = current_asynch;
		}
		else
		{
			current_asynch->next = POK_CURRENT_PARTITION.head_asynch_empty; //put the event in the (head of) empty queue
			if (POK_CURRENT_PARTITION.head_asynch_empty != POK_NULL)
				POK_CURRENT_PARTITION.head_asynch_empty->previous = current_asynch;
			POK_CURRENT_PARTITION.head_asynch_empty = current_asynch;
			pok_threads[current_asynch->pos].timeout = POK_NULL;
			current_asynch->timer =0;
			current_asynch->timestamp =0;
			current_asynch->mask =0;
			current_asynch->pos =0;
		}
 #ifdef POK_NEEDS_DEBUG_O1
		if (POK_CURRENT_PARTITION.head_asynch_empty != POK_NULL || POK_CURRENT_PARTITION.head_asynch_temporary != POK_NULL || POK_CURRENT_PARTITION.head_asynch_queue != POK_NULL)
		{
			printf("**********************************************************************\n");
			printf("DEBUG_O1::SERVICE ASYNCH EVENT: thread %d (to be activated at ");
			print_long(current_asynch->timestamp);printf(") has been activated at time ",current_asynch->pos);print_long(now);printf("\n");
			printf("** Empty queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_empty);
			printf("** Temporary queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_temporary);
			printf("** Actual queue: ");print_queue(POK_CURRENT_PARTITION.head_asynch_queue);
			printf("**********************************************************************\n");
		}
 #endif
		current_asynch = POK_CURRENT_PARTITION.head_asynch_queue;
	}
}

/*
	// TODO: TO BE IMPLEMENTED
void pok_sched_cancel_asynch_event(uint32_t thread_id)
{
	// printf("%d",thread_id);
}
*/
#endif /* POK_NEEDS_SCHED_O1 */

// *************************** END ASYNCH EVENTS QUEUES *************************** //

# if defined (POK_NEEDS_ARINC653)
uint32_t get_partition_period (uint8_t partition_id)
{
// corresponds to the major frame or to the partition period if defined in the deployment.h file
	return pok_partitions[partition_id].period;

}
#endif

uint8_t pok_sched_get_priority_min (const pok_sched_t sched_type)
{
	(void) sched_type;
	return 0;
}

uint8_t pok_sched_get_priority_max (const pok_sched_t sched_type)
{
	(void) sched_type;
#ifdef POK_CONFIG_PRIORITY_LEVELS
	return POK_CONFIG_PRIORITY_LEVELS -1;
#else
	return 255;
#endif
}

#ifdef POK_NEEDS_PARTITIONS
/* Elects next partition to be executed */
pok_partition_t*	pok_elect_partition(uint64_t time)
{
	pok_bool_t switched __attribute__((unused)) = FALSE;
	uint64_t now = time;
	/* End of the curently executing partition slot */

	pok_sched_current_slot = (pok_sched_current_slot + 1) % POK_CONFIG_SCHEDULING_NBSLOTS;

	if (pok_sched_next_major_frame <= now)
	{
		switched = TRUE;
		pok_sched_next_major_frame = pok_sched_next_major_frame + POK_CONFIG_SCHEDULING_MAJOR_FRAME;
 #ifdef POK_NEEDS_SCHED_O1
		start_of_MAF = now;
 #endif
 #ifdef POK_NEEDS_DEBUG_O1
		printf("*** MAJOR FRAME ***\n");
		printf("pok_sched_next_major_frame %d\n",pok_sched_next_major_frame);
//		printf("DEBUG_O1::Start of MAF: ");print_long(start_of_MAF);printf("\n");
 #endif

 # if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)
		// Convey data from SOURCE ports to DESTINATION ports
		pok_port_flushall();
 #endif
  	}

	/* Sets the next partition switch instant */
	pok_sched_next_deadline = pok_sched_next_deadline + pok_sched_slots[pok_sched_current_slot];

 #ifdef POK_NEEDS_DEBUG
	printf ("Switch from partition %d to partition %d\n", pok_current_partition, pok_sched_current_slot);
 #endif

	pok_partitions[pok_current_partition].prev_current_thread = pok_partitions[pok_current_partition].current_thread;
	pok_partitions[pok_current_partition].current_thread = POK_SCHED_CURRENT_THREAD;

	// Switch current partition to elected new partition.
	pok_current_partition = pok_sched_slots_allocation[pok_sched_current_slot];

	// set the current activation time of the partition -- used to calculate the process first release point
	pok_partitions[pok_current_partition].activation = now; // i.e. POK_GETTICK();
 #ifdef POK_NEEDS_DEBUG_O1
	printf("DEBUG_O1::Slot: %d, Current partition: %d\n",pok_sched_current_slot,pok_current_partition);
	printf("DEBUG_O1::Setting partition %d activation to ",pok_current_partition);print_long(now);printf("\n");
 #endif

 # if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)
	// Prefetch reads after flush (new partition)
	pok_slot_read_prefetch (pok_current_partition);
 #endif


 #ifdef POK_NEEDS_SCHED_O1
	// Reorder asynch events of the partition and activate those that have timed out
	pok_sched_reorder_asynch_list();
	pok_sched_service_asynch_events();
 #endif

//#endif  /* POK_CONFIG_NB_PARTITIONS > 1 */

	return (&(pok_partitions[pok_current_partition]));
}

uint32_t pok_elect_thread (pok_partition_t* current_partition, uint64_t now)
{
	// We elect the thread to be executed
	uint32_t 		elected;
 #ifdef POK_NEEDS_SCHED_O1
	uint32_t moment;
 #else
	uint8_t		i;
 #endif

	// Update of thread state only when the partition is in NORMAL mode
	switch (current_partition->mode)
	{
		case POK_PARTITION_MODE_INIT_COLD:
		case POK_PARTITION_MODE_INIT_WARM:
 #ifdef POK_NEEDS_ERROR_HANDLING
			if ((current_partition->thread_error != 0) &&
				 (pok_threads[current_partition->thread_error].state != POK_STATE_STOPPED))
			{
				elected = current_partition->thread_error;
			}
			else
			{
				elected = current_partition->thread_main;
			}
 #else
			elected = current_partition->thread_main;
 #endif
			break;
		case POK_PARTITION_MODE_NORMAL:
 #ifdef POK_NEEDS_ERROR_HANDLING
			if ((POK_SCHED_CURRENT_THREAD == POK_CURRENT_PARTITION.thread_error) &&
					(POK_CURRENT_THREAD.state == POK_STATE_RUNNABLE))
			{
					elected = POK_CURRENT_PARTITION.thread_error;
					POK_CURRENT_PARTITION.current_thread = elected;
					break;
			}
 #endif
// ********************* O1 SCHEDULER ***********************************************
 #ifdef POK_NEEDS_SCHED_O1
			moment = now - start_of_MAF + 1;
  #ifdef POK_NEEDS_DEBUG_O1
			printf("DEBUG_O1::Thread switch!\n");
			printf("DEBUG_O1::Now: ");print_long(now);printf("\n");
			printf("DEBUG_O1::Time since start of MAF: %d\n",moment);
  #endif
			pok_partitions[pok_current_partition].runnables |= masks[moment][POK_STATE_RUNNABLE];

// *********************************************************************************
 #else /* ! POK_NEEDS SCHED_O1 */
			// Update of thread state only when the partition is in NORMAL mode
			for (i = 0; i < pok_partitions[pok_current_partition].nthreads; i++)
			{
				pok_thread_t* thread = &(pok_threads[current_partition->thread_index_low + i]);
				// No timed_wait or timeout implemented so far
  #if defined (POK_NEEDS_LOCKOBJECTS)
// || defined (POK_NEEDS_PORTS_QUEUEING) || defined (POK_NEEDS_PORTS_SAMPLING)
				if ((thread->state == POK_STATE_WAITING) && (thread->wakeup_time <= now))
				{
					thread->state = POK_STATE_RUNNABLE;
   #ifdef POK_NEEDS_SCHED_FPPS
					insert_in_queue(POK_SCHED_CURRENT_PARTITION,thread);
   #endif
				}
  #endif /* defined (POK_NEEDS_LOCKOBJECTS) */

				if ((thread->next_activation <= now) && (thread->state == POK_STATE_WAIT_NEXT_ACTIVATION))
				{
					thread->state = POK_STATE_RUNNABLE;
					thread->remaining_time_capacity =  thread->time_capacity;
  #ifdef POK_NEEDS_SCHED_FPPS
					insert_in_queue(POK_SCHED_CURRENT_PARTITION,thread);
  #endif
				}
			} // end update loop
 #endif /* POK_NEEDS_SCHED_O1 */
         		elected = POK_CURRENT_PARTITION.sched_func (current_partition->thread_index_low,
                                                     current_partition->thread_index_high);

			/** Update information on partition to be consistent */
			if (elected == IDLE_THREAD)
			{
				pok_threads[IDLE_THREAD].partition = POK_SCHED_CURRENT_PARTITION;
			}

			POK_CURRENT_PARTITION.current_thread = elected;

			// Compute the thread next dealine only if the elected thread is not the main thread or the idle thread
			if ( (elected != IDLE_THREAD) && (elected != POK_CURRENT_PARTITION.thread_main))
			{
				// compute next thread's deadline
				pok_threads[elected].end_time = now + pok_threads[elected].remaining_time_capacity;
   			}
			break;

		default:
			break;
	} // switch on partition mode

	return (elected);
}


void pok_check_remaining_time_capacity(){

	if ( (POK_SCHED_CURRENT_THREAD != IDLE_THREAD) &&
		(POK_SCHED_CURRENT_THREAD != POK_CURRENT_PARTITION.thread_main) &&
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
			(( POK_CURRENT_PARTITION.runnables & (1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low)) ) != 0)
 #else
		(POK_CURRENT_THREAD.state == POK_STATE_RUNNABLE)
 #endif
		/* this is a necessary condition as the current thread could be in WAIT_FOR_NEXT_ACTIVATION state
		   e.g. it has performed a periodic wait */
 #ifdef POK_NEEDS_ERROR_HANDLING
		&& (POK_SCHED_CURRENT_THREAD != POK_CURRENT_PARTITION.thread_error)
 #endif
		)
	{
		if (POK_CURRENT_THREAD.remaining_time_capacity > 0)
        	{
			// the current thread has been executed at least once (note that it is the current thread)
			POK_CURRENT_THREAD.remaining_time_capacity = POK_CURRENT_THREAD.remaining_time_capacity - 1;
			// FIXED: if the current thread run out of time capacity then it
			// should not be elected and its state is set to POK_STATE_WAIT_NEXT_ACTIVATION
			if (POK_CURRENT_THREAD.remaining_time_capacity==0)
			{
				POK_CURRENT_THREAD.state = POK_STATE_WAIT_NEXT_ACTIVATION;

				// Original POK: next activation and deadline set when the thread is set to RUNNABLE
				POK_CURRENT_THREAD.next_activation = POK_CURRENT_THREAD.next_activation + POK_CURRENT_THREAD.period;
				POK_CURRENT_THREAD.end_time = POK_CURRENT_THREAD.next_activation + POK_CURRENT_THREAD.time_capacity;
			}
		}
	}
}

void pok_sched()
{
 #if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)
  #ifdef POK_NEEDS_SCHED_O1

 	/* enable both caches */
	pok_arch_cache_enable();

	/* manage POSTWRITE */
	if (next_subslot_postwrite)
	{
   #ifdef POK_NEEDS_DEBUG_O1
		printf("DEBUG_O1::Executing POSTWRITE for slot %d for %u\n",pok_sched_current_slot,pok_postwrite_times[pok_sched_current_slot]);
   #endif
		next_timer = pok_postwrite_times[pok_sched_current_slot] * time_inter;
		pok_arch_set_decr(next_timer);
		next_subslot_postwrite = FALSE;
		pok_slot_write_flush (pok_current_partition);
		return;
	}

  #endif
 #endif /* (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING) */

	uint32_t elected_thread = 0;
	uint64_t now = POK_GETTICK();

 #ifdef POK_NEEDS_SCHED_O1

	/* Invalidate both caches */
	pok_arch_cache_invalidate();

	pok_partition_t* elected_partition = pok_elect_partition(now);
	elected_thread = pok_elect_thread(elected_partition,now);
	//printf("NOW : %d \t", (uint32_t)((now << 32)>>32));
	//printf("elected_thread : %d \t",elected_thread );
	//printf("elected_thread : %d \t",elected_thread );
  # if ((defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)))
	next_timer = (pok_sched_slots[pok_sched_current_slot] - pok_postwrite_times[pok_sched_current_slot]) * time_inter;
	if (pok_postwrite_times[pok_sched_current_slot] > 0)
		next_subslot_postwrite = TRUE;
  #else
	next_timer = pok_sched_slots[pok_sched_current_slot] * time_inter;
  #endif

  #ifdef POK_NEEDS_DEBUG_O1
	printf("DEBUG_O1::Elected partition %d and elected thread %d\n",elected_partition->partition_id, elected_thread);
	printf("DEBUG_O1::Setting next timer to %d and switching partition\n",next_timer);
	printf("DEBUG_O1::Partition switch\n");
  #endif
	pok_arch_set_decr(next_timer);

	pok_sched_partition_switch (elected_thread);

 #else /* ! POK_NEEDS_SCHED_O1 */
		/* End of the currently executing partition slot */
		if (pok_sched_next_deadline <= now)
		{
			/* Select partition */
			pok_partition_t* elected_partition = pok_elect_partition(now);
			elected_thread = pok_elect_thread(elected_partition,now);
			/* Switch partition (and context switch) */
			pok_sched_partition_switch (elected_thread);

		} else {
			/* Select thread */
			elected_thread = pok_elect_thread(&(pok_partitions[pok_current_partition]),now);
			/* Context switch */
			pok_sched_context_switch (elected_thread);
		}
  #ifdef POK_NEEDS_DEBUG
		printf("elect_thread %d\n",elected_thread);
  #endif
 #endif /* POK_NEEDS_SCHED_O1 */

}

#endif /* ! POK_NEEDS_PARTITIONS */
/*
void pok_sched_thread_switch ()
{
	int i;
	uint64_t now;
	uint32_t elected;
	now = POK_GETTICK();

	for (i = 0; i <= POK_CONFIG_NB_THREADS; ++i)
	{
		if ((pok_threads[i].state == POK_STATE_WAITING) &&
			(pok_threads[i].wakeup_time <= now))
		{
			pok_threads[i].state = POK_STATE_RUNNABLE;
#ifdef POK_NEEDS_SCHED_FPPS
		      insert_in_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[i]);
#endif
		}
	}
	elected = pok_sched_part_election (0, POK_CONFIG_NB_THREADS);
	pok_sched_context_switch(elected);
}
#endif
*/
/* POK_NEEDS_PARTITIONS */

/*
 * Context-switch function to switch from one thread to another
 * Rely on architecture-dependent functionalities (must include arch.h)
 */
void pok_sched_context_switch (const uint32_t elected_id)
{
#ifndef POK_ARCH_MPPA
	uint32_t *current_sp;
	current_sp = &POK_CURRENT_THREAD.sp;
#else
	uint32_t current_sp;
	current_sp = POK_CURRENT_THREAD.sp;
#endif
	uint32_t new_sp;
	new_sp = pok_threads[elected_id].sp;

	/* No need to switch */
	if (POK_SCHED_CURRENT_THREAD == elected_id)
	{
#ifdef POK_NEEDS_DEBUG
         printf("<=> NO switch from thread %d, sp=0x%x\n",POK_SCHED_CURRENT_THREAD, current_sp);
         printf("<=> NO switch to thread %d, sp=0x%x\n",elected_id, new_sp);
#endif
		return;
	}

	/*  FIXME : current debug session about exceptions-handled */
#ifdef POK_NEEDS_DEBUG
	 printf("switch from thread %d, sp=0x%x\n",POK_SCHED_CURRENT_THREAD, current_sp);
	 printf("switch to thread %d, sp=0x%x entry point = 0x%x\n",elected_id, new_sp, pok_threads[elected_id].entry);
#endif
	// Update current thread id now
	current_thread = elected_id;
#ifdef POK_ARCH_MPPA
	pok_current_context = new_sp;
#endif
	// Call to low-level context switch routine from entry.S
	pok_context_switch(current_sp, new_sp);

}

/*
 * Context-switch function between partitions
 * Rely on architecture-dependent functionnalities (must include arch.h)
 */
void pok_sched_partition_switch (const uint32_t elected_id)
{
#ifndef POK_ARCH_MPPA
	uint32_t *current_sp;
	uint32_t new_sp;

	current_sp = &POK_CURRENT_THREAD.sp;
	new_sp = pok_threads[elected_id].sp;
#endif
// 	if (POK_SCHED_CURRENT_THREAD == elected_id)
// 	{
// #ifdef POK_NEEDS_DEBUG
// 		printf("<=> NO switch from thread %d, sp=0x%x\n",POK_SCHED_CURRENT_THREAD, current_sp);
// 		printf("<=> NO switch to thread %d, sp=0x%x\n",elected_id, new_sp);
// #endif
// 		return;
// 	}
//
// 	/* FIXME : current debug session about exceptions-handled */
// #ifdef POK_NEEDS_DEBUG
// 	printf("switch from thread %d, sp=0x%x\n",POK_SCHED_CURRENT_THREAD, current_sp);
// 	printf("switch to thread %d, sp=0x%x, entry point = 0x%x\n",elected_id, new_sp, pok_threads[elected_id].entry);
// #endif
//
// 	// Switch from one partition to another
// 	pok_space_switch(POK_CURRENT_THREAD.partition, pok_threads[elected_id].partition);
	// Update current thread id now
	current_thread = elected_id;
#ifdef POK_ARCH_MPPA
	pok_current_context = pok_threads[current_thread].sp;
#else
	// Call to low-level context switch routine from entry.S
	pok_context_switch(current_sp, new_sp);
#endif
}

uint32_t pok_sched_part_rr (const uint32_t index_low, const uint32_t index_high)
{
	uint32_t res;
	uint32_t from;

	pok_check_remaining_time_capacity();

	if (POK_SCHED_CURRENT_THREAD == IDLE_THREAD)
	{
		res = prev_current_thread;
	}
	else
	{
		res = POK_SCHED_CURRENT_THREAD;
	}

	from = res;

	if ((POK_CURRENT_THREAD.remaining_time_capacity > 0) && (POK_CURRENT_THREAD.state == POK_STATE_RUNNABLE))
	{
		return POK_SCHED_CURRENT_THREAD;
	}

	do
	{
		res++;
		if (res >= index_high)
		{
			res = index_low;
		}
	}
	while ((res != from) && (pok_threads[res].state != POK_STATE_RUNNABLE));

	if ((res == from) && (pok_threads[res].state != POK_STATE_RUNNABLE))
	{
		res = IDLE_THREAD;
		if (POK_SCHED_CURRENT_THREAD != IDLE_THREAD)
		{
			prev_current_thread = POK_SCHED_CURRENT_THREAD;
		}
	}
	return res;
}


#if defined (POK_NEEDS_LOCKOBJECTS)
//|| defined (POK_NEEDS_PORTS_QUEUEING) || defined (POK_NEEDS_PORTS_SAMPLING)
void pok_sched_unlock_thread (const uint32_t thread_id)
{
 #ifdef POK_NEEDS_SCHED_O1
	uint32_t thread_pos = thread_id;
	if (pok_threads[thread_pos].state != POK_STATE_LOCK)
	{
		printf("WARNING: Invoked UNLOCK on non-locked thread\n");
	}
	pok_threads[thread_pos].state = POK_STATE_RUNNABLE;
	POK_CURRENT_PARTITION.runnables |= (1 << (thread_pos - POK_CURRENT_PARTITION.thread_index_low));
  #ifdef POK_NEEDS_DEBUG_O1
	printf("DEBUG_O1::Invoked UNLOCK on thread %u in position %u\n",pok_threads[thread_id].id,thread_pos);
  #endif
 #else /* ! POK_NEEDS_SCHED_O1 */
	pok_threads[thread_id].state = POK_STATE_RUNNABLE;
  #ifdef POK_NEEDS_SCHED_FPPS
	// We just insert it in the ready queue
	insert_in_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[thread_id]);
  #endif
 #endif
}
#endif

#ifdef POK_NEEDS_SCHED_O1_SPLIT
/* NB: this function does not update the state of the threads, so in this implementation
 * we have to use the runnable bitmask to check if a thread is runnable
 */
void pok_sched_unlock_threads_by_bitmask (const uint32_t current_runnables)
{
	POK_CURRENT_PARTITION.runnables |= current_runnables;
}

void pok_sched_insert_sporadic_in_queue(uint32_t thread_bitmask)
{
	/* increment the tail index if thread_bitmask is not 0 (i.e. if we have to insert a thread in the queue) */
	POK_CURRENT_PARTITION.tail_index = ( POK_CURRENT_PARTITION.tail_index +
					(((thread_bitmask|(~(thread_bitmask)+1)) >> 31) & 1) ) % POK_CONFIG_NB_SPORADIC_THREADS;
	/* insert the thread bitmask in the queue adding the thread bitmask in the (new) tail position; if thread_bitmask
	 * is 0 the queue is not modified
	 */
	POK_CURRENT_PARTITION.sporadic_ready_queue[POK_CURRENT_PARTITION.tail_index] |= thread_bitmask;
}

void pok_sched_extract_sporadic_from_queue(uint32_t thread_bitmask)
{
	/* extract the thread from the queue resetting the bitmask in the head position; if thread_bitmask is 0 the queue
	 * is not modified
	 */
	POK_CURRENT_PARTITION.sporadic_ready_queue[POK_CURRENT_PARTITION.head_index] &= ~(thread_bitmask);
	/* increment the head index if thread_bitmask is not 0 (i.e. if we have to extract a thread from the queue) */
	POK_CURRENT_PARTITION.head_index = ( POK_CURRENT_PARTITION.head_index +
					(((thread_bitmask|(~(thread_bitmask)+1)) >> 31) & 1) ) % POK_CONFIG_NB_SPORADIC_THREADS;
}
#endif /* end ifdef POK_NEEDS_SCHED_O1_SPLIT */

#if defined (POK_NEEDS_LOCKOBJECTS) || defined (POK_NEEDS_PORTS_QUEUEING) || defined (POK_NEEDS_PORTS_SAMPLING)
void pok_sched_lock_current_thread (void)
{
	pok_threads[current_thread].state = POK_STATE_LOCK;
 #ifdef POK_NEEDS_SCHED_FPPS
	remove_from_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[current_thread]);
	pok_sched();
 #endif
 #ifdef POK_NEEDS_SCHED_O1
	POK_CURRENT_PARTITION.runnables &= ~(1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low));
  #ifdef POK_NEEDS_DEBUG_O1
	printf("DEBUG_O1::LOCKING thread %u in position %u\n",POK_CURRENT_THREAD.id,POK_CURRENT_THREAD.pos);
  #endif
	// Reorder asynch events of the partition and activate those that have timed out
	pok_sched_reorder_asynch_list();
	pok_sched_service_asynch_events();
	// elect next thread
	uint32_t elected = POK_CURRENT_PARTITION.sched_func (POK_CURRENT_PARTITION.thread_index_low,
                                                     POK_CURRENT_PARTITION.thread_index_high);

	pok_sched_context_switch(elected);
 #endif /*end if def POK_NEEDS_SCHED_O1*/
}

void pok_sched_lock_current_thread_timed (const uint64_t time)
{
	pok_threads[current_thread].state = POK_STATE_WAITING;
	pok_threads[current_thread].wakeup_time = time;
 #ifdef POK_NEEDS_SCHED_FPPS
	remove_from_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[current_thread]);
 #endif
}
#endif /* defined (POK_NEEDS_LOCKOBJECTS) */


void pok_sched_stop_self (void)
{
	POK_CURRENT_THREAD.state = POK_STATE_STOPPED;
#ifdef POK_NEEDS_SCHED_FPPS
	remove_from_queue(POK_SCHED_CURRENT_PARTITION,&POK_CURRENT_THREAD);
#endif
	// the following instruction has been removed for consistency with similar procedures
	// pok_sched ();
}

void pok_sched_suspend_self (void)
{
	POK_CURRENT_THREAD.state = POK_STATE_SUSPENDED;
#ifdef POK_NEEDS_SCHED_FPPS
	remove_from_queue(POK_SCHED_CURRENT_PARTITION,&POK_CURRENT_THREAD);
#endif
#ifdef POK_NEEDS_SCHED_O1
	POK_CURRENT_PARTITION.runnables &= ~(1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low));
	// Reorder asynch events of the partition and activate those that have timed out
	pok_sched_reorder_asynch_list();
	pok_sched_service_asynch_events();
	// elect next thread
	uint32_t elected = POK_CURRENT_PARTITION.sched_func (POK_CURRENT_PARTITION.thread_index_low,
                                                     POK_CURRENT_PARTITION.thread_index_high);

	pok_sched_context_switch(elected);
#else
	pok_sched();
#endif
}


void pok_sched_stop_thread (const uint32_t tid)
{
	pok_threads[tid].state = POK_STATE_STOPPED;
#ifdef POK_NEEDS_SCHED_FPPS
	// Remove from the ready queue (except for the main thread)
	if (tid != pok_partitions[POK_SCHED_CURRENT_PARTITION].thread_main)
	{
		remove_from_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[tid]);
	}
#endif
}

pok_ret_t pok_sched_end_period ()
{
	if (POK_SCHED_CURRENT_THREAD != IDLE_THREAD)
	{
		POK_CURRENT_THREAD.state = POK_STATE_WAIT_NEXT_ACTIVATION;
		// next realease point = process period + previous release point
		POK_CURRENT_THREAD.next_activation =  POK_CURRENT_THREAD.next_activation  + POK_CURRENT_THREAD.period;
		// deadline time = next release point + time capacity
		POK_CURRENT_THREAD.end_time = POK_CURRENT_THREAD.next_activation + POK_CURRENT_THREAD.time_capacity;
#ifdef POK_NEEDS_DEBUG
		printf("DEBUG::Activation of thread %d updated to ",POK_SCHED_CURRENT_THREAD);print_long(POK_CURRENT_THREAD.next_activation);printf("\n");
#endif
#if POK_NEEDS_SCHED_O1
		// set up new timer for next activation in case of DELAYED_START
		if(POK_CURRENT_THREAD.timeout != POK_NULL)
			pok_sched_set_asynch_event(POK_SCHED_CURRENT_THREAD, POK_CURRENT_THREAD.next_activation, POK_EVENT_DELAYED_START);
#endif

#ifdef POK_NEEDS_SCHED_FPPS
		remove_from_queue(POK_SCHED_CURRENT_PARTITION,&pok_threads[current_thread]);
#endif

#ifdef POK_NEEDS_SCHED_O1
		POK_CURRENT_PARTITION.runnables &= ~(1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low));
#endif

#ifdef POK_NEEDS_SCHED_O1_SPLIT
		uint64_t current_time = POK_CLOCK();
		/* if the calling thread is a predecessor, set its bit in the executed_predecessors bitmask */
		POK_CURRENT_PARTITION.executed_predecessors |= successors_bitmasks[POK_CURRENT_THREAD.id];
		/* if the calling thread is a predecessor and the lock object is not locked, its successor is now runnable; */
		uint8_t lockobj_id = predecessors_events_id[POK_CURRENT_THREAD.id] + POK_CURRENT_PARTITION.lockobj_index_low - 1;
		uint32_t successor_bitmask = ~(pok_partitions_lockobjs[lockobj_id].is_locked) & successors_bitmasks[POK_CURRENT_THREAD.id];
		/* if the successor has to be unlocked update its state in the lock object */
		pok_partitions_lockobjs[lockobj_id].waiting_threads &= ~(successor_bitmask);
		/* unlock the successor; note that the successor is a sporadic thread,
		 * so it has to be added in the FIFO queue (not in runnables!); if no prodecessor has to
		 * be resumed, the ready queue is not modified
		 */
		pok_sched_insert_sporadic_in_queue(successor_bitmask);
		/* if the successor has to be unlocked update its absolute deadline: VARIABILITY!!! */
		int successor_index;
		if (successor_bitmask != 0)
		{
			successor_index = get_thread_index_deBruijn(successor_bitmask);
			pok_threads[successor_index + POK_CURRENT_PARTITION.thread_index_low].deadline = current_time +
				pok_threads_deadline[pok_threads[successor_index + POK_CURRENT_PARTITION.thread_index_low].id];
		}
#endif /* endif POK_NEEDS_SCHED_O1_SPLIT */
	}
#ifdef POK_NEEDS_SCHED_O1
	// Reorder asynch events of the partition and activate those that have timed out
	pok_sched_reorder_asynch_list();
	pok_sched_service_asynch_events();
	// elect next thread
	uint32_t elected = POK_CURRENT_PARTITION.sched_func (POK_CURRENT_PARTITION.thread_index_low,
                                                     POK_CURRENT_PARTITION.thread_index_high);
	pok_sched_context_switch(elected);
#else
	pok_sched();
#endif

	return POK_ERRNO_OK;
}


#if defined (POK_NEEDS_PARTITIONS) && defined (POK_NEEDS_ERROR_HANDLING)
void pok_sched_activate_error_thread (void)
{
	uint32_t error_thread = pok_partitions[pok_current_partition].thread_error;
	if (error_thread != 0)
	{
		pok_threads[error_thread].remaining_time_capacity = 1000;
		pok_threads[error_thread].period = 100;
		pok_threads[error_thread].next_activation= 0;

		pok_threads[error_thread].state  = POK_STATE_RUNNABLE;
		pok_sched_context_switch (error_thread);
	}
}
#endif

// ************ FPPS SCHEDULING *****************
#ifdef POK_NEEDS_SCHED_FPPS
extern pok_thread_t* heads[];
extern pok_thread_t* priority_heads[][POK_CONFIG_PRIORITY_LEVELS];
extern pok_thread_t* priority_tails[][POK_CONFIG_PRIORITY_LEVELS];
void print_queues();

void remove_from_queue(uint16_t partition_id, pok_thread_t* thread)
{
	uint8_t thread_priority = thread->priority;

/*
#ifdef POK_NEEDS_DEBUG
	printf ("FPPS_DEBUG::Removing thread %d in partition %d\n",thread->id,partition_id);
	if (thread == POK_NULL)
	{
		printf ("FPPS_DEBUG::WARNING::Thread is POK_NULL!\n");
	}
   	if (heads[partition_id] != thread || priority_heads[partition_id][thread_priority] != thread)
	{
		printf ("FPPS_DEBUG::WARNING::Thread %d in partition %d has been removed and was not in the head of the queue!\n",thread->id,partition_id);
	}
#endif
*/

	// if the thread is the head of a priority queue the pointer to the head must be updated to the next thread
	if (priority_heads[partition_id][thread_priority] == thread)
	{
		priority_heads[partition_id][thread_priority] = thread->next;
	}

	// if the thread has the highest priority
	if (heads[partition_id] == thread)
	{
		// move the pointer of the global queue to its successor
		heads[partition_id] = thread->next;
		// thread has no successor <==> priority queue is empty
		if(thread->next == POK_NULL)
		{
			// search for the non-empty queue corresponding to the new highest priority
			int i=POK_CONFIG_PRIORITY_LEVELS-1;
			while(heads[partition_id]==POK_NULL && i>=0)
			{
				heads[partition_id] = priority_heads[partition_id][i];
				i--;
			}
		}
	}

	// update the pointer to the tail of the queue
	if (priority_tails[partition_id][thread_priority] == thread)
	{
		if (thread->previous != POK_NULL)
			// removed is the last (but not the only one) in its list
			priority_tails[partition_id][thread_priority] = thread->previous;
		else
			// removed thread was the only one in its list
			priority_tails[partition_id][thread_priority] = POK_NULL;
	}

	// update pointers of neighbours threads in the queue
	if (thread->previous != POK_NULL)
		thread->previous->next = thread->next;
	if (thread->next != POK_NULL)
		thread->next->previous = thread->previous;

	// reset pointers to previous and next threads in queue
	thread->previous = POK_NULL;
	thread->next = POK_NULL;
}

void insert_in_queue(uint16_t partition_id, pok_thread_t* thread)
{

	uint8_t thread_priority = thread->priority;

	// if any thread is in the queue update the pointer of the last thread
	if (priority_tails[partition_id][thread_priority] != POK_NULL)
	{
		thread->previous = priority_tails[partition_id][thread_priority];
		priority_tails[partition_id][thread_priority]->next = thread;
	}

	// update the pointer to the tail of the queue
	priority_tails[partition_id][thread_priority] = thread;

	// if queue was empty
	if (priority_heads[partition_id][thread_priority] == POK_NULL)
	{
		priority_heads[partition_id][thread_priority] = thread;
		// global head of the list may need to be updated as well
		if (heads[partition_id] == POK_NULL || heads[partition_id]->priority < thread_priority)
			heads[partition_id] = priority_heads[partition_id][thread_priority];
	}
}

uint32_t pok_sched_part_fpps ()
{
	uint32_t res;
	pok_thread_t* elected;

	elected = heads[POK_SCHED_CURRENT_PARTITION];
	if (elected == POK_NULL)
	{
		// queue is empty
		res = IDLE_THREAD;
		if (POK_SCHED_CURRENT_THREAD != IDLE_THREAD)
		{
			prev_current_thread = POK_SCHED_CURRENT_THREAD;
		}
	}
	else
	{
 #ifdef POK_NEEDS_DEBUG_FPPS
		print_queues();
 #endif
		// remove_from_queue(POK_SCHED_CURRENT_PARTITION,elected);
		res = elected->id;
   	}
	return res;
}

 #ifdef POK_NEEDS_DEBUG_FPPS
void print_queues()
{
	int i,j;
	printf("***** FPPS DEBUG ***********\n");
	printf("******* QUEUES *************\n");
	for(i =0; i<POK_CONFIG_NB_PARTITIONS; ++i)
	{
		printf("***** Partition %d **********\n",i);
		printf("Head: %d \n", heads[i]->id);
		for(j=0; j<POK_CONFIG_PRIORITY_LEVELS; ++j)
		{
			printf("Priority level %d: ",j);
			pok_thread_t* current = priority_heads[i][j];
			while (current != POK_NULL)
			{
				printf("%d-> ",current->id);
				current = current->next;
			}
			printf("\n");
		}
	}
	printf("****************************\n");
}
 #endif /* POK_NEEDS_DEBUG_FPPS */

#endif /* __POK_NEEDS_SCHED_FPPS */

// ************ O1 SCHEDULING *****************

#ifdef POK_NEEDS_SCHED_O1
/*	0x76BE629	 for 32bit Bitmaps 	*/
const uint32_t debruijn = 124511785;
const int dBIndex[32]={	0, 1,23, 2,29,24,19, 3,
			30,27,25,11,20, 8, 4,13,
			31,22,28,18,26,10, 7,12,
			21,17, 9, 6,16, 5,15,14};

/*
 * WARNING: Check "input != 0" before calling
 */
int get_thread_index_deBruijn (int partition_runnables)
{
	int res = 0;
	uint64_t temp;
	partition_runnables &= (-partition_runnables);
	temp = partition_runnables*debruijn;
	res = (int)(temp>>27);
	//printf ("DEBUG_O1::DE_BRUIJN Result:\t%d -> \t%d|%d\n", res, dBIndex[res]);
	return dBIndex[res];
}

uint32_t pok_sched_part_o1 ()
{
	uint32_t res;
	pok_thread_t* elected;

	unsigned int current_runnables = pok_partitions[POK_SCHED_CURRENT_PARTITION].runnables;

 #ifdef POK_NEEDS_DEBUG_O1
	printf("DEBUG_O1::Current runnables: %u\n", current_runnables);
 #endif
	if (current_runnables == 0)
	{
 #ifdef POK_NEEDS_DEBUG_O1
		printf("DEBUG_O1::Dispatching idle thread\n");
 #endif
		elected = POK_NULL;
		res = IDLE_THREAD;
		if (POK_SCHED_CURRENT_THREAD != IDLE_THREAD)
		{
			prev_current_thread = POK_SCHED_CURRENT_THREAD;
		}
	}
	else
	{
 #ifdef POK_NEEDS_SEPER_THREADS
		//TODO: REMOVE SUPER THREADS
		/* check if there are "super threads" to elect */
		if (pok_partitions[POK_SCHED_CURRENT_PARTITION].super_threads != 0)
		{
			/* if there is at least one "super thread" for this scheduling point the scheduler
			 * has to use the "super thread" bitmask to elect the next thread to execute;
			 * doing so the next thread will be the first "super thread"
			 */
			current_runnables = pok_partitions[POK_SCHED_CURRENT_PARTITION].super_threads;
		}
 #endif

		int thread_index = get_thread_index_deBruijn(current_runnables);
		//int position = pok_partitions[POK_SCHED_CURRENT_PARTITION].thread_index_low - zeros -1;
		int position =  pok_partitions[POK_SCHED_CURRENT_PARTITION].thread_index_low + thread_index;
		elected = &pok_threads[position];
		res = elected->pos;
//printf("DEBUG_O1::Next Thread: %i\n", position);

 #ifdef POK_NEEDS_SEPER_THREADS
		//TODO: REMOVE SUPER THREADS
		if (pok_partitions[POK_SCHED_CURRENT_PARTITION].super_threads != 0)
		{
			/* after the election the "super thread" returns to be a normal thread so its bit has
			 * to be resetted */
			pok_partitions[POK_SCHED_CURRENT_PARTITION].super_threads &= ~(1 << thread_index);
		}
 #endif

 #ifdef POK_NEEDS_DEBUG_O1
		printf("DEBUG_O1::Dispatching thread %d in runnable mask, corresponding to pok_threads[%d]\n",position,elected->pos);
 #endif
	}

	return res;
}

 #ifdef POK_NEEDS_SCHED_O1_SPLIT
uint32_t pok_sched_part_o1_split()
{
	//int fake_sporadic_position; /*set but not used*/
	uint32_t is_late = 0;
	int selected_position;

	/* get the bitmask corresponding to the periodic candidate (if no periodic threads are ready, this is the IDLE position) */
	int periodic_position = get_thread_index_deBruijn(POK_CURRENT_PARTITION.runnables);
	/* get the bitmask corresponding to the sporadic candidate (if no sporadic threads are ready, this is the IDLE position) */
	int sporadic_position = get_thread_index_deBruijn(POK_CURRENT_PARTITION.sporadic_ready_queue[POK_CURRENT_PARTITION.head_index]);
	int original_sporadic_position = sporadic_position;
	int sporadic_index = sporadic_position + POK_CURRENT_PARTITION.thread_index_low;

	uint64_t WCET = pok_threads[sporadic_index].time_capacity;
	uint64_t current_time = POK_CLOCK();
	/* check if the sporadic thread can execute within its deadline and if not check if it has tardiness
	 * (this works also for IDLE thread)
	 */
	bool_t deadline_condition = pok_threads[sporadic_index].deadline > (current_time + WCET);
	bool_t tardiness_condition = has_tardiness[pok_threads[sporadic_index].id];
	if ( deadline_condition || tardiness_condition )
	{
		/* sporadic thread can execute */
		//fake_sporadic_position = IDLE_THREAD - POK_CURRENT_PARTITION.thread_index_low; /*set but not used*/
		is_late = 0;
	}
	else
	{
		/* sporadic thread can not execute */
		sporadic_position = IDLE_THREAD - POK_CURRENT_PARTITION.thread_index_low;
		is_late = 0xffffffff;
	}
        /* check if the sporadic thread interferes with the periodic thread */
	if (WCET < pok_threads[periodic_position + POK_CURRENT_PARTITION.thread_index_low].time_capacity)
	{
		selected_position = sporadic_position;
	} else {
		selected_position = periodic_position;
	}

	/* extract the selected thread from the sporadic FIFO ready queue (if it is sporadic) */
	pok_sched_extract_sporadic_from_queue( ((1<<selected_position) & ~(1<<(IDLE_THREAD - POK_CURRENT_PARTITION.thread_index_low))) & 							POK_CURRENT_PARTITION.sporadic_bitmask );
	/* if the sporadic thread can't execute because it is late, we remove it from the ready queue */
	pok_sched_extract_sporadic_from_queue( (1<<original_sporadic_position) & is_late );

	return (selected_position + POK_CURRENT_PARTITION.thread_index_low);
}
 #endif /* end ifdef POK_NEEDS_SCHED_O1_SPLIT */


/*void set_runnable(pok_thread_t* thread)
{
	uint32_t nr_shifts = thread->pos - pok_partitions[POK_SCHED_CURRENT_PARTITION].thread_index_low;
	pok_partitions[POK_SCHED_CURRENT_PARTITION].runnables |= (1 << nr_shifts);
}*/
#endif /* POK_NEEDS_SCHED_O1 */

#endif /* __POK_NEEDS_SCHED */

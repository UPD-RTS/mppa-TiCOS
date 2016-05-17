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

#ifndef __POK_SCHED_H__
#define __POK_SCHED_H__

#if defined (POK_NEEDS_SCHED) || defined (POK_NEEDS_THREADS)

#include <types.h>
#include <errno.h>
#include <core/schedvalues.h>

#ifdef POK_NEEDS_PARTITIONS
extern uint8_t pok_current_partition;
#define POK_SCHED_CURRENT_PARTITION pok_current_partition
#endif


extern uint32_t  current_thread;
#define POK_SCHED_CURRENT_THREAD current_thread

typedef enum
{
	POK_STATE_STOPPED = 0,
	POK_STATE_RUNNABLE = 1,
	POK_STATE_WAITING = 2,
	POK_STATE_LOCK = 3,
	POK_STATE_WAIT_NEXT_ACTIVATION = 4,
	POK_STATE_SUSPENDED = 5,
	POK_STATE_T_NR_ELEMENTS = 6
} pok_state_t;

uint32_t get_partition_period(uint8_t			partition_id);

void pok_sched_init (void); /* Initialize scheduling stuff */

void pok_sched (void);		/* Main scheduling function, this function
							* schedules everything
							*/

/* Get priority function, return are made according to a scheduler */
uint8_t pok_sched_get_priority_min (const pok_sched_t sched_type);
uint8_t pok_sched_get_priority_max (const pok_sched_t sched_type);

/* Scheduler election method */
uint8_t pok_sched_election (void);
uint32_t pok_sched_part_rr (const uint32_t ,const uint32_t);
uint32_t pok_sched_part_rms (const uint32_t ,const uint32_t);
uint32_t pok_sched_part_fpps ();
uint32_t pok_sched_part_o1 ();
/* the election method for the split solution */
uint32_t pok_sched_part_o1_split ();


/* Context switch functions */
void pok_sched_context_switch (const uint32_t);
void pok_partition_switch (void);

/*
 * Functions to lock threads
 */
void pok_sched_lock_thread (const uint32_t);
void pok_sched_unlock_thread (const uint32_t);
#ifdef POK_NEEDS_SCHED_O1_SPLIT
/* a function to update the runnables bitmask "or-ing" the bitmask
 * representing the threads to unlock (possibly 0)
 */
void pok_sched_unlock_threads_by_bitmask (const uint32_t);
#endif
void pok_sched_lock_current_thread (void);
void pok_sched_lock_current_thread_timed (const uint64_t time);
void pok_sched_stop_thread (const uint32_t tid);
void pok_sched_stop_self (void);
void pok_sched_suspend_self (void);
//void pok_sched_suspend (uint32_t tid);
pok_ret_t pok_sched_end_period ();

#ifdef POK_NEEDS_PARTITIONS
void pok_sched_activate_error_thread (void);
#endif

#define pok_sched_get_current(thread_id) *thread_id=POK_SCHED_CURRENT_THREAD

#ifdef POK_NEEDS_SCHED_O1
unsigned int masks[POK_CONFIG_SCHEDULING_MAJOR_FRAME][POK_STATE_T_NR_ELEMENTS]; //32-bits mask for each partition
uint64_t start_of_MAF;
extern uint32_t pok_thread_ids_to_pos_map[POK_CONFIG_NB_THREADS];
// inclusions from timer.c
extern uint64_t time_inter;
extern uint64_t next_timer;
extern int pok_arch_set_decr (uint64_t);
typedef enum
{
	POK_EVENT_DELAYED_START	=0,
	POK_EVENT_TIMED_WAIT	=1
} pok_event_type_t;

#pragma pack (push)
#pragma pack (2)
typedef struct pok_sched_asynch_event_t
{
	uint32_t pos;
	uint64_t timer; 	/*relative*/
	uint64_t timestamp; /*absolute*/
	uint32_t mask;
	pok_event_type_t type;
	struct pok_sched_asynch_event_t* previous;
	struct pok_sched_asynch_event_t* next;

} pok_sched_asynch_event_t;
#pragma pack (pop)
pok_sched_asynch_event_t asynch_queue[POK_CONFIG_NB_ASYNCH_EVENTS];

#ifdef POK_NEEDS_SCHED_O1_SPLIT
/* with the split solution this function is used also in lockobj.c */
int get_thread_index_deBruijn (int partition_runnables);
#endif

void pok_sched_set_asynch_event(uint32_t thread_id, uint64_t time, pok_event_type_t type);
void pok_sched_reorder_asynch_list();
void pok_sched_service_asynch_events();
// void pok_sched_cancel_asynch_event(uint32_t thread_id); /* TO BE IMPLEMENTED */

#ifdef POK_NEEDS_DEBUG_O1
extern void print_long(long long); //just for debug
void print_queue(pok_sched_asynch_event_t* head);
#endif

#ifdef POK_NEEDS_SCHED_O1_SPLIT
/* two functions to insert/extract sporadic threads in/from the FIFO ready queue;
 * if thread_bitmask is 0 the queue is not modified
 */
void pok_sched_insert_sporadic_in_queue(uint32_t thread_bitmask);
void pok_sched_extract_sporadic_from_queue(uint32_t thread_bitmask);
#endif

#endif

# if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)
bool_t next_subslot_postwrite;
#endif

#endif /* POK_NEEDS.... */

#endif /* !__POK_SCHED_H__ */

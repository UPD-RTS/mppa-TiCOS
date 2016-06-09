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
 * \file	 core/lockobj.c
 * \brief	Provides functionnalities for locking functions (mutexes, semaphores and so on)
 *
 * This file contains the implementation code for mutexes, conditions and
 * semaphores. This is implemented in the same file since the functionnalities
 * does not differ so much.
 */


// lockobjs are used by events, blackboards, buffers.

#if  defined (POK_NEEDS_LOCKOBJECTS)

#include <arch.h>
#include <errno.h>
#include <types.h>
#include <core/sched.h>
#include <core/time.h>
#include <core/partition.h>
#include <core/thread.h>
#include <core/lockobj.h>
#include <libc.h>

pok_lockobj_t	pok_partitions_lockobjs[POK_CONFIG_NB_LOCKOBJECTS];

 #ifdef POK_NEEDS_SCHED_O1_SPLIT
extern uint64_t pok_threads_deadline[];
extern uint32_t successors_bitmasks[];
 #endif

/**
 * Init the array of lockobjects
 */
pok_ret_t pok_lockobj_init ()
{

 #if POK_CONFIG_NB_LOCKOBJECTS > 0	// i.e. defined (POK_NEEDS_LOCKOBJECTS)
	uint8_t i;
  #ifdef POK_NEEDS_PARTITIONS
   #ifdef POK_NEEDS_ERROR_HANDLING
	uint32_t total_lockobjects;

	total_lockobjects = 0;

	for ( i = 0 ; i < POK_CONFIG_NB_PARTITIONS ; i++)
	{
		total_lockobjects = total_lockobjects + pok_partitions[i].nlockobjs;
	}

	if (total_lockobjects != POK_CONFIG_NB_LOCKOBJECTS)
	{
		pok_kernel_error (POK_ERROR_KIND_KERNEL_CONFIG);
	}
   #endif // POK_NEEDS_ERROR_HANDLING
  #endif  // POK_NEEDS_PARTITIONS

	for ( i = 0 ; i < POK_CONFIG_NB_LOCKOBJECTS ; i++)
	{
		//	pok_partitions_lockobjs[i].spin		  = 0;
  #ifdef POK_NEEDS_SCHED_O1_SPLIT
		/* the bitmask is init with all 0s (event is not locked) */
		pok_partitions_lockobjs[i].is_locked	= 0;
  #else
		pok_partitions_lockobjs[i].is_locked	= FALSE;
  #endif
		pok_partitions_lockobjs[i].initialized = FALSE;
	}
 #endif // POK_CONFIG_NB_LOCKOBJECTS > 0
	return POK_ERRNO_OK;
}

/*
* calls by the (non-ARINC) ports creation functions and by the pok_lockobj_partition_create function
*/
pok_ret_t pok_lockobj_create (pok_lockobj_t* obj, const pok_lockobj_attr_t* attr)
{
	uint32_t tmp;

	/* Check the policy of the lockobj */
	if ((attr->locking_policy != POK_LOCKOBJ_POLICY_STANDARD))
	{
		return POK_ERRNO_LOCKOBJ_POLICY;
	}

	/* Check the kind of the locjobj, must have a declared kind
	 * If not, of course, we reject the creation. */
	if ( /*(attr->kind != POK_LOCKOBJ_KIND_MUTEX) 	only the POK_LOCKOBJ_KIND_EVENT used a the moment
		&& (attr->kind != POK_LOCKOBJ_KIND_SEMAPHORE)
		&& */ (attr->kind != POK_LOCKOBJ_KIND_EVENT))
	{
		return POK_ERRNO_LOCKOBJ_KIND;
	}

	for (tmp = 0 ; tmp < POK_CONFIG_NB_THREADS ; tmp++ )
	{
		obj->thread_state [tmp] = LOCKOBJ_STATE_UNLOCK;
	}

 #ifdef POK_NEEDS_SCHED_O1_SPLIT
	/* Initialize the bitmask */
	obj->waiting_threads = 0;
 #endif

	obj->queueing_policy	= attr->queueing_policy;
	obj->locking_policy		= attr->locking_policy;
	obj->kind				= attr->kind;
	obj->initialized	 	= TRUE;
	return POK_ERRNO_OK;

}


 #ifdef POK_NEEDS_LOCKOBJECTS
/*
* calls during blackboard, event, and buffer creation.
*/
pok_ret_t pok_lockobj_partition_create (pok_lockobj_id_t* id, const pok_lockobj_attr_t* attr)
{
	uint8_t i;
	uint8_t pid;
	uint8_t mid;
	pok_ret_t ret;
	uint8_t lower_bound = 0;
	uint8_t upper_bound = 0;
	//uint8_t nb_lockobjs = 0; /* set but not used */
	bool_t  found = FALSE;

	if (  (POK_CURRENT_PARTITION.mode != POK_PARTITION_MODE_INIT_COLD) &&
			(POK_CURRENT_PARTITION.mode != POK_PARTITION_MODE_INIT_WARM))
	{
		return POK_ERRNO_MODE;
	}

	pid = POK_SCHED_CURRENT_PARTITION;

	lower_bound = pok_partitions[pid].lockobj_index_low;
	upper_bound = pok_partitions[pid].lockobj_index_high;
	//nb_lockobjs = pok_partitions[pid].nlockobjs; /* set but not used */

	/*
	 * Find a lockobject for the partition
	 */
	mid = lower_bound;
	while (mid < upper_bound)
	{

		if (pok_partitions_lockobjs[mid].initialized == FALSE)
		{
			found = TRUE;
			break;
		}
		mid++;
	}

	if (found == FALSE)
	{
		return POK_ERRNO_LOCKOBJ_UNAVAILABLE;
	}

	*id = mid;

	ret = pok_lockobj_create (&pok_partitions_lockobjs[mid], attr);

	if (ret != POK_ERRNO_OK)
	{
		return ret;
	}

	/** CHECK this loop may be over all threads *not just those belonging to the current partition*/
	for (i = 0 ; i < POK_CONFIG_NB_THREADS ; i++)
	{
		pok_partitions_lockobjs[mid].thread_state[i] = LOCKOBJ_STATE_UNLOCK;
	}

	return POK_ERRNO_OK;
}
 #endif


/*
*	 pok_lockobj_eventwait
*/
pok_ret_t pok_lockobj_eventwait (pok_lockobj_t* obj, const uint64_t timeout)
{
	pok_ret_t ret;

	//SPIN_LOCK (obj->eventspin);

	if (obj->initialized == FALSE)
	{
		return POK_ERRNO_LOCKOBJ_NOTREADY;
	}

	if (obj->kind != POK_LOCKOBJ_KIND_EVENT)
	{
		return POK_ERRNO_EINVAL;
	}

 #ifndef POK_NEEDS_SCHED_O1_SPLIT
	 // Thre si no need to wait
	if (obj->is_locked == FALSE){
  #ifdef DEBUG_LOCK
		printf ("EVENT is already UP!");
  #endif
		return POK_ERRNO_OK;
	 }

 #else  /* POK_NEEDS_SCHED_O1_SPLIT is defined */
	/* with the split solution we have to use the executed_predecessors bitmask to check if
	 * the predecessor of the calling thread has executed; if the calling thread has not a
	 * predecessor (i.e. it is not a successor) its bit in executed_processor is always 1
	 * so the condition is always true (i.e. the condition is only "the event is UP") */
	if ( ( (POK_CURRENT_PARTITION.executed_predecessors & (1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low))) & ~(obj->is_locked) ) != 0 )
	{
		return POK_ERRNO_OK;
	}
 #endif

 #ifndef POK_NEEDS_SCHED_O1_SPLIT
	obj->thread_state[POK_SCHED_CURRENT_THREAD] = LOCKOBJ_STATE_WAITEVENT;
 #else /* POK_NEEDS_SCHED_O1_SPLIT is defined */
	/* current thread has to wait, set its bit in the bitmask */
	obj->waiting_threads |= (1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low));
 #endif

	/* NO SUPPORT FOR TIMEOUT RIGHT NOW */

	pok_sched_lock_current_thread ();

	//SPIN_UNLOCK (obj->eventspin);

 #ifndef POK_NEEDS_SCHED_O1_SPLIT
	obj->thread_state[POK_SCHED_CURRENT_THREAD] = LOCKOBJ_STATE_UNLOCK;
 #else /* POK_NEEDS_SCHED_O1_SPLIT is defined*/
	/* reset the bit of the new current thread */
	obj->waiting_threads &= ~(1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low));
	/* if the unlocked thread is a successor reset its bit in executed_predecessor */
	POK_CURRENT_PARTITION.executed_predecessors &= ~(1 << (POK_CURRENT_THREAD.pos - POK_CURRENT_PARTITION.thread_index_low));
 #endif

	/* Here, we come back after we wait*/
	if ((timeout != 0 ) && (POK_GETTICK() >= timeout))
	{
		ret = POK_ERRNO_TIMEOUT;
	}
	else
	{
		ret = POK_ERRNO_OK;
	}
	//SPIN_UNLOCK (obj->eventspin);
	return ret;
}

/*
*	Scan the array of threads and unlock the FIRST thread is waiting for
*	the event associated to the lock object.
* 	NO priotiy used
*/
pok_ret_t pok_lockobj_eventsignal (pok_lockobj_t* obj)
{

	//SPIN_LOCK (obj->eventspin);
 #ifndef POK_NEEDS_SCHED_O1_SPLIT
	uint32_t tmp;
	//pok_ret_t ret;
	//int found = 0; FIXME: variable set but not used

	for (tmp = 0 ; tmp < POK_CONFIG_NB_THREADS ; tmp++)
	{
		if (tmp == POK_SCHED_CURRENT_THREAD)
			continue;

		if (obj->thread_state[tmp] == LOCKOBJ_STATE_WAITEVENT)
		{
			//found = 1; FIXME: variable set but not used
  #ifdef DEBUG_LOCK
			printf("unlocking th %i\n", tmp);
  #endif
			// Update the lockobj state
			obj->thread_state[tmp] = LOCKOBJ_STATE_UNLOCK;
			pok_sched_unlock_thread (tmp);
			//SPIN_UNLOCK (obj->eventspin);
			obj->is_locked = FALSE;
			return POK_ERRNO_OK;
		}
 	} /* for */
 	obj->is_locked = FALSE;
	/* SPIN_UNLOCK (obj->eventspin); */
	return POK_ERRNO_NOTFOUND;

 #else /* POK_NEEEDS_SCHED_O1_SPLIT is defined */

	/* set the event UP */
	obj->is_locked &= 0;

	/* check if there is a thread waiting for the signal; if not return NOTFOUND */
	if (obj->waiting_threads == 0)
		return POK_ERRNO_NOTFOUND;

	uint64_t current_time = POK_CLOCK();
	/* get the index of the first waiting thread and create its bitmask */
	uint32_t waiting_thread_bitmask = 1 << get_thread_index_deBruijn(obj->waiting_threads);
	/* if this thread is a successor it needs to be unlocked only if its predecessor has executed */
	uint32_t current_runnables = waiting_thread_bitmask & POK_CURRENT_PARTITION.executed_predecessors;
	/* update the state of the thread to unlock in the lock object (resetting its bit in the waiting_threads bitmask) */
	obj->waiting_threads &= ~(current_runnables);
	/* if the thread that needs to be unlocked is a successor, reset its bit in the executed_predecessos bitmask:
	 * this because we want that the thread waits for its predecessor for the next actication
	 */
	POK_CURRENT_PARTITION.executed_predecessors &= ~(current_runnables & POK_CURRENT_PARTITION.successors);
	/* unlock the thread: if the thread to unlock is sporadic (a successor or not) we have to add it
	 * to the ready queue (not in runnables bitmask!); otherwise we have to set its bit in runnables
	 */
	pok_sched_unlock_threads_by_bitmask(current_runnables & ~(POK_CURRENT_PARTITION.sporadic_bitmask));
	pok_sched_insert_sporadic_in_queue(current_runnables & POK_CURRENT_PARTITION.sporadic_bitmask);
	/* if the thread to be unlocked is sporadic update its absolute deadline: VARIABILITY!!! */
	int sporadic_index;
	if ((current_runnables & POK_CURRENT_PARTITION.sporadic_bitmask) != 0)
	{
		sporadic_index = get_thread_index_deBruijn(current_runnables) + POK_CURRENT_PARTITION.thread_index_low;
		pok_threads[sporadic_index].deadline = current_time + pok_threads_deadline[pok_threads[sporadic_index].id];
	}

	return POK_ERRNO_OK;
 #endif /* end ifndef POK_NEEDS_SCHED_O1_SPLIT */
}

/*
*	Unlock ALL threads that are waiting for the event associated to the lock object
*	(e.g. message in a blackboard/buffer, free space in a buffer)
*/
pok_ret_t pok_lockobj_eventbroadcast (pok_lockobj_t* obj)
{
 #ifndef POK_NEEDS_SCHED_O1_SPLIT

	uint32_t tmp;
	//SPIN_LOCK (obj->eventspin);

	for (tmp = 0 ; tmp < POK_CONFIG_NB_THREADS ; tmp++)
	{
		if (tmp == POK_SCHED_CURRENT_THREAD)
			continue;

		if (obj->thread_state[tmp] == LOCKOBJ_STATE_WAITEVENT)
		{
  #ifdef DEBUG_LOCK
			printf("unlockiing th %i\n",tmp);
  #endif
			pok_sched_unlock_thread (tmp);
		}
	}
	obj->is_locked = FALSE;
	//SPIN_UNLOCK (obj->eventspin);

 #else /* POK_NEEDS_SCHED_O1_SPLIT is defined */

	int current_thread_position;
	int current_thread_bitmask;
	uint64_t current_time = POK_CLOCK();
	/* with the split solution the threads that needs to be unlocked are those which are waiting
	 * on the lock object and, if they are successors, which have the predecessor that has executed
	 */
	int current_runnables = obj->waiting_threads & POK_CURRENT_PARTITION.executed_predecessors;
	/* update the state of the threads to be unlocked resetting their bit in the waiting_threads bitmask */
	obj->waiting_threads &= ~(current_runnables);
	/* iterate on waiting threads */
	while (current_runnables != 0)
	{
		/* get the position of the first waiting thread */
		current_thread_position = get_thread_index_deBruijn(current_runnables) + POK_CURRENT_PARTITION.thread_index_low;
		current_thread_bitmask = (1 << current_thread_position);
		/* if the thread to be unlocked is a successor reset its bit in the executed_predecessors bitmask */
		POK_CURRENT_PARTITION.executed_predecessors &= ~(current_thread_bitmask & POK_CURRENT_PARTITION.successors);
		/* unlock the thread: if the thread to unlock is sporadic (a successor or not) we have to add it
		 * to the ready queue (not in runnables bitmask!); otherwise we have to set its bit in runnables
		 */
		pok_sched_unlock_threads_by_bitmask(current_thread_bitmask & ~(POK_CURRENT_PARTITION.sporadic_bitmask));
		pok_sched_insert_sporadic_in_queue(current_thread_bitmask & POK_CURRENT_PARTITION.sporadic_bitmask);
		/* if the thread to be unlocked is sporadic update its absolute deadline: VARIABILITY!!! */
		if ((current_thread_bitmask & POK_CURRENT_PARTITION.sporadic_bitmask) != 0)
		{
			pok_threads[current_thread_position].deadline = current_time +
		pok_threads_deadline[pok_threads[current_thread_position].id];
		}
		/* update current_runnables bitmask */
		current_runnables &= ~(current_thread_bitmask);
	}
	/* unlock the lock object */
	obj->is_locked = 0;
 #endif /* end ifndef POK_NEEDS_SCHED_O1_SPLIT */

	return POK_ERRNO_OK;
}

/*
*	pok_lockobj_lock
*/
pok_ret_t pok_lockobj_lock (pok_lockobj_t* obj, const pok_lockobj_lockattr_t* attr)
{
	//uint64_t timeout = 0;

	if (obj->initialized == FALSE)
	{
 #ifdef DEBUG_LOCK
		printf("pok_lockobj_lock-NOT init\n");
 #endif
		return POK_ERRNO_LOCKOBJ_NOTREADY;
	}

//	SPIN_LOCK (obj->spin);
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
	if ( (obj->is_locked == 0 ) && (obj->thread_state[POK_SCHED_CURRENT_THREAD] == LOCKOBJ_STATE_UNLOCK ))
 #else
	if ( (obj->is_locked == FALSE ) && (obj->thread_state[POK_SCHED_CURRENT_THREAD] == LOCKOBJ_STATE_UNLOCK ))
 #endif
	{
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
		obj->is_locked = ~(0);
 #else
		obj->is_locked = TRUE;
 #endif
		/*SPIN_UNLOCK (obj->spin);*/
	}
	else
	{
		/*
		 * attr->time corresponds to the timeout for the waiting object
		 */
		if ((attr != POK_NULL) && (attr->time > 0))
		{
			//timeout = attr->time; /* NO SUPPORT FOR TIMEOUT RIGHT NOW */
		}
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
		while ( (obj->is_locked != 0 ) || (obj->thread_state[POK_SCHED_CURRENT_THREAD] == LOCKOBJ_STATE_LOCK))
 #else
		while ( (obj->is_locked == TRUE ) || (obj->thread_state[POK_SCHED_CURRENT_THREAD] == LOCKOBJ_STATE_LOCK))
 #endif
		{
			obj->thread_state[POK_SCHED_CURRENT_THREAD] = LOCKOBJ_STATE_LOCK;

			/* NO SUPPORT FOR TIMEOUT RIGHT NOW */

			pok_sched_lock_current_thread ();

//			SPIN_UNLOCK (obj->spin);

		}

		switch (obj->kind)
		{
			default:
			{
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
				obj->is_locked = ~(0);
 #else
				obj->is_locked = TRUE;
 #endif
				break;
			}
		}
		pok_sched_unlock_thread (POK_SCHED_CURRENT_THREAD);
	}

	return POK_ERRNO_OK;
}

/*
*	pok_lockobj_unlock
*/
pok_ret_t pok_lockobj_unlock (pok_lockobj_t* obj, const pok_lockobj_lockattr_t* attr)
{

	uint32_t res;
	(void) attr;			/* unused at this time */

	if (obj->initialized == FALSE)
	{
		return POK_ERRNO_LOCKOBJ_NOTREADY;
	}

	res = 0;
	//SPIN_LOCK (obj->spin);

	switch (obj->kind)
	{
		default:
		{
 #ifdef POK_NEEDS_SCHED_O1_SPLIT
			obj->is_locked = 0;
 #else
			obj->is_locked = FALSE;
 #endif
			break;
		}
	}

	res = POK_SCHED_CURRENT_THREAD;
	res = (res + 1) % (POK_CONFIG_NB_THREADS);

	do
	{
		if (obj->thread_state[res] == LOCKOBJ_STATE_LOCK)
		{
			obj->thread_state[res] = LOCKOBJ_STATE_UNLOCK;
			pok_sched_unlock_thread (res);
			break;
		}
		res = (res + 1) % (POK_CONFIG_NB_THREADS);
	}
	while ((res != POK_SCHED_CURRENT_THREAD));

	obj->thread_state[POK_SCHED_CURRENT_THREAD] = LOCKOBJ_STATE_UNLOCK;
	//SPIN_UNLOCK (obj->spin);

	return POK_ERRNO_OK;
}

 #ifdef POK_NEEDS_LOCKOBJECTS
pok_ret_t pok_lockobj_partition_wrapper (const pok_lockobj_id_t id, const pok_lockobj_lockattr_t* attr)
{
	/* First, we check that the locked object belongs to the partition
	 * If not, we return an error
	 */
	pok_ret_t ret;

	if (id < pok_partitions[POK_SCHED_CURRENT_PARTITION].lockobj_index_low)
	{
		return POK_ERRNO_EINVAL;
	}

	if ( id >= pok_partitions[POK_SCHED_CURRENT_PARTITION].lockobj_index_high)
	{
		return POK_ERRNO_EINVAL;
	}

	if (pok_partitions_lockobjs[id].kind != attr->obj_kind)
	{
		return POK_ERRNO_EINVAL;
	}

	switch (attr->operation)
	{
		case LOCKOBJ_OPERATION_LOCK:
			ret = pok_lockobj_lock (&pok_partitions_lockobjs[id], attr);
			return ret;
			break;

		case LOCKOBJ_OPERATION_UNLOCK:
			{
				ret = pok_lockobj_unlock (&pok_partitions_lockobjs[id], attr);
				return ret;
				break;
			}

		case LOCKOBJ_OPERATION_WAIT:
			{
				ret = pok_lockobj_eventwait (&pok_partitions_lockobjs[id], attr->time);
				return ret;
				break;
			}

		case LOCKOBJ_OPERATION_SIGNAL:
			{
				ret = pok_lockobj_eventsignal (&pok_partitions_lockobjs[id]);
				return ret;
				break;
			}

		case LOCKOBJ_OPERATION_BROADCAST:
			{
				ret = pok_lockobj_eventbroadcast (&pok_partitions_lockobjs[id]);
				return ret;
				break;
			}

		default:
			return POK_ERRNO_EINVAL;
	}
	return POK_ERRNO_EINVAL;
}
 #endif /* POK_NEEDS_LOCKOBJECTS */

#endif /* defined (POK_NEEDS_LOCKOBJECTS) || defined (POK_NEEDS_PORTS_QUEUEING) || defined (POK_NEEDS_PORTS_SAMPLING) */

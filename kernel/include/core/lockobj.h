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


#ifndef __POK_KERNEL_LOCKOBJ_H__
#define __POK_KERNEL_LOCKOBJ_H__

#include <types.h>
#include <arch.h>

#ifndef POK_CONFIG_NB_LOCKOBJECTS
#define POK_CONFIG_NB_LOCKOBJECTS 0
#endif

/*
 * In POK, all locking objects are implemented in the same way. It avoids
 * code duplication and save the environnement because we will consume less
 * space on your disk and will save trees because stupid people that prints
 * the code will consume less paper.
 * Moreoever, if all lockobjects share the same code, it will be easy to
 * certify and verify the behavior of all.
 */


typedef enum
{
//	POK_LOCKOBJ_KIND_MUTEX 		= 1,
//	POK_LOCKOBJ_KIND_SEMAPHORE	= 2,
	POK_LOCKOBJ_KIND_EVENT		= 3 	/* only this lockobj kind is used at the moment */
}pok_lockobj_kind_t;

/* All kind of lock objects we have in the kernel */

typedef enum
{
	POK_LOCKOBJ_POLICY_STANDARD = 0,
}pok_locking_policy_t;


typedef enum
{
	LOCKOBJ_STATE_LOCK 		= 0,
	LOCKOBJ_STATE_UNLOCK 	= 1,
	LOCKOBJ_STATE_WAITEVENT = 2
}pok_mutex_state_t;

typedef struct
{
	pok_lockobj_kind_t			kind;
	pok_locking_policy_t		locking_policy;
	pok_queueing_discipline_t	queueing_policy;  
//	pok_sem_value_t			initial_value;
//	pok_sem_value_t			max_value;	/* semaphores not implemented at the moment */
}pok_lockobj_attr_t;

typedef struct
{
//	pok_spinlock_t			spin;		/* spin lock not used at the moment */
//	pok_spinlock_t			eventspin; 	/* spin lock not used at the moment */
/* spinlock to enfoce mutual exclusion */

#ifdef POK_NEEDS_SCHED_O1_SPLIT
	/* if the split solution is used, we use a bitmask to say if the event is locked:
	  - all 1s means event is locked
	  - all 0s means event is up */
	uint32_t					is_locked;
#else
	bool_t						is_locked;
#endif
	
	pok_mutex_state_t			thread_state[POK_CONFIG_NB_THREADS + 2];  
	/* Describe which thread is blocked in the mutex */

#ifdef POK_NEEDS_SCHED_O1_SPLIT
	uint32_t					waiting_threads;
	/* Bitmask to store which threads (of the current partition) are waiting 
	  on the lockobject
	 */
#endif

	pok_locking_policy_t		locking_policy;
	/* Locking policy */

	pok_queueing_discipline_t	queueing_policy;
	/* Locking policy */

	pok_lockobj_kind_t			kind;
	
	bool_t						initialized;
	/* Is the mutex initialized ? */

	uint16_t					current_value;
	uint16_t					max_value;
} pok_lockobj_t;


/* NOT used at the moment */
/* 
typedef enum
{
	LOCKOBK_LOCK_REGULAR	= 1,
	LOCKOBJ_LOCK_TIMED	= 2
}pok_lockobj_lock_kind_t; 
*/

typedef enum
{
	LOCKOBJ_OPERATION_LOCK		= 1,
	LOCKOBJ_OPERATION_UNLOCK	= 2,
	LOCKOBJ_OPERATION_WAIT		= 3,
	LOCKOBJ_OPERATION_SIGNAL	= 4,
	LOCKOBJ_OPERATION_BROADCAST = 5
}pok_lockobj_operation_t;

typedef struct
{
	pok_lockobj_operation_t	operation;
	pok_lockobj_kind_t	obj_kind;
//	pok_lockobj_lock_kind_t	lock_kind; /*NOT USED at the moment*/
	uint64_t		time;
}pok_lockobj_lockattr_t;


pok_ret_t pok_lockobj_create (pok_lockobj_t* obj, const pok_lockobj_attr_t* attr);
pok_ret_t pok_lockobj_init ();
pok_ret_t pok_lockobj_partition_create (pok_lockobj_id_t* id, const pok_lockobj_attr_t* attr);
pok_ret_t pok_lockobj_lock (pok_lockobj_t* obj, const pok_lockobj_lockattr_t* attr);
pok_ret_t pok_lockobj_unlock (pok_lockobj_t* obj, const pok_lockobj_lockattr_t* attr);
pok_ret_t pok_lockobj_eventwait (pok_lockobj_t* obj, const uint64_t timeout);
pok_ret_t pok_lockobj_eventsignal (pok_lockobj_t* obj);
pok_ret_t pok_lockobj_eventbroadcast (pok_lockobj_t* obj);
pok_ret_t pok_lockobj_partition_wrapper (const pok_lockobj_id_t id, const pok_lockobj_lockattr_t* attr);

#endif

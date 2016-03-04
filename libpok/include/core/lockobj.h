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


#ifndef __POK_LIBPOK_LOCKOBJ_H__
#define __POK_LIBPOK_LOCKOBJ_H__

#include <types.h>

typedef enum
{
//	POK_LOCKOBJ_KIND_MUTEX = 1, 	/* NOT USED at the moment */
//	POK_LOCKOBJ_KIND_SEMAPHORE = 2, /* NOT USED at the moment */
	POK_LOCKOBJ_KIND_EVENT = 3	/* only this lockobj kind is used at the moment */
}pok_lockobj_kind_t;

typedef enum
{
	POK_LOCKOBJ_POLICY_STANDARD = 0	/* only this lockobj policy used at the moment */
}pok_locking_policy_t;


typedef struct
{
	pok_lockobj_kind_t			kind;
	pok_locking_policy_t		locking_policy;
	pok_queueing_discipline_t	queueing_policy;  
//	pok_sem_value_t			initial_value;	/* semaphores not implemented at the moment */
//	pok_sem_value_t			max_value;	/* semaphores not implemented at the moment */
}pok_lockobj_attr_t;

/* NOT used at the moment */
/*
typedef enum
{
	LOCKOBK_LOCK_REGULAR = 1,
	LOCKOBJ_LOCK_TIMED	= 2
}pok_lockobj_lock_kind_t;
*/

typedef enum
{
	LOCKOBJ_OPERATION_LOCK		= 1,
	LOCKOBJ_OPERATION_UNLOCK	= 2,
	LOCKOBJ_OPERATION_WAIT		= 3,
	LOCKOBJ_OPERATION_SIGNAL	= 4,
	LOCKOBJ_OPERATION_BROADCAST	= 5
}pok_lockobj_operation_t;

typedef struct
{
	pok_lockobj_operation_t	operation;
	pok_lockobj_kind_t		obj_kind;
//	pok_lockobj_lock_kind_t	lock_kind; /*NOT USED at the moment*/
	uint64_t				time;
}pok_lockobj_lockattr_t;


#endif

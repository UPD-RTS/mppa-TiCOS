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

#ifdef POK_NEEDS_ARINC653_PROCESS

#include <arinc653/types.h>

#ifndef APEX_PROCESS
#define APEX_PROCESS

#define MAX_NUMBER_OF_PROCESSES  SYSTEM_LIMIT_NUMBER_OF_PROCESSES
#define MIN_PRIORITY_VALUE		 1
#define MAX_PRIORITY_VALUE		 63
#define MAX_LOCK_LEVEL			 16

typedef NAME_TYPE			PROCESS_NAME_TYPE;

typedef APEX_INTEGER		PROCESS_ID_TYPE;

typedef APEX_INTEGER		LOCK_LEVEL_TYPE;

typedef APEX_UNSIGNED		STACK_SIZE_TYPE;

typedef APEX_INTEGER		WAITING_RANGE_TYPE;

typedef APEX_INTEGER		PRIORITY_TYPE;

typedef enum
{
	DORMANT	= 0,
	READY	= 1,
	RUNNING = 2,
	WAITING = 3
} PROCESS_STATE_TYPE;

typedef enum
{
	SOFT = 0,
	HARD = 1
} DEADLINE_TYPE;

typedef
	struct {
		PROCESS_NAME_TYPE	NAME;
		SYSTEM_ADDRESS_TYPE	ENTRY_POINT;
		STACK_SIZE_TYPE		STACK_SIZE;
		PRIORITY_TYPE		BASE_PRIORITY;
		SYSTEM_TIME_TYPE	PERIOD;
		SYSTEM_TIME_TYPE	TIME_CAPACITY;
		DEADLINE_TYPE		DEADLINE;
	} PROCESS_ATTRIBUTE_TYPE;


typedef struct {
	TIME_TYPE			DEADLINE_TIME;
	PRIORITY_TYPE		CURRENT_PRIORITY;
	PROCESS_STATE_TYPE	PROCESS_STATE;
	PROCESS_ATTRIBUTE_TYPE	 ATTRIBUTES;
} PROCESS_STATUS_TYPE;

extern void CREATE_PROCESS
		(const PROCESS_ATTRIBUTE_TYPE * const	/* in */	ATTRIBUTE, 
		PROCESS_ID_TYPE * const					/* out */	PROCESS_ID, 
		RETURN_CODE_TYPE * const				/* out */	RETURN_CODE);

extern void SET_PRIORITY (
		/*in */ PROCESS_ID_TYPE		PROCESS_ID,
		/*in */ PRIORITY_TYPE		PRIORITY,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void SUSPEND_SELF (
		/*in */ SYSTEM_TIME_TYPE	TIME_OUT,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void SUSPEND (
		/*in */ PROCESS_ID_TYPE		PROCESS_ID,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void RESUME (
		/*in */ PROCESS_ID_TYPE		PROCESS_ID,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void STOP_SELF ();

extern void STOP
		(const PROCESS_ID_TYPE		/* in */	PROCESS_ID, 
		RETURN_CODE_TYPE * const	/* out */	RETURN_CODE);

extern void START (
		/*in */ PROCESS_ID_TYPE	  	PROCESS_ID,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void DELAYED_START (
		/*in */ PROCESS_ID_TYPE		PROCESS_ID,
		/*in */ SYSTEM_TIME_TYPE	DELAY_TIME,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void LOCK_PREEMPTION (
		/*out*/ LOCK_LEVEL_TYPE		*LOCK_LEVEL,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void UNLOCK_PREEMPTION (
		/*out*/ LOCK_LEVEL_TYPE		*LOCK_LEVEL,
		/*out*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void GET_MY_ID(
		PROCESS_ID_TYPE *  const 	/* out */	PROCESS_ID, 
		RETURN_CODE_TYPE *  const 	/* out */	RETURN_CODE);

extern void GET_PROCESS_ID
		(const PROCESS_NAME_TYPE  	/* in */	PROCESS_NAME, 
		PROCESS_ID_TYPE * const		/* out */	PROCESS_ID, 
		RETURN_CODE_TYPE * const  	/* out */	RETURN_CODE);

extern void GET_PROCESS_STATUS
		(const PROCESS_ID_TYPE		/* in */	PROCESS_ID, 
		PROCESS_STATUS_TYPE * const	/* out */	PROCESS_STATUS, 
		RETURN_CODE_TYPE * const    /* out */	RETURN_CODE);

#endif
#endif

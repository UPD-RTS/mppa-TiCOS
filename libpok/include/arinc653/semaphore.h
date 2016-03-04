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

#ifdef POK_NEEDS_ARINC653_SEMAPHORE
#ifndef APEX_SEMAPHORE
#define APEX_SEMAPHORE

#ifndef POK_NEEDS_ARINC653_PROCESS
#define POK_NEEDS_ARINC653_PROCESS 1
#endif

#include <arinc653/types.h>
#include <arinc653/process.h>

/* SEMAPHORE constant definitions */
/* max nb of semaphores */
#define MAX_NUMBER_OF_SEMAPHORES 63
/* max semaphore value */
#define MAX_SEMAPHORE_VALUE 32767
/* SEMAPHORE type definitions */

typedef APEX_INTEGER SEMAPHORE_ID_TYPE;

/* semaphore value type */
typedef APEX_INTEGER SEMAPHORE_VALUE_TYPE;

/* semaphore name type */
typedef NAME_TYPE SEMAPHORE_NAME_TYPE;

/* semaphore status type */
typedef
struct {
	SEMAPHORE_VALUE_TYPE CURRENT_VALUE;
	SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE;
	WAITING_RANGE_TYPE WAITING_PROCESSES;
} SEMAPHORE_STATUS_TYPE;

/* semaphore management services */
extern void CREATE_SEMAPHORE (
		/*IN */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
		/*IN */ SEMAPHORE_VALUE_TYPE CURRENT_VALUE,
		/*IN */ SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE,
		/*IN */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
		/*OUT*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void WAIT_SEMAPHORE (
		/*IN */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
		/*IN */ SYSTEM_TIME_TYPE TIME_OUT,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void SIGNAL_SEMAPHORE (
		/*IN */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_SEMAPHORE_ID (
		/*IN */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
		/*OUT*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_SEMAPHORE_STATUS (
		/*IN */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
		/*OUT*/ SEMAPHORE_STATUS_TYPE *SEMAPHORE_STATUS,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

#endif
#endif

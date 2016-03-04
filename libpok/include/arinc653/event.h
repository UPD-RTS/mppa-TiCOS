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


#ifdef POK_NEEDS_ARINC653_EVENT

#ifndef APEX_EVENT
#define APEX_EVENT


#ifndef POK_NEEDS_ARINC653_PROCESS
#define POK_NEEDS_ARINC653_PROCESS 1
#endif

#include <arinc653/process.h>
/* EVENT constant definitions	*/
/* max nb of events */
#define MAX_NUMBER_OF_EVENTS 63
/* EVENT type definitions */
/* event ident type */
typedef APEX_INTEGER EVENT_ID_TYPE;
/* event state type */
enum EVENT_STATE_VALUE_TYPE { UP, DOWN };

typedef enum EVENT_STATE_VALUE_TYPE EVENT_STATE_TYPE;

/* event name type */
typedef NAME_TYPE EVENT_NAME_TYPE;
/* event status type */
typedef struct {
	EVENT_STATE_TYPE	EVENT_STATE;
	WAITING_RANGE_TYPE	WAITING_PROCESSES;
} EVENT_STATUS_TYPE;

/* event management services */
extern void CREATE_EVENT (
		/*IN */ EVENT_NAME_TYPE 	EVENT_NAME,
		/*OUT*/ EVENT_ID_TYPE		*EVENT_ID,
		/*OUT*/ RETURN_CODE_TYPE	*RETURN_CODE );

extern void SET_EVENT (
		/*IN */ EVENT_ID_TYPE EVENT_ID,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void RESET_EVENT (
		/*IN */ EVENT_ID_TYPE EVENT_ID,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void WAIT_EVENT (
		/*IN */ EVENT_ID_TYPE EVENT_ID,
		/*IN */ SYSTEM_TIME_TYPE TIME_OUT,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_EVENT_ID (
		/*IN */ EVENT_NAME_TYPE EVENT_NAME,
		/*OUT*/ EVENT_ID_TYPE *EVENT_ID,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_EVENT_STATUS (
		/*IN */ EVENT_ID_TYPE EVENT_ID,
		/*OUT*/ EVENT_STATUS_TYPE *EVENT_STATUS ,
		/*OUT*/ RETURN_CODE_TYPE *RETURN_CODE );

#endif
#endif

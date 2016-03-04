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


#ifdef POK_NEEDS_ARINC653_BLACKBOARD

/*-------------------------------------------------------------------*/ 
/*																	 */ 
/* BLACKBOARD constant and type definitions and management services  */ 
/*																	*/ 
/*-------------------------------------------------------------------*/ 
 
#ifndef APEX_BLACKBOARD 
#define APEX_BLACKBOARD 

#ifndef POK_NEEDS_ARINC653_PROCESS
#define POK_NEEDS_ARINC653_PROCESS
#endif

#include <arinc653/types.h>
#include <arinc653/process.h>
 
#define  MAX_NUMBER_OF_BLACKBOARDS		SYSTEM_LIMIT_NUMBER_OF_BLACKBOARDS 
 
typedef  NAME_TYPE		 BLACKBOARD_NAME_TYPE; 
 
typedef  APEX_INTEGER	 BLACKBOARD_ID_TYPE; 
 
typedef  enum { EMPTY = 0, OCCUPIED = 1 } EMPTY_INDICATOR_TYPE; 
 
typedef struct { 
	EMPTY_INDICATOR_TYPE	EMPTY_INDICATOR; 
	MESSAGE_SIZE_TYPE		MAX_MESSAGE_SIZE; 
	WAITING_RANGE_TYPE		WAITING_PROCESSES; 
} BLACKBOARD_STATUS_TYPE; 
 
extern void CREATE_BLACKBOARD ( 
		 /*in */ BLACKBOARD_NAME_TYPE	BLACKBOARD_NAME, 
		 /*in */ MESSAGE_SIZE_TYPE		MAX_MESSAGE_SIZE, 
		 /*out*/ BLACKBOARD_ID_TYPE		*BLACKBOARD_ID, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE ); 
 
extern void DISPLAY_BLACKBOARD ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*in */ MESSAGE_ADDR_TYPE		MESSAGE_ADDR,		 /* by reference */ 
		 /*in */ MESSAGE_SIZE_TYPE		LENGTH, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE ); 
 
extern void READ_BLACKBOARD ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*in */ SYSTEM_TIME_TYPE		TIME_OUT, 
		 /*out*/ MESSAGE_ADDR_TYPE		MESSAGE_ADDR, 
		 /*out*/ MESSAGE_SIZE_TYPE		*LENGTH, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE ); 
 
extern void CLEAR_BLACKBOARD ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE ); 
 
extern void GET_BLACKBOARD_ID ( 
		 /*in */ BLACKBOARD_NAME_TYPE	BLACKBOARD_NAME, 
		 /*out*/ BLACKBOARD_ID_TYPE		*BLACKBOARD_ID, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE ); 
 
extern void GET_BLACKBOARD_STATUS ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*out*/ BLACKBOARD_STATUS_TYPE	*BLACKBOARD_STATUS, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE ); 
 
#endif 

#endif

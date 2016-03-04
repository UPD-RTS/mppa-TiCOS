/*
 * Copyright (c) 2012  University of Padua, Department of Mathematics
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

#include <arinc653/types.h>
#include <arinc653/blackboard.h>
#include <middleware/blackboard.h>

void CREATE_BLACKBOARD ( 
		 /*in */ BLACKBOARD_NAME_TYPE	BLACKBOARD_NAME, 
		 /*in */ MESSAGE_SIZE_TYPE		MAX_MESSAGE_SIZE, 
		 /*out*/ BLACKBOARD_ID_TYPE		*BLACKBOARD_ID, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	pok_blackboard_id_t	core_id;
	pok_ret_t			core_ret;

	core_ret 		= pok_blackboard_create (BLACKBOARD_NAME, MAX_MESSAGE_SIZE, &core_id);
	*BLACKBOARD_ID	= core_id;
	*RETURN_CODE	= core_ret;
}
 
void DISPLAY_BLACKBOARD ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*in */ MESSAGE_ADDR_TYPE		MESSAGE_ADDR,		 /* by reference */ 
		 /*in */ MESSAGE_SIZE_TYPE		LENGTH, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	pok_ret_t core_ret;
	core_ret 		= pok_blackboard_display (BLACKBOARD_ID, MESSAGE_ADDR, LENGTH);
	*RETURN_CODE 	= core_ret;
}
 
void READ_BLACKBOARD ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*in */ SYSTEM_TIME_TYPE		TIME_OUT, 
		 /*out*/ MESSAGE_ADDR_TYPE		MESSAGE_ADDR, 
		 /*out*/ MESSAGE_SIZE_TYPE		*LENGTH, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	pok_ret_t core_ret;
	uint64_t timeout = TIME_OUT / 10;  // expressed in 100 us, converted ms
	core_ret = pok_blackboard_read (BLACKBOARD_ID, timeout, MESSAGE_ADDR, LENGTH);
	*RETURN_CODE = core_ret;
}
 
void CLEAR_BLACKBOARD ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{

	pok_ret_t core_ret;
	core_ret = pok_blackboard_clear (BLACKBOARD_ID);	
	*RETURN_CODE = core_ret;
}
 
void GET_BLACKBOARD_ID ( 
		 /*in */ BLACKBOARD_NAME_TYPE	BLACKBOARD_NAME, 
		 /*out*/ BLACKBOARD_ID_TYPE		*BLACKBOARD_ID, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	(void) BLACKBOARD_NAME;
	(void) BLACKBOARD_ID;
	*RETURN_CODE = NOT_AVAILABLE;
}
 
void GET_BLACKBOARD_STATUS ( 
		 /*in */ BLACKBOARD_ID_TYPE		BLACKBOARD_ID, 
		 /*out*/ BLACKBOARD_STATUS_TYPE	*BLACKBOARD_STATUS, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	pok_ret_t 					core_ret;
	pok_blackboard_status_t*	status;

	core_ret = pok_blackboard_status (BLACKBOARD_ID,status);
	if (core_ret == POK_ERRNO_OK){
		BLACKBOARD_STATUS->EMPTY_INDICATOR		= status->empty;
		BLACKBOARD_STATUS->MAX_MESSAGE_SIZE 	= status->msg_size;
		BLACKBOARD_STATUS->WAITING_PROCESSES	= status->waiting_processes;
	}	
	*RETURN_CODE = core_ret;
}
#endif

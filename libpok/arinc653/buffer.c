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

#ifdef POK_NEEDS_ARINC653_BUFFER
#include <arinc653/types.h>
#include <arinc653/buffer.h>
#include <types.h>
#include <middleware/port.h>
#include <middleware/buffer.h>
#include <libc/string.h>
#include <libc/stdio.h>

/********************************************************************
* SERVICE  CREATE_BUFFER
********************************************************************/
void CREATE_BUFFER ( 
		 /*in */ BUFFER_NAME_TYPE		BUFFER_NAME, 
		 /*in */ MESSAGE_SIZE_TYPE		MAX_MESSAGE_SIZE, 
		 /*in */ MESSAGE_RANGE_TYPE		MAX_NB_MESSAGE, 
		 /*in */ QUEUING_DISCIPLINE_TYPE	QUEUING_DISCIPLINE, 
		 /*out*/ BUFFER_ID_TYPE			*BUFFER_ID, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	pok_ret_t					core_ret;
	pok_buffer_id_t				core_id;
	pok_queueing_discipline_t	core_discipline;

	// TODO Reorder pok and arinc enumerations to correctly map into each other
	switch (QUEUING_DISCIPLINE)
	{
		case PRIORITY:
			core_discipline = POK_PORT_QUEUEING_DISCIPLINE_PRIORITY;
		break;

		case FIFO:
			core_discipline = POK_PORT_QUEUEING_DISCIPLINE_FIFO;
		break;

		default:
			*RETURN_CODE = INVALID_PARAM;
			printf ("CREATE_BUFFER: Invalid param\n");
		return;
	}

	if (MAX_NB_MESSAGE > POK_CONFIG_MAX_BUFFERS_MESSAGES)
	{
		 printf("CREATE_BUFFER: Max number of buffers out of range\n");			
		 return INVALID_PARAM;
	}

	core_ret = pok_buffer_create (BUFFER_NAME, MAX_NB_MESSAGE, MAX_MESSAGE_SIZE, core_discipline, &core_id);

	*BUFFER_ID = core_id;

	*RETURN_CODE = core_ret;
}

/********************************************************************
* SERVICE  SEND_BUFFER
********************************************************************/
void SEND_BUFFER ( 
		 /*in */ BUFFER_ID_TYPE			BUFFER_ID, 
		 /*in */ MESSAGE_ADDR_TYPE		MESSAGE_ADDR,		 /* by reference */ 
		 /*in */ MESSAGE_SIZE_TYPE		LENGTH, 
		 /*in */ SYSTEM_TIME_TYPE		TIME_OUT, 
		 /*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
		pok_ret_t core_ret;
#ifdef DEBUG_BF
		printf ("->SEND_BUFFER\n");
#endif
		uint64_t timeout = TIME_OUT / 10;  // expressed in 100 us, converted ms
		core_ret = pok_buffer_send (BUFFER_ID, MESSAGE_ADDR, LENGTH, timeout);
		*RETURN_CODE = core_ret;
}

/********************************************************************
* SERVICE  RECEIVE_BUFFER
********************************************************************/
void RECEIVE_BUFFER ( 
		/*in */ BUFFER_ID_TYPE			BUFFER_ID, 
		/*in */ SYSTEM_TIME_TYPE		TIME_OUT, 
		/*out*/ MESSAGE_ADDR_TYPE		MESSAGE_ADDR, 
		/*out*/ MESSAGE_SIZE_TYPE		*LENGTH, 
		/*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	pok_ret_t 		core_ret;
	pok_port_size_t core_size;
#ifdef DEBUG_BF
	printf ("->RECEIVE_BUFFER\n");
#endif
	uint64_t timeout 	= TIME_OUT / 10;  // expressed in 100 us, converted ms	
	core_ret 			= pok_buffer_receive (BUFFER_ID, timeout, MESSAGE_ADDR, &core_size);
	*LENGTH 			= (APEX_INTEGER) core_size;
	*RETURN_CODE 		= core_ret;
}

/********************************************************************
* SERVICE  GET_BUFFER_ID
********************************************************************/
void GET_BUFFER_ID ( 
		/*in */ BUFFER_NAME_TYPE		BUFFER_NAME, 
		/*out*/ BUFFER_ID_TYPE			*BUFFER_ID, 
		/*out*/ RETURN_CODE_TYPE		*RETURN_CODE )
{
	(void) BUFFER_NAME;
	(void) BUFFER_ID;
	*RETURN_CODE = NOT_AVAILABLE;
}

/********************************************************************
* SERVICE  GET_BUFFER_STATUS
********************************************************************/
void GET_BUFFER_STATUS ( 
		/*in */ BUFFER_ID_TYPE			  BUFFER_ID, 
		/*out*/ BUFFER_STATUS_TYPE		 *BUFFER_STATUS, 
		/*out*/ RETURN_CODE_TYPE			*RETURN_CODE )
{
	(void) BUFFER_ID;
	(void) BUFFER_STATUS;
	*RETURN_CODE = NOT_AVAILABLE;
}
 
#endif 

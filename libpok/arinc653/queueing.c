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

#include <core/dependencies.h>

#ifdef POK_NEEDS_ARINC653_QUEUEING

#include <types.h>
#include <middleware/port.h>
#include <arinc653/types.h>
#include <arinc653/queueing.h>


extern unsigned char *input_buffers_queuing[];

uint8_t input_buffers_queuing_index=0; 
 #ifndef POK_NEEDS_PORTS_QUEUEING_DYNAMIC
extern MESSAGE_ADDR_TYPE *receiving_addresses[];
 #endif

/********************************************************************
* SERVICE  CREATE_QUEUING_PORT
*********************************************************************/
void CREATE_QUEUING_PORT
	(const QUEUING_PORT_NAME_TYPE	/* in */	QUEUING_PORT_NAME, 
	const MESSAGE_SIZE_TYPE			/* in */	MAX_MESSAGE_SIZE, 
	const MESSAGE_RANGE_TYPE		/* in */	MAX_NB_MESSAGE, 
	const PORT_DIRECTION_TYPE		/* in */	PORT_DIRECTION, 
	const QUEUING_DISCIPLINE_TYPE	/* in */	QUEUING_DISCIPLINE, 
	QUEUING_PORT_ID_TYPE * const	/* out */	QUEUING_PORT_ID, 
	RETURN_CODE_TYPE * const		/* out */	RETURN_CODE)
{

#ifdef POK_NEEDS_QUEUING_PORTS_STUBBED
			*RETURN_CODE = NO_ERROR;
			return;
#endif

	pok_ret_t								core_ret;
	pok_port_direction_t				 core_direction;
	pok_port_queueing_discipline_t	core_discipline;
	pok_port_id_t						  core_id;


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
			return;
	}

	switch (PORT_DIRECTION)
	{
		case SOURCE:
			core_direction = POK_PORT_DIRECTION_OUT;
			break;
		
		case DESTINATION:
			core_direction = POK_PORT_DIRECTION_IN;
			break;

		default:
			*RETURN_CODE = INVALID_PARAM;
			return;
	}

	if (core_direction == POK_PORT_DIRECTION_IN)
	{
#if (POK_CONFIG_PART_NB_INPUT_QUEUING_PORTS > 0)
 #ifdef POK_QD
		printf("\naddr: 0x%x\n",input_buffers_queuing[input_buffers_queuing_index]);
		printf("vaddr: 0x%x\n",(uint32_t)(input_buffers_queuing[input_buffers_queuing_index]));
 #endif
			// in case of input ports the input_buffer is associated to the input token
		core_ret = pok_port_queueing_create (QUEUING_PORT_NAME, MAX_MESSAGE_SIZE, MAX_NB_MESSAGE, core_direction, 
				core_discipline, &core_id, input_buffers_queuing[input_buffers_queuing_index]); 
		input_buffers_queuing_index++;
#endif
	}else{
#ifdef POK_QD
		printf("in create queuing port 2\n"); //ET
#endif
		core_ret = pok_port_queueing_create (QUEUING_PORT_NAME, MAX_MESSAGE_SIZE, MAX_NB_MESSAGE, core_direction,
				 core_discipline, &core_id, NULL); 			
	}
	*QUEUING_PORT_ID	= core_id;
	*RETURN_CODE 		= core_ret;
}


/********************************************************************
* SERVICE  SEND_QUEUING_MESSAGE
********************************************************************/
void SEND_QUEUING_MESSAGE
		(const QUEUING_PORT_ID_TYPE	/* in */	QUEUING_PORT_ID, 
		const MESSAGE_ADDR_TYPE		/* in */	MESSAGE_ADDR, 
		const MESSAGE_SIZE_TYPE		/* in */	LENGTH, 
		const SYSTEM_TIME_TYPE	 	/* in */	TIME_OUT, 
		RETURN_CODE_TYPE * const  	/* out */	RETURN_CODE)
{

#ifdef POK_NEEDS_QUEUING_PORTS_STUBBED
	*RETURN_CODE = NO_ERROR;
	return;
#endif

	pok_ret_t core_ret;

	uint64_t timeout = TIME_OUT / 10;  // expressed in 100 us, converted ms
 
	core_ret = pok_port_queueing_send (QUEUING_PORT_ID, MESSAGE_ADDR, LENGTH, timeout);

	*RETURN_CODE = core_ret;
}


/********************************************************************
* SERVICE  RECEIVE_QUEUING_MESSAGE
********************************************************************/
#ifdef POK_NEEDS_QUEUING_PORTS_STUBBED
void RECEIVE_QUEUING_MESSAGE
		(const QUEUING_PORT_ID_TYPE	/* in */	QUEUING_PORT_ID, 
		const SYSTEM_TIME_TYPE	 	/* in */	TIME_OUT, 
		MESSAGE_ADDR_TYPE 			/* in out */MESSAGE_ADDR, 
		MESSAGE_SIZE_TYPE * const	/* out */	LENGTH, 
		RETURN_CODE_TYPE * const	/* out */	RETURN_CODE)
{
			*RETURN_CODE = NO_ERROR;
			return;
}

#elif (POK_CONFIG_PART_NB_INPUT_QUEUING_PORTS > 0)
/********************************************************************
* SERVICE  RECEIVE_QUEUING_MESSAGE
********************************************************************/
void RECEIVE_QUEUING_MESSAGE
		(const QUEUING_PORT_ID_TYPE	/* in */	QUEUING_PORT_ID, 
		const SYSTEM_TIME_TYPE		/* in */	TIME_OUT, 
		MESSAGE_ADDR_TYPE 			/* in out */	MESSAGE_ADDR, 
		MESSAGE_SIZE_TYPE * const	/* out */	LENGTH, 
		RETURN_CODE_TYPE * const  	/* out */	RETURN_CODE)
{

	pok_ret_t core_ret;

 #ifndef POK_NEEDS_PORTS_QUEUEING_DYNAMIC
	(void) MESSAGE_ADDR;
	MESSAGE_ADDR_TYPE tmp_message_addr;
	uint64_t timeout = TIME_OUT / 10;  // expressed in 100 us, converted ms	
	core_ret = pok_port_queueing_receive (QUEUING_PORT_ID, timeout, POK_CONFIG_MAX_MESSAGE_SIZE, &tmp_message_addr, (pok_port_size_t*)LENGTH);
	*(receiving_addresses[QUEUING_PORT_ID]) =  tmp_message_addr;
	#ifdef POK_QD
 	printf("address in receiving_addresses[%d] 0x%x\n",QUEUING_PORT_ID,receiving_addresses[QUEUING_PORT_ID]);
	printf("address in returned 0x%x\n",tmp_message_addr);
	printf("message in address returned %c\n",((unsigned char *)tmp_message_addr)[0]);
	#endif
 #else
	uint64_t timeout = TIME_OUT / 10;  // expressed in 100 us, converted ms
	core_ret = pok_port_queueing_receive (QUEUING_PORT_ID, timeout, POK_CONFIG_MAX_MESSAGE_SIZE, MESSAGE_ADDR, (pok_port_size_t*)LENGTH);
 #endif

	*RETURN_CODE = core_ret;
}
#endif

/********************************************************************
* SERVICE  GET_QUEUING_PORT_ID
********************************************************************/
void GET_QUEUING_PORT_ID
		(const QUEUING_PORT_NAME_TYPE	/* in */	QUEUING_PORT_NAME, 
		QUEUING_PORT_ID_TYPE * const	/* out */	QUEUING_PORT_ID, 
		RETURN_CODE_TYPE * const	/* out */	RETURN_CODE)
{
#ifdef POK_NEEDS_QUEUING_PORTS_STUBBED
			*RETURN_CODE = NO_ERROR;
			return;
#endif

	(void) QUEUING_PORT_NAME;
	(void) QUEUING_PORT_ID;
	*RETURN_CODE = NOT_AVAILABLE;
}

/********************************************************************
* SERVICE  GET_QUEUING_PORT_STATUS
*********************************************************************/
void GET_QUEUING_PORT_STATUS
		(const QUEUING_PORT_ID_TYPE		/* in */	QUEUING_PORT_ID, 
		QUEUING_PORT_STATUS_TYPE * const/* out */	QUEUING_PORT_STATUS, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE)
{
#ifdef POK_NEEDS_QUEUING_PORTS_STUBBED
			*RETURN_CODE = NO_ERROR;
			return;
#endif
	(void) QUEUING_PORT_ID;
	(void) QUEUING_PORT_STATUS;
	*RETURN_CODE = NOT_AVAILABLE;
}

#endif

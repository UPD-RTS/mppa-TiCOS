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

#ifdef POK_NEEDS_ARINC653_SAMPLING

#include <core/dependencies.h>
#include <types.h>
#include <middleware/port.h>
#include <arinc653/types.h>
#include <arinc653/sampling.h>
#include <libc/string.h>
#ifdef POK_SD
#include <libc/stdio.h>
#endif

#ifndef POK_NEEDS_PORTS_SAMPLING_DYNAMIC
  extern MESSAGE_ADDR_TYPE *receiving_addresses[];
#endif

#if (POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS > 0)
// array of input port buffers
extern unsigned char *input_buffers[]; // the size of the array can vary across partition
uint8_t input_buffers_index=0;
#endif

/********************************************************************
* SERVICE  CREATE_SAMPLING_PORT
*********************************************************************/
void CREATE_SAMPLING_PORT
	(	const SAMPLING_PORT_NAME_TYPE	/* in */	SAMPLING_PORT_NAME,
		const MESSAGE_SIZE_TYPE			/* in */	MAX_MESSAGE_SIZE,
		const PORT_DIRECTION_TYPE		/* in */	PORT_DIRECTION,
		const SYSTEM_TIME_TYPE			/* in */	REFRESH_PERIOD,
		SAMPLING_PORT_ID_TYPE * const	/* out */	SAMPLING_PORT_ID,
		RETURN_CODE_TYPE * const		/* out */	RETURN_CODE)
{
		pok_port_direction_t	core_direction;
		pok_port_id_t			core_id;
		pok_ret_t				core_ret;

		switch (PORT_DIRECTION)
		{
				case SOURCE:
						core_direction = POK_PORT_DIRECTION_OUT;
						break;
				case DESTINATION:
						core_direction = POK_PORT_DIRECTION_IN;
						break;
				default:
						*RETURN_CODE = NO_ERROR;
						*SAMPLING_PORT_ID = 0xEEEEEEEE;
						return;
		}

		if (core_direction == POK_PORT_DIRECTION_IN)
		{
#ifdef POK_SD
			printf(" POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS %d\n",POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS);
#endif

#if (POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS > 0)
 #ifdef POK_SD
			printf("****[POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS][POK_CONFIG_MAX_MESSAGE_SIZE] %d, %d\n",
				POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS,POK_CONFIG_MAX_MESSAGE_SIZE);
			printf("in CREATE message %c\n",(input_buffers[input_buffers_index])[0]);
			printf("input_buffers[input_buffers_index] 0x%x\n",
					(uint32_t)(*input_buffers[input_buffers_index]));
 #endif
			core_ret = pok_port_sampling_create (SAMPLING_PORT_NAME, MAX_MESSAGE_SIZE,
							core_direction, REFRESH_PERIOD, &core_id,
							(unsigned char *)(&input_buffers[input_buffers_index]));
			*(receiving_addresses[core_id]) =  (unsigned char *)(&input_buffers[input_buffers_index]);
			input_buffers_index++;
#endif // (POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS > 0)
		}else{
			core_ret = pok_port_sampling_create (SAMPLING_PORT_NAME, MAX_MESSAGE_SIZE,
								core_direction, REFRESH_PERIOD, &core_id, NULL);
		}

		*SAMPLING_PORT_ID = core_id;
		*RETURN_CODE = core_ret;
}


/********************************************************************
* SERVICE  WRITE_SAMPLING_MESSAGE
*********************************************************************/
void WRITE_SAMPLING_MESSAGE
		(const SAMPLING_PORT_ID_TYPE	/* in */	SAMPLING_PORT_ID,
		const MESSAGE_ADDR_TYPE			/* in */	MESSAGE_ADDR,
		const MESSAGE_SIZE_TYPE			/* in */	LENGTH,
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE)
{
	pok_ret_t core_ret;
	core_ret = pok_port_sampling_write (SAMPLING_PORT_ID, MESSAGE_ADDR, LENGTH);
	//printf ("WRITE SAMPLING MESSAGE ADDRESS: %x", MESSAGE_ADDR[0]);
	*RETURN_CODE = core_ret;
}



/********************************************************************
* SERVICE  READ_SAMPLING_MESSAGE
*********************************************************************/
void READ_SAMPLING_MESSAGE
		(const SAMPLING_PORT_ID_TYPE	/* in */	SAMPLING_PORT_ID,
		MESSAGE_ADDR_TYPE				/* in out */MESSAGE_ADDR,
		MESSAGE_SIZE_TYPE * const		/* out */	LENGTH,
		VALIDITY_TYPE * const	  		/* out */	VALIDITY,
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE)

{
		pok_ret_t 			core_ret;
		pok_port_size_t	 	len = 0;
		bool_t				valid;

#ifdef POK_NEEDS_PORTS_SAMPLING_DYNAMIC
	core_ret = pok_port_sampling_read  (SAMPLING_PORT_ID, MESSAGE_ADDR, &len, &valid);
#else
	(void) MESSAGE_ADDR;

	MESSAGE_ADDR_TYPE tmp_message_addr;
	core_ret = pok_port_sampling_read  (SAMPLING_PORT_ID, &tmp_message_addr, &len, &valid);

  #ifdef POK_SD
	printf("address in receiving_addresses[%d] 0x%x\n",SAMPLING_PORT_ID,receiving_addresses[SAMPLING_PORT_ID]);
	printf("address in returned tmp_message_addr 0x%x\n",tmp_message_addr);
	printf("message in returned address %c\n",((unsigned char *)tmp_message_addr)[0]);
	if (len >1)
	{
		unsigned int j;
		for (j=0; j<len; j++)
		{
			printf(" 0x%x", ((unsigned char *)tmp_message_addr)[j]);
		}
		printf("\n");
	}
  #endif
#endif
	// maxlen and len are the same
	*LENGTH = len;
	*VALIDITY = valid;
	*RETURN_CODE = core_ret;
#ifdef POK_SD
	printf("Exiting from READ_SAMPLING_MESSAGE\n");
#endif

}



/********************************************************************
* SERVICE  GET_SAMPLING_PORT_ID
********************************************************************/
void GET_SAMPLING_PORT_ID
		(const SAMPLING_PORT_NAME_TYPE	/* in */	SAMPLING_PORT_NAME,
		SAMPLING_PORT_ID_TYPE * const	/* out */	SAMPLING_PORT_ID,
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE)

{
	pok_port_id_t		  core_id;

	pok_port_sampling_id (SAMPLING_PORT_NAME, &core_id, NULL);

	*SAMPLING_PORT_ID = core_id;
	*RETURN_CODE = NO_ERROR;
}


/********************************************************************
* SERVICE  GET_SAMPLING_PORT_STATUS
********************************************************************/
void GET_SAMPLING_PORT_STATUS
		(const SAMPLING_PORT_ID_TYPE		/* in */	SAMPLING_PORT_ID,
		SAMPLING_PORT_STATUS_TYPE * const	/* out */	SAMPLING_PORT_STATUS,
		RETURN_CODE_TYPE * const  			/* out */	RETURN_CODE)

{
	(void) SAMPLING_PORT_ID;
	(void) SAMPLING_PORT_STATUS;
	*RETURN_CODE = NOT_AVAILABLE;
}

#endif // POK_NEEDS_ARINC653_SAMPLING

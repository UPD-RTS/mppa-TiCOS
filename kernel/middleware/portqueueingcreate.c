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


#ifdef POK_NEEDS_PORTS_QUEUEING

#include <errno.h>
#include <types.h>
#include <libc.h>
#include <core/lockobj.h>
#include <middleware/port.h>
#include <middleware/queue.h>
#include <core/partition.h>

extern uint8_t		 pok_current_partition;

extern pok_port_t	 pok_ports[POK_CONFIG_NB_PORTS];


pok_ret_t pok_port_queueing_create (	char*					name,
					const	pok_port_size_t		max_message_size,
				 	const	pok_range_t			max_nb_messages,
					const pok_port_direction_t	direction,
					const pok_port_queueing_discipline_t	discipline,
					pok_port_id_t*				id,
					void *						partition_level_buffer)
{
	pok_ret_t	ret;

#ifdef POK_QD
	printf("in kernel queiing create\n");
#endif

	if (discipline != POK_PORT_QUEUEING_DISCIPLINE_FIFO)
	{
		return POK_ERRNO_DISCIPLINE;
	}

	if(max_nb_messages > POK_CONFIG_MAX_QUEUING_MESSAGES)
	{
		return POK_ERRNO_EINVAL;
	}

	ret = pok_port_create (name, max_message_size*max_nb_messages, direction, POK_PORT_KIND_QUEUEING, id);

#ifdef POK_QD
	printf("Create result: %d, POK_ERRNO_EXISTS d ", ret, POK_ERRNO_EXISTS);
	printf("Max_nb_messages : %d\n",max_nb_messages);
#endif

	pok_ports[*id].max_nb_messages = max_nb_messages; 
	pok_ports[*id].max_message_size = max_message_size; 

	if (ret == POK_ERRNO_OK)
	{

		if (direction == POK_PORT_DIRECTION_IN)
		{

#ifdef POK_QD
			printf("direction %d\n",direction);
			printf("virtual address of partition_level_buffer 0x%x\n",(unsigned char *)partition_level_buffer);
			printf("virtual address of partition_level_buffer - integer 0x%x\n",(uint32_t)partition_level_buffer);
#endif	
			uint8_t t;	
			uint32_t base_addr = pok_partitions[pok_current_partition].base_addr;
			uint32_t base_buffer_phisical_address =  (uint32_t)partition_level_buffer + base_addr;

#ifdef POK_QD
			printf("base addr 0x%x\n",base_addr);
			printf("base_buffer_phisical_address  - integer 0x%x\n", base_buffer_phisical_address);
#endif
			uint32_t phisical_address_of_inner_buffer;
			uint32_t virtual_address_of_inner_buffer;

			// save in every token the virtual and phisical address of the input buffer
			for (t=0; t < max_nb_messages /*POK_CONFIG_MAX_QUEUING_MESSAGES*/;t++)
			{
				virtual_address_of_inner_buffer = *((uint32_t *)(base_buffer_phisical_address + (t*sizeof(unsigned char *))));
				phisical_address_of_inner_buffer =  virtual_address_of_inner_buffer + base_addr;
	
#ifdef POK_QD
				printf("in pok_port_queueing_create - n.of token %i\n",t);
				printf("virtual_address_of_inner_buffer  - integer 0x%x\n", virtual_address_of_inner_buffer);
				printf("phisical_address_of_inner_buffer - integer 0x%x\n", phisical_address_of_inner_buffer);
#endif
				pok_ports[*id].tokens[t].msg_slot = (unsigned char *)phisical_address_of_inner_buffer;
				pok_ports[*id].tokens[t].msg_slot_vaddr = virtual_address_of_inner_buffer;
				pok_ports[*id].tokens[t].empty = TRUE; 
				pok_ports[*id].tokens[t].size = 0;	
			}
		}
		return POK_ERRNO_OK;
	} //POK_PORT_DIRECTION_IN

	if (ret != POK_ERRNO_OK)
	{
		return ret;
	}

	pok_ports[*id].discipline = discipline;

	return ret;
}

#endif

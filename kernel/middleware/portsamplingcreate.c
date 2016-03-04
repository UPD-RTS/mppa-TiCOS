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


#ifdef POK_NEEDS_PORTS_SAMPLING

#include <errno.h>
#include <types.h>
#include <core/lockobj.h>
#include <middleware/port.h>
#include <middleware/queue.h>
#include <libc.h>
#include <core/partition.h>

extern uint8_t		 pok_current_partition;

extern pok_port_t	 pok_ports[POK_CONFIG_NB_PORTS];

pok_ret_t pok_port_sampling_create  (	char*				name, 
					const pok_port_size_t		max_message_size, 
					const pok_port_direction_t	direction,
					const uint64_t			refresh,
					pok_port_id_t*			id,
					void*				partition_level_buffer)
{
	pok_ret_t ret;

#ifdef POK_SD
		printf("\n");
		// direction is an enum
		if (direction){
			printf ("Dir: IN (dest)\n");
			printf("port id %d\n",*id);
		} else {
			printf ("Dir: OUT (source)\n");
		}
#endif
	ret = pok_port_create (name, max_message_size, direction, POK_PORT_KIND_SAMPLING, id);

	if (ret != POK_ERRNO_OK)
	{
		return ret;
	}

	pok_ports[*id].refresh		= refresh;
	pok_ports[*id].last_receive	= 0;
	pok_ports[*id].max_message_size	= max_message_size;

	if (direction == POK_PORT_DIRECTION_IN){
		uint32_t base_addr = pok_partitions[pok_current_partition].base_addr;
		uint32_t base_buffer_phisical_address =  (uint32_t)partition_level_buffer + base_addr;
#ifdef POK_SD
		printf("partition_level_buffer 0x%x\n",(unsigned char *)partition_level_buffer);
		printf("phisicall address 0x%x\n",base_buffer_phisical_address);
#endif
		pok_ports[*id].token.msg_slot = (unsigned char *)base_buffer_phisical_address;
		pok_ports[*id].token.msg_slot_vaddr = (uint32_t)partition_level_buffer;	

#ifdef POK_SD				
		printf("token address 0x%x\n",(unsigned char *)pok_ports[*id].token.msg_slot);
		printf("token virtual address 0x%x\n",(unsigned char *)pok_ports[*id].token.msg_slot_vaddr);
#endif

		pok_ports[*id].token.empty = TRUE; 
		pok_ports[*id].token. size = 0;	
	}
	return ret;
}

#endif // POK_NEEDS_PORTS_SAMPLING

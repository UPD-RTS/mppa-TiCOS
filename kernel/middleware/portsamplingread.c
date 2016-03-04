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
#include <libc.h>
#include <core/lockobj.h>
#include <core/sched.h>
#include <core/time.h>
#include <middleware/port.h>
#include <middleware/queue.h>

extern pok_port_t	 pok_ports[POK_CONFIG_NB_PORTS];

// read the message in the queue
pok_ret_t pok_port_sampling_read (	const 			pok_port_id_t id, 
					void*				data, 
					pok_port_size_t*	len,
					bool_t*				valid)
{
	pok_ret_t ret;
	
	if (data == NULL)
	{
		return POK_ERRNO_EINVAL;
	}

	if (id > POK_CONFIG_NB_PORTS)
	{
		return POK_ERRNO_EINVAL;
	}

	if (! pok_own_port (POK_SCHED_CURRENT_PARTITION, id))
	{
		return POK_ERRNO_PORT;
	}

	if (pok_ports[id].ready != TRUE)
	{
		return POK_ERRNO_PORT;
	}

	if (pok_ports[id].kind != POK_PORT_KIND_SAMPLING)
	{
		return POK_ERRNO_EINVAL;
	}

	if (pok_ports[id].direction != POK_PORT_DIRECTION_IN)
	{
		return POK_ERRNO_MODE;
	}

	pok_port_size_t size = 0;

// Sampling port semantics: always return the last value
 #ifdef POK_SD 
 	printf("pok_ports[%i].token.empty->%i\n", id, pok_ports[id].token.empty);
 #endif
	if (pok_ports[id].token.empty)
	{	
		ret = POK_ERRNO_EMPTY;
	} else {
		// Data is already there
		// data = (void*) pok_ports[id].token.msg_slot;
#ifdef POK_SD 
		printf("before memcpy\n");	
		printf("message addr in data addr 0x%x\n",((unsigned char *)*(unsigned char **)data));
		printf("in pok_ports[id].token.msg_slot[0] %c\n",((unsigned char *)pok_ports[id].token.msg_slot)[0]);
		printf("token address 0x%x\n",(unsigned char *)pok_ports[id].token.msg_slot);
		printf("token virtual address 0x%x\n",(unsigned char *)pok_ports[id].token.msg_slot_vaddr);
#endif
		// update the data pointer to the input buffer
		*(unsigned char **)data = (unsigned char *)pok_ports[id].token.msg_slot_vaddr;
 #ifdef POK_SD 
		printf("after memcpy\n");
		printf("message addr in data addr 0x%x\n",((unsigned char *)*(unsigned char **)data));
		printf("in pok_ports[id].token.msg_slot %c or 0x%x\n",
				((unsigned char *)pok_ports[id].token.msg_slot)[0],
				((unsigned char *)pok_ports[id].token.msg_slot)[0]);
			
		if (pok_ports[id].token.size >1)
		{
			unsigned int j;
			for (j=0; j<pok_ports[id].token.size; j++)
			{
				printf(" 0x%x", ((unsigned char *)pok_ports[id].token.msg_slot)[j]);
			}
			printf("\n");					
		}
 #endif

 #ifdef POK_SD 
		printf("in READ SAMPLING size: %d\n",pok_ports[id].token.size);
 #endif
		// Update size
		size = pok_ports[id].token.size;
		ret = POK_ERRNO_OK;
	}

	if (ret == POK_ERRNO_EMPTY)
	{
		*len = 0;
		*valid = 0;
		return POK_ERRNO_NOACTION; 
	}
	else
	{
		*len = size;
		*valid = TRUE;
 #ifdef POK_SD 
		printf("return from sampling read POK_ERRNO_OK, size=%d\n",*len);
 #endif
		return POK_ERRNO_OK;
	}
	return ret;
}

#endif


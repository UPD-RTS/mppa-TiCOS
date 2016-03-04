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

#include "dependencies.h"
#include <types.h>
#include <errno.h>
#include <libc.h>

#include <core/lockobj.h>
#include <core/time.h>
#include <core/sched.h>

#include <middleware/port.h>
#include <middleware/queue.h>


extern pok_port_t	 pok_ports[POK_CONFIG_NB_PORTS];

// receives all data present in the queue
pok_ret_t pok_port_queueing_receive (	const pok_port_id_t	id, 
				 	uint64_t		timeout, 
					const pok_port_size_t	maxlen, 
					void*			data, 
					pok_port_size_t*	len)
{

	(void) timeout;

#ifdef POK_QD
	printf("In kernel pok_port_queueing_receive\n");
#endif

	pok_port_size_t	clen;
	pok_port_size_t	rlen;
	pok_ret_t ret;

	clen = 0 ;
	rlen = 0 ;

	if (data == NULL)
	{
		return POK_ERRNO_EINVAL;
	}
	
	if (maxlen <= 0)
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

	if (pok_ports[id].kind != POK_PORT_KIND_QUEUEING)
	{
		return POK_ERRNO_KIND;
	}

	if (pok_ports[id].direction != POK_PORT_DIRECTION_IN)
	{
		return POK_ERRNO_DIRECTION;
	}

 #ifdef POK_QD
	printf("All tokens are empty? %d\n",pok_ports[id].all_tokens_empty);
 #endif
	if (pok_ports[id].all_tokens_empty)
	{		
		ret = POK_ERRNO_EMPTY;
	} 
	else
	{		
		// Data is already there
 #ifdef POK_QD
		printf("Token address 0x%x\n",pok_ports[id].tokens[pok_ports[id].first_not_empty].msg_slot);
		printf("Token v address 0x%x\n",pok_ports[id].tokens[ pok_ports[id].first_not_empty ].msg_slot_vaddr);
		printf("Message size in token %d \n",pok_ports[id].tokens[pok_ports[id].first_not_empty].size);
		printf("Message in pok_ports[id].tokens[pok_ports[id].first_not_empty].msg_slot[0] %c\n",
						pok_ports[id].tokens[ pok_ports[id].first_not_empty ].msg_slot[0]); 
		printf("Data addr (unsigned char **)data: 0x%x\n",(unsigned char **)data);		
		printf("Message addr in data addr 0x%x\n",((unsigned char *)*(unsigned char **)data));
 #endif							
			
		// change the address
		*(unsigned char **)data = (unsigned char *)pok_ports[id].tokens[ pok_ports[id].first_not_empty ].msg_slot_vaddr;

		// Update the message size 
		*len = pok_ports[id].tokens[pok_ports[id].first_not_empty].size;

		pok_ports[id].all_tokens_full = FALSE;
		pok_ports[id].tokens[pok_ports[id].first_not_empty].empty = TRUE;
		pok_ports[id].tokens[pok_ports[id].first_not_empty].size = 0;
		pok_ports[id].first_not_empty	= (pok_ports[id].first_not_empty +1) % pok_ports[id].max_nb_messages;

		if (pok_ports[id].first_not_empty == pok_ports[id].first_empty){
			// all tokens are empty
			pok_ports[id].first_not_empty = -1;
			pok_ports[id].first_empty = 0;
			pok_ports[id].all_tokens_empty = TRUE;
		}		

 #ifdef POK_QD 
		printf("after address copy \n");
		printf("message size %d \n",*len);
		printf("data addr: 0x%x\n",(unsigned char *)data);
		printf("message addr in data addr 0x%x\n",((unsigned char *)*(unsigned char **)data));
 #endif
	
		ret = POK_ERRNO_OK;
	}
	//pok_arch_cache_read_HID0 ();

	return ret;
}

#endif // POK_NEEDS_PORTS_QUEUEING

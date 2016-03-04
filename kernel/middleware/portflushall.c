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


/**
 * \file	 middleware/portflushall.c
 * \brief	Flush the ports and send the data of IN ports to OUT ports
 */

#if defined (POK_NEEDS_PORTS_QUEUEING) || defined (POK_NEEDS_PORTS_SAMPLING)

#include <types.h>
#include <libc.h>
#include <core/partition.h>
#include <middleware/port.h>
#include <middleware/queue.h>
#include <arch.h>

extern uint8_t	pok_ports_nb_destinations[POK_CONFIG_NB_PORTS];				/*  from deployment.c  */
extern uint8_t	pok_ports_nb_ports_by_partition[POK_CONFIG_NB_PARTITIONS];	/*  from deployment.c  */
extern uint8_t*	pok_ports_destinations[POK_CONFIG_NB_PORTS];				/*  from deployment.c  */
extern uint8_t*	pok_ports_by_partition[POK_CONFIG_NB_PARTITIONS];			/*  from deployment.c  */
extern uint8_t	pok_global_ports_to_local_ports[POK_CONFIG_NB_GLOBAL_PORTS];/*  from deployment.c  */

 #ifdef POK_NEEDS_PORTS_SLOT
extern uint8_t*	pok_outputports_to_flush[POK_CONFIG_SCHEDULING_NBSLOTS];
extern uint8_t 	pok_nb_outputports_to_flush[POK_CONFIG_SCHEDULING_NBSLOTS];
extern uint8_t* pok_inputports_to_preload[POK_CONFIG_SCHEDULING_NBSLOTS];
extern uint8_t	pok_nb_inputports_to_preload[POK_CONFIG_SCHEDULING_NBSLOTS];
 #endif 

/* #ifdef POK_TEST
extern unsigned int		pok_ports_message_size[POK_CONFIG_NB_PORTS];
extern unsigned char	pok_ports_messages[POK_CONFIG_NB_PORTS][512];
 #endif 
*/
extern pok_queue_t	pok_queue;
extern pok_port_t	pok_ports[POK_CONFIG_NB_PORTS];										

uint8_t				pok_buffer_flush[POK_PORT_MAX_SIZE];

/** 
	Posted WRITE (SAMPLING & QUEUING PORTS implementation)
*/
void pok_slot_write_flush (uint8_t pid)
{  

#if defined(POK_SD) || defined(POK_QD)
	printf("partition: %d, Slot post write flush...................\n",pid);
#endif 
	uint8_t part_portid;
	pok_port_t port;

#ifdef POK_NEEDS_PORTS_SLOT
	uint8_t part_nb_ports = pok_nb_outputports_to_flush[pid];
#else
	uint8_t part_nb_ports = pok_ports_nb_ports_by_partition[pid];
#endif

	uint8_t p;
#if defined(POK_SD) || defined(POK_QD)
	printf("nb of ports to flush: %d\n",part_nb_ports);
#endif
	for (p=0; p < part_nb_ports; p++)
	{
#ifdef POK_NEEDS_PORTS_SLOT
		part_portid = pok_outputports_to_flush[pid][p];
#else
		part_portid = pok_ports_by_partition[pid][p];
#endif

#if defined(POK_SD) || defined(POK_QD)
		printf("\nPort to flush: %d\n",part_portid);
#endif
		port = pok_ports[part_portid];

#ifdef POK_NEEDS_PORTS_SAMPLING
		// Check this is a SAMPLING source port
		if ((port.kind == POK_PORT_KIND_SAMPLING && port.direction == POK_PORT_DIRECTION_OUT) && !port.token.empty)
		{
 #ifdef POK_SD 
			printf(" SAMPLING PORT; message to flush\n");
			printf("	port[%i]-(size?%i) -addr: 0x%x  msg_slot[0] %c or 0x%x\n", part_portid, (int)port.token.size, 
  				port.token.msg_slot,port.token.msg_slot[0],(unsigned char)(port.token.msg_slot[0]));
			int ret_tmp1 = 
 #endif
			// Flush the local (partition) msg to the kernle port  
			pok_port_write(part_portid,port.token.msg_slot,(int)port.token.size);
			pok_ports[part_portid].token.size = 0;
 			pok_ports[part_portid].token.empty = TRUE;
 #ifdef POK_SD 
			printf("pok_port_write result: %i\n",ret_tmp1); 
 #endif
		} // if sampling
#endif // POK_NEEDS_PORTS_SAMPLING

#ifdef POK_NEEDS_PORTS_QUEUEING
//#ifndef POK_TEST
  #ifdef POK_QD 
		if ((port.kind == POK_PORT_KIND_QUEUEING && port.direction == POK_PORT_DIRECTION_OUT))
		{
			printf("QUEUING PORT;\n");
			printf ("	port %d, all tokens empty?  %d  \n",part_portid,port.all_tokens_empty);
		}
  #endif
		// Check this is a QUEUEING source port
		if ((port.kind == POK_PORT_KIND_QUEUEING && port.direction == POK_PORT_DIRECTION_OUT) && !port.all_tokens_empty
		/*port.nb_initialized_tokens > 0*/ )
		{
			pok_ports[part_portid].empty = FALSE;
			uint8_t nb_full_tokens = 0;
  #ifdef POK_QD 
			printf ("	port.first_empty %d\n",port.first_empty);
			printf ("	port.first_not_empty %d\n",port.first_not_empty);
  #endif
			if (port.first_empty > port.first_not_empty)
			{
				nb_full_tokens = port.first_empty - port.first_not_empty;
			}
			else
			{
				nb_full_tokens = port.first_empty + (port.max_nb_messages/*POK_CONFIG_MAX_QUEUING_MESSAGES*/ - port.first_not_empty);
			} 
  #ifdef POK_QD 
			printf ("	nb_full_tokens %d\n",nb_full_tokens);
  #endif
			uint8_t m;
			bool_t port_full = FALSE;
			for (m=0; (m<nb_full_tokens) && (!port_full); m++)
			{
  #ifdef POK_QD 
 				printf("TOKEN position %d, message size %d\n",pok_ports[part_portid].first_not_empty,
					pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].size);
 				printf("	pok_port_available_size %d\n",pok_port_available_size(part_portid));
				printf("	message to flush: %c\n",
				pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].msg_slot[0]);
  #endif						
				if (pok_port_available_size(part_portid) >=  
						pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].size)
				{
					// Flush the local (partition) msg to the port
  #ifdef POK_QD
					int ret_write = 
  #endif
					pok_port_write (part_portid, 
					(void*)pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].msg_slot, 
					pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].size);
  #ifdef POK_QD 
				
					printf(" 	write result %d\n",ret_write);
					printf(" 	message flush in kernel buffer port[%i]-(size?%i) -addr: 0x%x", part_portid, 
							(int)pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].size, 
							pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].msg_slot);
					printf ("	Memory: %c\n", pok_queue.data[pok_ports[part_portid].index]);
					printf (" 	Memory: %c\n", pok_queue.data[pok_ports[part_portid].index+1]);
					printf (" 	Memory: %c\n", pok_queue.data[pok_ports[part_portid].index+2]);
  #endif
 					pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].empty = TRUE;
					pok_ports[part_portid].tokens[pok_ports[part_portid].first_not_empty].size = 0;				
					pok_ports[part_portid].first_not_empty = (pok_ports[part_portid].first_not_empty +1) % pok_ports[part_portid].max_nb_messages; //POK_CONFIG_MAX_QUEUING_MESSAGES
					pok_ports[part_portid].must_be_flushed = TRUE;
  #ifdef POK_QD
					printf("	port[%d].must_be_flushed = TRUE\n",part_portid); 
  #endif							
					pok_ports[part_portid].all_tokens_full = FALSE;
	
				}
				else
				{
					port_full = TRUE;
				}
			} // for messages

			if (pok_ports[part_portid].first_not_empty == pok_ports[part_portid].first_empty)
			{
				// all tokens empty
				pok_ports[part_portid].first_not_empty 	= -1;
				pok_ports[part_portid].first_empty 		= 0;
				pok_ports[part_portid].all_tokens_empty = TRUE;
			}		

			//pok_ports[part_portid].nb_initialized_tokens = 0;		
		} // if queuing port
// #endif // POK_TEST
#endif // POK_NEEDS_PORTS_QUEUEING
	}
	printf(" \n");
 
}

/** 
	Prefetch READ (SAMPLING PORTS implementation)
*/
void pok_slot_read_prefetch (uint8_t pid)
{

#if defined(POK_SD) || defined(POK_QD)
	printf("partition: %d, Slot read prefetch...........................\n",pid);
#endif

	uint8_t part_portid;
	pok_port_t port;
	// Number of ports in partition "pid"
#ifdef POK_NEEDS_PORTS_SLOT
	uint8_t part_nb_ports = pok_nb_inputports_to_preload[pid];
#else
	uint8_t part_nb_ports = pok_ports_nb_ports_by_partition[pid];
#endif

#if defined(POK_SD) || defined(POK_QD)
	printf("	nb of ports to preload: %d\n",part_nb_ports);
#endif
	uint8_t p;
	for (p=0; p<part_nb_ports;p++)
	{
#ifdef POK_NEEDS_PORTS_SLOT
		part_portid = pok_inputports_to_preload[pid][p]; 
#else
		part_portid = pok_ports_by_partition[pid][p];
#endif
		port = pok_ports[part_portid];
	
#if defined(POK_SD) || defined(POK_QD)
		printf("port_id: %d, port kind: %d, port direction: %d, port empty: %d\n",
				part_portid, port.kind, port.direction, port.empty);
#endif
		
#ifdef POK_NEEDS_PORTS_SAMPLING
		// Check this is a SAMPLING destination port
		if ((port.kind == POK_PORT_KIND_SAMPLING && port.direction == POK_PORT_DIRECTION_IN) )
		{
 #ifdef POK_SD 
			printf("input sampling port\n");	 
 #endif			
			if (!port.empty)
			{
				// Flush the port data into the local (partition) slot
				// memcpy ((void*)port.token.msg_slot, &pok_queue.data[port.index + port.off_b], port.off_e);
 #ifdef POK_SD 
				printf("PORT is not empty\n");
				int ret_tmp = 
 #endif
				pok_port_get_message (part_portid, (void *)pok_ports[part_portid].token.msg_slot, 
					&(pok_ports[part_portid].token.size));
					pok_ports[part_portid].token.empty = FALSE;
					
 #ifdef POK_SD 
				printf("	message read from port	*[%i]- (size?%i) - data:%c, result %i\n "
						,part_portid, pok_ports[part_portid].token.size,pok_ports[part_portid].token.msg_slot[0],ret_tmp);
				if (pok_ports[part_portid].token.size >1)
				{
					unsigned int j;
					for (j=0; j<pok_ports[part_portid].token.size; j++)
					{
						printf(" 0x%x", ((unsigned char *)pok_ports[part_portid].token.msg_slot)[j]);
					}
					printf("\n");		
 				}
 #endif
			}
 /* 
 #ifdef POK_TEST
			else  // sampling port is empty
 			{	
  #ifdef POK_SD 
				printf("PORT is empty\n");
				unsigned int j;
				printf("pok_ports_messages:\n");
				for (j=0; j<pok_ports_message_size[part_portid]; j++)
				{
					printf(" 0x%x", pok_ports_messages[part_portid][j]);
				}
				printf("\n");

  #endif
				memcpy((void *)(pok_ports[part_portid].token.msg_slot),&(pok_ports_messages[part_portid]),
							pok_ports_message_size[part_portid]);
				pok_ports[part_portid].token.size = pok_ports_message_size[part_portid];
				//port.token.empty = FALSE;
				pok_ports[part_portid].token.empty = FALSE;
  #ifdef POK_SD 
				printf("	PORT EMPTY -- message read from port	*[%i]- (size?%i) - data:%c, 0x%x \n ",
				part_portid, pok_ports[part_portid].token.size,	pok_ports[part_portid].token.msg_slot[0], pok_ports[part_portid].token.msg_slot[0]);
  				
				printf("pok_ports[part_portid].token.msg_slot\n");
				for (j=0; j<pok_ports[part_portid].token.size; j++)
				{
					printf(" 0x%x", ((unsigned char *)pok_ports[part_portid].token.msg_slot)[j]);
				}	

  #endif
			}
 #endif
 */

 #ifdef POK_SD
			printf("msg address 0x%x\n",port.token.msg_slot);
			printf("msg virtual address 0x%x\n",port.token.msg_slot_vaddr);
 #endif
									
			// Update token size to the msg size 
			// port.token.size = port.off_e;
		} // if sampling
#endif 

#ifdef POK_NEEDS_PORTS_QUEUEING
 //#ifndef POK_TEST
  #ifdef POK_QD 
		if ((port.kind == POK_PORT_KIND_QUEUEING && port.direction == POK_PORT_DIRECTION_IN))
		{
			printf("	number of port.nb_messages %d in port %d \n",port.nb_messages,part_portid);
			//printf(" *[%i]-(size?%i) - data:%d ",part_portid, port.off_e, 
			//		*((int *)pok_queue.data[port.index + port.off_b]));
		}
  #endif
		if ( (port.kind == POK_PORT_KIND_QUEUEING && port.direction == POK_PORT_DIRECTION_IN) 
				&& (port.nb_messages > 0) && !port.all_tokens_full ) 
		{

			uint8_t m;
						
			for (m=0; (m<port.nb_messages) && (!pok_ports[part_portid].all_tokens_full);m++)
			{
				pok_port_get_message (part_portid, 
					(void *)pok_ports[part_portid].tokens[pok_ports[part_portid].first_empty].msg_slot, 
					&(pok_ports[part_portid].tokens[pok_ports[part_portid].first_empty].size));

  #ifdef POK_QD
				printf("	put message [%c], size %d, from port %d, in token %d\n",
				pok_ports[part_portid].tokens[pok_ports[part_portid].first_empty].msg_slot[0],
				pok_ports[part_portid].tokens[pok_ports[part_portid].first_empty].size, part_portid,
								pok_ports[part_portid].first_empty); 
  #endif							
				pok_ports[part_portid].tokens[pok_ports[part_portid].first_empty].empty = FALSE;
				if (pok_ports[part_portid].all_tokens_empty)
				{
					pok_ports[part_portid].all_tokens_empty = FALSE;
					pok_ports[part_portid].first_not_empty = 0;
				}
							
				pok_ports[part_portid].first_empty = 
					(pok_ports[part_portid].first_empty +1) % pok_ports[part_portid].max_nb_messages;//POK_CONFIG_MAX_QUEUING_MESSAGES;
								
  #ifdef POK_QD
				printf("	pok_ports[part_portid].all_tokens_empty = %d\n",
					pok_ports[part_portid].all_tokens_empty);
  #endif						

				if (pok_ports[part_portid].first_empty == pok_ports[part_portid].first_not_empty)
				{
					pok_ports[part_portid].all_tokens_full  = TRUE;	
				}
  #ifdef POK_QD			
				printf("	pok_ports[part_portid].all_tokens_full = %d\n",
					pok_ports[part_portid].all_tokens_full);
  #endif
			}// for queue messages 
		} // if queuing
// #endif  // POK_TEST
#endif  // POK_NEEDS_PORTS_QUEUEING
	} // for ports
	printf(" \n");
}


void pok_port_flush_partition (uint8_t pid)
{
	uint8_t nb;
	uint8_t local;
	uint8_t i;
	uint8_t j;
	uint8_t local_dest;		/* recipient port, global id */
	uint8_t global_dest;	/* recipient port, local id  */

#ifdef POK_NEEDS_PORTS_SAMPLING		
	pok_port_size_t len;
	uint8_t ndest;
#endif


#if defined(POK_SD) || defined(POK_QD) 
	printf ("\n\nFlushing partition ...................\n");
#endif
	nb = pok_ports_nb_ports_by_partition[pid];

	for (i = 0 ; i < nb ; i++)
	{
		local = pok_ports_by_partition[pid][i];
#if defined(POK_SD) || defined(POK_QD) 
		printf ("Flushing port: %i\n", local);
#endif
		if (pok_ports[local].direction != POK_PORT_DIRECTION_OUT)
		{
#if defined(POK_SD) || defined(POK_QD)
			printf("	INPUT port: no flush\n");
#endif
			continue;
		}

		if (pok_ports[local].empty == TRUE)  
		{
#if defined(POK_SD) || defined(POK_QD) 
			printf("	EMPTY PORT\n");
#endif
			continue;
		}

		if (pok_ports[local].must_be_flushed == FALSE)
		{
#if defined(POK_SD) || defined(POK_QD) 
			printf("	NOT TO BE FLUSHED\n");
#endif
			continue;
		}


#ifdef POK_NEEDS_PORTS_SAMPLING			
		if (pok_ports[local].kind == POK_PORT_KIND_SAMPLING)
		{

 #if defined(POK_SD) 
			printf("SAMPLING PORT \n");
 #endif	
			len = pok_port_consumed_size (local);

			// move all messages from the output port to the pok_buffer_flush
			if (pok_port_get (local, pok_buffer_flush, len) != POK_ERRNO_OK)
			{
 #if defined(POK_SD)  
				printf("	ERROR GETTING MESSAGEs FROM THE OUTPUT PORT\n");
 #endif
				continue;

			}else{
 #if defined(POK_SD)  
				printf("	moving the message from the output port to the pok_buffer_flush ");
 #endif
			}
	
			ndest = pok_ports_nb_destinations[local];
 #ifdef POK_SD
			printf ("of port [");
 #endif
			for (j=0 ; j < ndest ; j++)
			{
				global_dest = pok_ports_destinations[local][j];

				local_dest = pok_global_ports_to_local_ports[global_dest];
 #ifdef POK_SD 
				printf ("%i,", local_dest);
 #endif
				if (pok_ports[local_dest].ready != TRUE)
				{
 #ifdef POK_SD 
					printf("NOT READY \n ");
 #endif
					continue;
				}

				if (pok_ports[local_dest].direction != POK_PORT_DIRECTION_IN)
				{
 #ifdef POK_SD 
					printf("OUTPUT PORT\n ");
 #endif
					continue;
				}

				pok_port_write (local_dest, pok_buffer_flush, len);
				pok_ports[local_dest].token.empty = FALSE;
				pok_ports[local_dest].token.size = len;

			} // for
		} // if sampling
#endif  // sampling

#ifdef POK_NEEDS_PORTS_QUEUEING
// #ifndef POK_TEST
		if (pok_ports[local].kind == POK_PORT_KIND_QUEUEING){
	
			// in case of queuing port one source port correspond to one destination port (no multicast allowed)
			uint16_t nb_messages = pok_ports[local].nb_messages;

			global_dest = pok_ports_destinations[local][0];

			local_dest = pok_global_ports_to_local_ports[global_dest];
  #ifdef POK_QD
			printf("\n	number of messages to flush  %d\n",nb_messages);
			printf ("	local dest %i\n", local_dest);
  #endif
			if (pok_ports[local_dest].ready != TRUE)
			{
  #ifdef POK_QD 
				printf("	@4 ");
  #endif
				continue;
			}

			if (pok_ports[local_dest].direction != POK_PORT_DIRECTION_IN)
			{
  #ifdef POK_QD 
				printf("	@5 ");
  #endif
				continue;
			}

			uint32_t size = 0;		
			for (j=0 ; j < nb_messages ; j++)
			{
				// move all messages from the output port to the pok_buffer_flush and then to the destination port
				int ret_tmp = pok_port_get_message (local, (void *)pok_buffer_flush, &size);
  #ifdef POK_QD 
				printf("get result, POK_ERRNO_OK: %d,%d\n", ret_tmp,POK_ERRNO_OK);
  #endif
				if ( ret_tmp!= POK_ERRNO_OK)
				{
  #ifdef POK_QD 
					printf("	@4 ");
  #endif
					continue;
				}
				
				// in case of queuing port only destination is possibile
  #ifdef POK_QD
 				printf("	message[%c] of size %d get from the port %d\n",((unsigned char *)pok_buffer_flush)[0]
							,size,local);
				printf("	dest port %d full? %d\n",local_dest,pok_ports[pid].full); 
				printf("	pok_port_available_size(local_dest:%d) : %d, size  %d\n",
					local_dest, pok_port_available_size(local_dest),size);
  #endif		
				// Check if the message can be written on local dest
				if (pok_port_available_size(local_dest) >= size)
				{
					pok_port_write (local_dest, pok_buffer_flush, size);
  #ifdef POK_QD
					printf("	message written[%c] to the the port %d\n",
						((unsigned char *)pok_buffer_flush)[0],local_dest); 
  #endif		
				}
				
			}

			pok_ports[local].must_be_flushed = FALSE;
  #ifdef POK_QD
			printf("port[%d].must_be_flushed = FALSE\n",local); 
  #endif		

		} // if quieuing port
// #endif // POK_TEST
#endif // queueing

#ifdef POK_SD
		printf ("]\n");
#endif

	}
}


/**
 * Flush all the ports, write all OUT ports to their destinations.
 * This function is called at each major frame
 */
void pok_port_flushall (void)
{
//printf("In flushall.........");
  
	uint8_t p;
	for (p = 0 ; p < POK_CONFIG_NB_PARTITIONS ; p++)
	{
		if ((pok_partitions[p].mode == POK_PARTITION_MODE_NORMAL) || (pok_partitions[p].mode == POK_PARTITION_MODE_IDLE))
		{
			pok_port_flush_partition (p);
		}
	}

}
#endif

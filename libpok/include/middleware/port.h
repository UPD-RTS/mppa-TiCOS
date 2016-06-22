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
 *              Copyright (c) 2007-2009 POK team
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


#ifndef __POK_LIBPOK_PORTS_H__
#define __POK_LIBPOK_PORTS_H__

#include <core/dependencies.h>

#include <types.h>
#include <errno.h>
#include <core/syscall.h>

typedef enum
{
	POK_PORT_QUEUEING_DISCIPLINE_FIFO       = 1,
	POK_PORT_QUEUEING_DISCIPLINE_PRIORITY   = 2
} pok_port_queueing_disciplines_t;

typedef enum
{
	// ARINC source port
	POK_PORT_DIRECTION_OUT  = 0,
	// ARINC destination port
	POK_PORT_DIRECTION_IN   = 1
} pok_port_directions_t;

typedef pok_queueing_discipline_t pok_port_queueing_discipline_t;

typedef enum
{
	POK_PORT_KIND_QUEUEING  = 1,
	POK_PORT_KIND_SAMPLING  = 2,
//	POK_PORT_KIND_VIRTUAL	= 2,
	POK_PORT_KIND_INVALID   = 10
} pok_port_kinds_t;


#ifdef POK_NEEDS_PORTS_QUEUEING
/* Queueing port functions */
/* NOT USED at the moment  */
/* typedef struct
   {
        pok_port_size_t		size;
        pok_port_direction_t direction;
        uint8_t				  nb_messages;
        uint8_t				  waiting_processes;
   }pok_port_queueing_status_t;
 */


pok_ret_t pok_port_queueing_create (const char *name,
				const pok_port_size_t size,
				const pok_range_t max_nb_messages,
				const pok_port_direction_t direction,
				const pok_port_queueing_discipline_t discipline,
				pok_port_id_t *id,
				void *partition_input_buffers
				);

pok_ret_t pok_port_queueing_receive (const pok_port_id_t id,
					const uint64_t timeout,
					const pok_port_size_t maxlen,
					void *data,
					pok_port_size_t *len);

pok_ret_t pok_port_queueing_send (const pok_port_id_t id,
					const void *data,
					const pok_port_size_t len,
					const uint64_t timeout);
/*
   NOT USED at the moment
   #define pok_port_queueing_status(id,status) \
                  pok_syscall2(POK_SYSCALL_MIDDLEWARE_QUEUEING_STATUS,(uint32_t)id,(uint32_t)status)
 */

/*NOT USED at the moment
   #define pok_port_queueing_id(name,id) \
                  pok_syscall2(POK_SYSCALL_MIDDLEWARE_QUEUEING_ID,(uint32_t)name,(uint32_t)id)

 */
#endif

#ifdef POK_NEEDS_PORTS_SAMPLING
/* Sampling port functions */
/*NOT USED at the moment
   typedef struct
   {
        pok_port_size_t		size;
        pok_port_direction_t direction;
        uint64_t				 refresh;
        bool_t					validity;
   }pok_port_sampling_status_t;
 */

pok_ret_t pok_port_sampling_create (const char *name,
					const pok_port_size_t size,
					const pok_port_direction_t direction,
					const uint64_t refresh,
					pok_port_id_t *id,
					void *partition_input_buffers);

pok_ret_t pok_port_sampling_write (const pok_port_id_t id,
					const void *data,
					const pok_port_size_t len);

pok_ret_t pok_port_sampling_read (const pok_port_id_t id,
					void *message,
					pok_port_size_t *len,
					bool_t *valid);


pok_ret_t pok_port_sampling_id    (const char *name,
					pok_port_id_t *id,
					const pok_port_direction_t direction);

/* NOT USED at the moment
#define pok_port_sampling_status(id,status) \
	pok_syscall2(POK_SYSCALL_MIDDLEWARE_SAMPLING_STATUS,(uint32_t)id,(uint32_t)status)
*/
#endif

#endif /* __POK_LIBPOK_PORTS_H__ */

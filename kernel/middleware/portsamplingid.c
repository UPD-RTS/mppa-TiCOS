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
#include <types.h>
#include <libc.h>
#include <core/sched.h>
#include <middleware/port.h>


extern char*	pok_ports_names[POK_CONFIG_NB_PORTS];
extern uint8_t	pok_ports_kind[POK_CONFIG_NB_PORTS];


/* return POK_ERRNO_OK only in case the element IS found */
pok_ret_t pok_port_sampling_id (char* name, pok_port_id_t* id)
{
	uint8_t i;

	for (i = 0; i < POK_CONFIG_NB_PORTS ; i++)
	{
		if ( (strcmp (name, pok_ports_names[i]) == 0) && (pok_ports_kind[i] == POK_PORT_KIND_SAMPLING))
		{
 #ifdef POK_SD
			printf ("in pok_port_sampling_id -	partition: %u \n",POK_SCHED_CURRENT_PARTITION);
			printf ("in pok_port_sampling_id -	port: %u\n", i);
 #endif
			if (! pok_own_port (POK_SCHED_CURRENT_PARTITION, i))
			{
				return POK_ERRNO_PORT;
			}
		
			*id = i;
 #ifdef POK_SD
			printf(" (i=%i) ",i);
 #endif
			return POK_ERRNO_OK;
		}
	}
 #ifdef POK_SD
		printf (" (not found) ");
 #endif
	return POK_ERRNO_NOTFOUND;
}

#endif

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
 */



#ifndef __POK_KERNEL_SIZES_H__
#define __POK_KERNEL_SIZES_H__

#ifdef POK_NEEDS_PORTS_QUEUEING

#include <types.h>

typedef struct
{
	uint32_t	sizes[POK_CONFIG_MAX_QUEUING_MESSAGES];
	int16_t		first;  //in the input port points to the first valid item to be read
	//uint16_t	last; 	// in the output port points to the last item used, 
} pok_msg_sizes_t;

#endif

#endif

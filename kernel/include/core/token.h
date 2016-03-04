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


#ifndef __POK_KERNEL_TOKEN_H__
#define __POK_KERNEL_TOKEN_H__

#if (defined POK_NEEDS_PORTS_SAMPLING) || (POK_NEEDS_PORTS_QUEUEING) 

#include <types.h>

typedef struct
{
		pok_bool_t  empty;
		uint32_t		msg_slot;
		uint32_t		size;
} pok_token_t;

#endif

#endif

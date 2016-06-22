/*
 * Copyright (c) 2012  University of Padua, Department of Mathematics
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

#include <core/dependencies.h>

#ifdef POK_NEEDS_PORTS_SAMPLING

#include <errno.h>
#include <types.h>
#include <core/syscall.h>
#include <middleware/port.h>

pok_ret_t pok_port_sampling_id (const char *name,
				pok_port_id_t *id,
				const pok_port_direction_t direction)
{
	return (pok_syscall3 (POK_SYSCALL_MIDDLEWARE_SAMPLING_ID,
				(uint32_t) name,
				(uint32_t) id,
				(uint32_t) direction));
}

#endif

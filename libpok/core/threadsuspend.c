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

#ifdef POK_NEEDS_THREADS

#include <arch.h>
#include <types.h>
#include <core/syscall.h>
#include <core/thread.h>


# if defined (POK_NEEDS_ARINC653)
pok_ret_t pok_thread_suspend_other (uint32_t thread_id)
{
	return pok_syscall2  (POK_SYSCALL_THREAD_SUSPEND_OTHER, (uint32_t)&thread_id, 0);
}

#endif
#endif

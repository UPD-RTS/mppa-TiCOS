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
#ifdef POK_ARCH_PPC

#include "soclib.h"

void exit(int level)
{

#ifdef POK_NEEDS_QEMU_SETUP
	(void) level;
	while (1);
#else

#define SIMHELPER_BASE                  0xD0800000
#define SIMHELPER_END_WITH_RETVAL       1

	soclib_io_set(
		base(SIMHELPER),
		SIMHELPER_END_WITH_RETVAL,
		level);
	trap();
	while (1);
#endif
}

#else //POK_ARCH_MPPA

#include <libc.h>
#include <reent.h>
#include <unistd.h>
#include "atexit.h"

void
_DEFUN (exit, (code),
	int code)
{
  __call_exitprocs (code, NULL);

  if (_GLOBAL_REENT->__cleanup)
    (*_GLOBAL_REENT->__cleanup) (_GLOBAL_REENT);
  _exit (code);
}

#endif

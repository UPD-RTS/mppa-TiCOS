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
 *                                      Copyright (c) 2007-2009 POK team 
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


#include <dependencies.h>
#include <errno.h>


#include <libc.h>
#include <core/debug.h>
#include <core/cons.h>

#ifdef POK_NEEDS_QEMU_SETUP
#include "ioports.h"
#include "cons.h"
#else
#include <soclib.h>
#endif


#if defined (POK_NEEDS_CONSOLE) || defined (POK_NEEDS_DEBUG)


 #ifdef POK_NEEDS_QEMU_SETUP
#define  COM1      0x3F8

static int is_transmit_empty(void)
{
   return inb(COM1 + 5) & 0x20;
// *((volatile unsigned char *)(POK_PREP_IOBASE + port))
}

static void write_serial(char a)
{
   while (is_transmit_empty() == 0)
     ;

   outb(COM1,a);
}

 #endif // POK_NEEDS_QEMU_SETUP



pok_bool_t pok_cons_write (const char *s, size_t length)
{
   for (; length > 0; length--)
 #ifdef POK_NEEDS_QEMU_SETUP
		write_serial (*s++);
 #else
		putchar((int)(*s++));	
 #endif
   return 0;
}

 #ifdef POK_NEEDS_QEMU_SETUP
int pok_cons_init (void)
{
	outb(COM1 + 1, 0x00);    // Disable all interrupts
	outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outb(COM1 + 1, 0x00);    //                  (hi byte)
	outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set

	pok_print_init (write_serial, NULL);

	return 0;
}
 #endif

#endif



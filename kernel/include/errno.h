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


#ifndef __POK_ERRNO_H__
#define __POK_ERRNO_H__

typedef enum
{
	 POK_ERRNO_OK			=	0,
	 POK_ERRNO_EINVAL		=	1,
	 POK_ERRNO_UNAVAILABLE	=	2,
	 POK_ERRNO_TOOMANY		=	5,
	 POK_ERRNO_EPERM		=	6,
	 POK_ERRNO_EXISTS		=	7,

	 POK_ERRNO_ERANGE		=	8,
	 POK_ERRNO_EDOM			=	9,
	 POK_ERRNO_HUGE_VAL		=  10,

	 POK_ERRNO_EFAULT		=  11,

	 POK_ERRNO_THREAD		=  49,
	 POK_ERRNO_THREADATTR	=  50,

	 POK_ERRNO_TIME			=  100,

	 POK_ERRNO_PARTITION_ATTR	=  200,

	 POK_ERRNO_TIMEOUT		=  250,
	 POK_ERRNO_MODE			=  251,
	 
	 POK_ERRNO_PORT			=  301,
	 POK_ERRNO_NOTFOUND		=  302,
	 POK_ERRNO_DIRECTION	=  303,
	 POK_ERRNO_SIZE			=  304,
	 POK_ERRNO_DISCIPLINE	=  305,
	 POK_ERRNO_PORTPART		=  307,
	 POK_ERRNO_EMPTY		=  308,
	 POK_ERRNO_KIND			=  309,
	 POK_ERRNO_FULL			=  311,
	 POK_ERRNO_READY		=  310,

	 POK_ERRNO_PARTITION	=  401,
 
	 POK_ERRNO_LOCKOBJ_UNAVAILABLE  = 500,
	 POK_ERRNO_LOCKOBJ_NOTREADY	= 501,
	 POK_ERRNO_LOCKOBJ_KIND		= 502,
	 POK_ERRNO_LOCKOBJ_POLICY	= 503,

	 POK_ERRNO_PARTITION_MODE	=  601,

#if defined (POK_NEEDS_ARINC653)
	POK_ERRNO_NOACTION			=  701
#endif 

} pok_ret_t;

#endif

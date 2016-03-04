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


#ifndef __POK_KERNEL_DEPENDENCIES_H__
#define __POK_KERNEL_DEPENDENCIES_H__


#ifdef POK_SD
  #ifndef POK_NEEDS_CONSOLE
		#define POK_NEEDS_CONSOLE	 1
  #endif
#endif

#ifdef POK_QD
  #ifndef POK_NEEDS_CONSOLE
		#define POK_NEEDS_CONSOLE	 1
  #endif
#endif

#ifdef POK_NEEDS_DEBUG
  #ifndef POK_NEEDS_CONSOLE
		#define POK_NEEDS_CONSOLE	 1
	#endif
#endif


#ifdef POK_NEEDS_ERROR_HANDLING

  #ifndef POK_NEEDS_CONSOLE
		#define POK_NEEDS_CONSOLE	 1
	#endif
#endif

#ifdef POK_NEEDS_CONSOLE
	#ifndef POK_NEEDS_PROARTIS_PRINTF
		#define POK_NEEDS_PROARTIS_PRINTF 1
	#endif
#endif


#ifdef POK_NEEDS_PORTS_SAMPLING_DYNAMIC
	#ifndef POK_NEEDS_PORTS_SAMPLING
		#define POK_NEEDS_PORTS_SAMPLING
	#endif
#endif

/*#ifdef POK_NEEDS_PORTS_QUEUEING_DYNAMIC
	#ifndef POK_NEEDS_PORTS_QUEUEING
		#define POK_NEEDS_PORTS_QUEUEING
	#endif
#endif
*/
#endif

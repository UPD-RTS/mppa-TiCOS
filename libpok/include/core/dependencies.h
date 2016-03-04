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

#ifndef __LIBPOK_DEPENDENCIES_H__
#define __LIBPOK_DEPENDENCIES_H__


#if defined(POK_NEEDS_ARINC653) 
	#ifndef POK_NEEDS_ARINC653_PROCESS
		#define POK_NEEDS_ARINC653_PROCESS	1
	#endif
	#ifndef POK_NEEDS_ARINC653_PARTITION
		#define POK_NEEDS_ARINC653_PARTITION	1
	#endif
#endif

#ifdef	POK_NEEDS_ARINC653_TIME
		#ifndef POK_NEEDS_ARINC653
			#define POK_NEEDS_ARINC653	1
		#endif
#endif

#ifdef POK_NEEDS_ARINC653_PROCESS
		#ifndef POK_NEEDS_ARINC653
			#define POK_NEEDS_ARINC653	1
		#endif
#endif


#ifdef POK_NEEDS_ARINC653_PARTITION
	#ifndef POK_NEEDS_ARINC653
		#define POK_NEEDS_ARINC653	1
	#endif
#endif

#ifdef POK_NEEDS_ARINC653_QUEUEING
		#ifndef POK_NEEDS_ARINC653	
			#define POK_NEEDS_ARINC653	1
		#endif
	#ifndef POK_CONFIG_PART_NB_INPUT_QUEUING_PORTS
		#define POK_CONFIG_PART_NB_INPUT_QUEUING_PORTS 0
	#endif
#endif

#ifdef POK_NEEDS_ARINC653_SAMPLING
		#ifndef POK_NEEDS_ARINC653
			#define POK_NEEDS_ARINC653	1
		#endif
	#ifndef POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS
		#define POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS 0
	#endif
#endif

#ifdef POK_NEEDS_ARINC653_EVENT
	#ifndef POK_NEEDS_ARINC653
		#define POK_NEEDS_ARINC653	1
	#endif
#endif

#ifdef POK_NEEDS_ARINC653_BUFFER
	#ifndef POK_NEEDS_ARINC653
		#define POK_NEEDS_ARINC653	1
	#endif
#endif

#ifdef POK_NEEDS_ARINC653_BLACKBOARD
	
	#ifndef POK_NEEDS_BLACKBOARDS
		#define POK_NEEDS_BLACKBOARDS	1
	#endif
		#ifndef POK_NEEDS_ARINC653
			#define POK_NEEDS_ARINC653	1
		#endif
#endif

#ifdef POK_NEEDS_ARINC653_ERROR
		#ifndef POK_NEEDS_ARINC653
			#define POK_NEEDS_ARINC653	1
		#endif
#endif


#ifdef POK_CONFIG_OPTIMIZE_FOR_GENERATED_CODE

	#ifdef POK_NEEDS_LIBC_STDIO
		#ifndef POK_CONFIG_NEEDS_FUNC_MEMCPY
			#define POK_CONFIG_NEEDS_FUNC_MEMCPY 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_PRINTF
			#define POK_CONFIG_NEEDS_FUNC_PRINTF 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_VPRINTF
			#define POK_CONFIG_NEEDS_FUNC_VPRINTF 1
		#endif
	#endif

	#ifdef POK_NEEDS_LIBC_STDLIB
		#ifndef POK_CONFIG_NEEDS_FUNC_MEMCPY
			#define POK_CONFIG_NEEDS_FUNC_MEMCPY 1
		#endif

//	#ifndef POK_NEEDS_ALLOCATOR
//	#define POK_NEEDS_ALLOCATOR						1
//	#endif
	
//	#ifndef POK_CONFIG_NEEDS_FUNC_RAND
//	#define POK_CONFIG_NEEDS_FUNC_RAND			  1
//	#endif

//	#ifndef POK_CONFIG_NEEDS_FUNC_MALLOC
//	#define POK_CONFIG_NEEDS_FUNC_MALLOC			1
//	#endif

//	#ifndef POK_CONFIG_NEEDS_FUNC_CALLOC
//	#define POK_CONFIG_NEEDS_FUNC_CALLOC			1
//	#endif

//	#ifndef POK_CONFIG_NEEDS_FUNC_FREE
//	#define POK_CONFIG_NEEDS_FUNC_FREE			  1
//	#endif
	#endif /* POK_NEEDS_STDLIB */

	#ifdef POK_NEEDS_STRING
		#ifndef POK_CONFIG_NEEDS_FUNC_MEMCPY
			#define POK_CONFIG_NEEDS_FUNC_MEMCPY			1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_MEMSET
			#define POK_CONFIG_NEEDS_FUNC_MEMSET			1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_MEMCMP
			#define POK_CONFIG_NEEDS_FUNC_MEMCMP			1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRCMP
			#define POK_CONFIG_NEEDS_FUNC_STRCMP			1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRNCMP
			#define POK_CONFIG_NEEDS_FUNC_STRNCMP		  1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRCPY
			#define POK_CONFIG_NEEDS_FUNC_STRCPY			1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRNCPY
			#define POK_CONFIG_NEEDS_FUNC_STRNCPY		  1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN			1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STREQ
			#define POK_CONFIG_NEEDS_FUNC_STREQ			 1
		#endif
	#endif /* POK_NEEDS_STRING */

	#ifdef	POK_NEEDS_ARINC653_TIME
		#ifndef POK_NEEDS_FUNCTION_TIME_GET
			#define POK_NEEDS_FUNCTION_TIME_GET 1 
		#endif	
	#endif

	#ifdef POK_NEEDS_ARINC653_PROCESS
		#define POK_NEEDS_THREADS 1
		#ifndef POK_CONFIG_NEEDS_FUNC_STRCPY
			#define POK_CONFIG_NEEDS_FUNC_STRCPY		  1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRNCMP
			#define POK_CONFIG_NEEDS_FUNC_STRNCMP		  1
		#endif
	
		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
	 		#define POK_CONFIG_NEEDS_FUNC_STRLEN			1
		#endif
	#endif

	#ifdef POK_NEEDS_ARINC653_PARTITION
		#define POK_NEEDS_PARTITIONS 1
	#endif

	#ifdef POK_NEEDS_ARINC653_QUEUEING
		#define POK_NEEDS_PORTS_QUEUEING 1
		#ifndef POK_CONFIG_NEEDS_FUNC_MEMSET
			#define POK_CONFIG_NEEDS_FUNC_MEMSET 1
		#endif
	#endif

	#ifdef POK_NEEDS_ARINC653_SAMPLING
		#define POK_NEEDS_PORTS_SAMPLING 1
		#ifndef POK_CONFIG_NEEDS_FUNC_MEMSET
			#define POK_CONFIG_NEEDS_FUNC_MEMSET 1
		#endif  
	#endif

	#ifdef POK_NEEDS_ARINC653_EVENT
		#ifndef POK_CONFIG_NEEDS_FUNC_STRNCMP
			#define POK_CONFIG_NEEDS_FUNC_STRNCMP 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN 1
		#endif
	#endif


	#ifdef POK_NEEDS_ARINC653_BUFFER

		#ifndef POK_NEEDS_BUFFERS
			#define POK_NEEDS_BUFFERS 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STREQ
			#define POK_CONFIG_NEEDS_FUNC_STREQ 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN 1
		#endif
	#endif

	#ifdef POK_NEEDS_ARINC653_BLACKBOARD

		#ifndef POK_NEEDS_BLACKBOARDS
			#define POK_NEEDS_BLACKBOARDS 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STREQ
			#define POK_CONFIG_NEEDS_FUNC_STREQ 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN 1
		#endif
	#endif


	#ifdef POK_NEEDS_ARINC653_ERROR
		#ifndef POK_NEEDS_ERROR_HANDLING
			#define POK_NEEDS_ERROR_HANDLING 	1
		#endif

		#ifndef POK_NEEDS_FUNC_MEMCPY
			#define POK_CONFIG_NEEDS_FUNC_MEMCPY 1
		#endif
	#endif

	#ifdef 	POK_NEEDS_ERROR_HANDLING

		#ifndef POK_NEEDS_FUNCTION_TIME_GETTICK
			#define POK_NEEDS_FUNCTION_TIME_GETTICK	1
		#endif

	#endif


	#ifdef POK_NEEDS_EVENTS
		#ifndef POK_NEEDS_FUNCTION_TIME_GETTICK
			#define POK_NEEDS_FUNCTION_TIME_GETTICK	1
		#endif
	#endif

	#ifdef POK_NEEDS_BUFFERS
		#ifndef POK_NEEDS_FUNCTION_TIME_GETTICK
			#define POK_NEEDS_FUNCTION_TIME_GETTICK	1
		#endif

		#ifndef POK_NEEDS_FUNC_MEMCPY
			#define POK_CONFIG_NEEDS_FUNC_MEMCPY 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STREQ
			#define POK_CONFIG_NEEDS_FUNC_STREQ 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN 1
		#endif
	#endif

	#ifdef POK_NEEDS_BLACKBOARDS
		#ifndef POK_NEEDS_FUNCTION_TIME_GETTICK
			#define POK_NEEDS_FUNCTION_TIME_GETTICK 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STREQ
			#define POK_CONFIG_NEEDS_FUNC_STREQ 1
		#endif

		#ifndef POK_CONFIG_NEEDS_FUNC_STRLEN
			#define POK_CONFIG_NEEDS_FUNC_STRLEN 1
		#endif

		#ifndef POK_NEEDS_FUNC_MEMCPY
			#define POK_CONFIG_NEEDS_FUNC_MEMCPY 1
		#endif
	#endif

	#ifdef POK_NEEDS_TIME
		#ifndef POK_NEEDS_FUNCTION_TIME_GETTICK
			#define POK_NEEDS_FUNCTION_TIME_GETTICK 1
		#endif
	#endif
#endif /* POK_CONFIG_OPTIMIZE_FOR_GENERATED_CODE */

/*************************************************/

/*	
*	Currently the macro POK_CONFIG_OPTIMIZE_FOR_GENERATED_CODE is supposed to be always defined
* 	in the cpu/Makefile
*/
/*
#ifndef POK_CONFIG_OPTIMIZE_FOR_GENERATED_CODE

	// core functions 
	#define POK_NEEDS_FUNCTION_TIME_GETTICK		1
	#define POK_NEEDS_SEMAPHORES				1
	#define POK_NEEDS_THREADS					1
	#define POK_NEEDS_PORTS_QUEUEING		 	1
	#define POK_NEEDS_PARTITIONS				1
	#define POK_NEEDS_BUFFERS					1
	#define POK_NEEDS_BLACKBOARDS			 	1
	#define POK_NEEDS_SEMAPHORES				1

	// string.h functions 
	#define POK_CONFIG_NEEDS_FUNC_MEMCPY			1
	#define POK_CONFIG_NEEDS_FUNC_MEMSET			1
	#define POK_CONFIG_NEEDS_FUNC_MEMCMP			1
	#define POK_CONFIG_NEEDS_FUNC_STRCMP			1
	#define POK_CONFIG_NEEDS_FUNC_STRNCMP		 	1
	#define POK_CONFIG_NEEDS_FUNC_STRCPY			1
	#define POK_CONFIG_NEEDS_FUNC_STRNCPY		 	1
	#define POK_CONFIG_NEEDS_FUNC_STRLEN			1
	#define POK_CONFIG_NEEDS_FUNC_STREQ			 	1
	#define POK_CONFIG_NEEDS_FUNC_ITOA			  	1
	#define POK_CONFIG_NEEDS_FUNC_UDIVDI3		  	1

	// stdlib.h functions 
	#define POK_CONFIG_NEEDS_FUNC_RAND			  	1
	#define POK_CONFIG_NEEDS_FUNC_MALLOC			1
	#define POK_CONFIG_NEEDS_FUNC_CALLOC			1
	#define POK_CONFIG_NEEDS_FUNC_FREE			  	1

#endif*/ /* POK_CONFIG_OPTIMIZE_FOR_GENERATED_CODE */

#endif

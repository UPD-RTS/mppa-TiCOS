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


#ifdef POK_NEEDS_ARINC653_ERROR
#ifndef APEX_ERROR
#define APEX_ERROR

#ifndef POK_NEEDS_ARINC653_PROCESS
#define POK_NEEDS_ARINC653_PROCESS 1
#endif

#include <arinc653/process.h>

#include <arinc653/types.h>

#define  MAX_ERROR_MESSAGE_SIZE			64 
 
typedef  APEX_INTEGER	ERROR_MESSAGE_SIZE_TYPE; 
 
typedef  APEX_BYTE		ERROR_MESSAGE_TYPE[MAX_ERROR_MESSAGE_SIZE]; 
 
enum ERROR_CODE_VALUE_TYPE {
	DEADLINE_MISSED		= 0,
	APPLICATION_ERROR	= 1,
	NUMERIC_ERROR		= 2,
	ILLEGAL_REQUEST		= 3,
	STACK_OVERFLOW		= 4,
	MEMORY_VIOLATION	= 5,
	HARDWARE_FAULT		= 6,
	POWER_FAIL			= 7
};

typedef enum ERROR_CODE_VALUE_TYPE ERROR_CODE_TYPE;
/*------------------------------*/
/* error status type */
/*------------------------------*/
typedef struct{
	ERROR_CODE_TYPE		ERROR_CODE;
	MESSAGE_SIZE_TYPE	LENGTH;
	PROCESS_ID_TYPE		FAILED_PROCESS_ID;
	SYSTEM_ADDRESS_TYPE	FAILED_ADDRESS;
	ERROR_MESSAGE_TYPE	MESSAGE;
} ERROR_STATUS_TYPE;

/* ERROR MANAGEMENT SERVICES */

extern void REPORT_APPLICATION_MESSAGE (MESSAGE_ADDR_TYPE	 MESSAGE,
										MESSAGE_SIZE_TYPE	 LENGTH,
										RETURN_CODE_TYPE	 *RETURN_CODE);

extern void CREATE_ERROR_HANDLER
		(const SYSTEM_ADDRESS_TYPE	/* in */	ENTRY_POINT, 
		const STACK_SIZE_TYPE	 	/* in */	STACK_SIZE, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE);

//extern void CREATE_ERROR_HANDLER (SYSTEM_ADDRESS_TYPE  ENTRY_POINT,
//											 STACK_SIZE_TYPE		STACK_SIZE,
//											 RETURN_CODE_TYPE	  *RETURN_CODE);

extern void GET_ERROR_STATUS
		(ERROR_STATUS_TYPE * const	/* out */	ERROR_STATUS, 
		RETURN_CODE_TYPE * const	/* out */	RETURN_CODE);

//extern void GET_ERROR_STATUS (ERROR_STATUS_TYPE  *ERROR_STATUS,
//										RETURN_CODE_TYPE	*RETURN_CODE );
extern void RAISE_APPLICATION_ERROR
		(const ERROR_CODE_TYPE	 /* in */	ERROR_CODE, 
		const MESSAGE_ADDR_TYPE  /* in */	MESSAGE_ADDR, 
		const MESSAGE_SIZE_TYPE	 /* in */	LENGTH, 
		RETURN_CODE_TYPE * const /* out */	RETURN_CODE);

#endif
#endif 

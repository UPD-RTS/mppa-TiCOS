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

/**
 * \file semaphore.c
 * \brief Provides ARINC653 API functionnalities for semaphore management.
 */

#ifdef POK_NEEDS_ARINC653_SEMAPHORE

#include <types.h>
#include <libc/string.h>
#include <arinc653/types.h>
#include <arinc653/semaphore.h>
#include <core/semaphore.h>

/********************************************************************
* SERVICE  CREATE_SEMAPHORE
*********************************************************************/
void CREATE_SEMAPHORE (SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
							  SEMAPHORE_VALUE_TYPE CURRENT_VALUE,
							  SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE,
							  QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
							  SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
							  RETURN_CODE_TYPE *RETURN_CODE )
{
	// Run-to-completion semantics
	*RETURN_CODE = NO_ERROR;
}

/********************************************************************
* SERVICE  WAIT_SEMAPHORE
*********************************************************************/
void WAIT_SEMAPHORE (SEMAPHORE_ID_TYPE SEMAPHORE_ID,
							SYSTEM_TIME_TYPE TIME_OUT,
							RETURN_CODE_TYPE *RETURN_CODE )
{
	// Run-to-completion semantics
	*RETURN_CODE = NO_ERROR;

}

/********************************************************************
* SERVICE SIGNAL_SEMAPHORE
*********************************************************************/
void SIGNAL_SEMAPHORE (SEMAPHORE_ID_TYPE SEMAPHORE_ID,
							  RETURN_CODE_TYPE *RETURN_CODE )
{
	// Run-to-completion semantics
	*RETURN_CODE = NO_ERROR;
}

/********************************************************************
* SERVICE  GET_SEMAPHORE_ID
*********************************************************************/
void GET_SEMAPHORE_ID (SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
							  SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
							  RETURN_CODE_TYPE *RETURN_CODE )
{
	// Run-to-completion semantics
	*RETURN_CODE = NO_ERROR;
}

/********************************************************************
* SERVICE GET_SEMAPHORE_STATUS 
*********************************************************************/
void GET_SEMAPHORE_STATUS (SEMAPHORE_ID_TYPE SEMAPHORE_ID,
									SEMAPHORE_STATUS_TYPE *SEMAPHORE_STATUS,
									RETURN_CODE_TYPE *RETURN_CODE )
{
	// Run-to-completion semantics
	*RETURN_CODE = NO_ERROR;
}

#endif

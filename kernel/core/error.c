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

#include "dependencies.h"
#include <types.h>
#include <core/thread.h>
#include <core/error.h>
#include <core/debug.h>
#include <core/partition.h>
#include <libc.h>

#ifdef POK_NEEDS_ERROR_HANDLING

/**
 * Declare the error to the recover process.
 * If the recover process is not created, the partition handler
 * is called.
 */
void pok_error_declare (const uint8_t error)
{
	/**
	 * Ok, the error handler process is created inside the partition
	 * so we declare the error in a appropriate structure.
	 */

	if (POK_CURRENT_PARTITION.thread_error != 0)
	{
		POK_CURRENT_PARTITION.error_status.error_kind		= error;
		POK_CURRENT_PARTITION.error_status.failed_thread 	= POK_SCHED_CURRENT_THREAD - POK_CURRENT_PARTITION.thread_index_low;
		POK_CURRENT_PARTITION.error_status.msg_size			= 0;
		
	}
	else
	{
		/**
		 * If the current partition does not have an error handler process,
		 * we raise the error inside the partition.
		 */
		pok_partition_error (POK_SCHED_CURRENT_PARTITION, error);
	}
}


 #ifndef POK_USE_GENERATED_KERNEL_ERROR_HANDLER
void pok_kernel_error (uint32_t error)
{
  #ifdef POK_NEEDS_DEBUG
	printf ("[KERNEL] [WARNING] Error %d was raised by the kernel but no error recovery was set\n", error);
  #else
	(void) error;
  #endif /* POK_NEEDS_DEBUG */
	return;
}
 #endif

 #ifdef POK_NEEDS_PARTITIONS
  #ifndef POK_USE_GENERATED_PARTITION_ERROR_HANDLER
void pok_partition_error (uint8_t partition, uint32_t error)
{
   #ifdef POK_NEEDS_DEBUG
	printf ("[KERNEL] [WARNING] Error %d was raised by partition %d but no error recovery was set\n", error, partition);
   #else
	(void) partition;
	(void) error;
   #endif /* POK_NEEDS_DEBUG */
	return;
}
  #endif /* POK_USE_GENERATED_PARTITION_ERROR_HANDLER */
 #endif /* POK_NEEDS_PARTITIONS */


void pok_error_raise_application_error (
 #if defined (POK_NEEDS_ARINC653) 
	uint8_t error_code,
 #endif
	char* msg, uint32_t msg_size)
{
	if (msg_size > POK_ERROR_MAX_MSG_SIZE)
	{
		msg_size = POK_ERROR_MAX_MSG_SIZE;
	}

	pok_error_status_t* status;
	status						= &pok_partitions[pok_current_partition].error_status;

 # if defined (POK_NEEDS_ARINC653) 
	status->error_kind		= error_code;
 # else
	status->error_kind		= POK_ERROR_KIND_APPLICATION_ERROR;
 # endif
	status->failed_thread	= POK_SCHED_CURRENT_THREAD - POK_CURRENT_PARTITION.thread_index_low;
	status->msg_size		  = msg_size;

	memcpy (status->msg, msg, msg_size);

 # if defined (POK_NEEDS_ARINC653) 
  # ifdef POK_NEEDS_CONSOLE
	printf("ERROR message: %s, ERROR code  %d\n",msg,error_code);
  # endif
 # else
	pok_sched_activate_error_thread ();
 # endif

}

#endif


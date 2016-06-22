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

#ifdef POK_NEEDS_ARINC653_EVENT
#include <arinc653/types.h>
#include <arinc653/event.h>

#include <core/event.h>		/* For core function */
#include <libc/string.h>	/* For strcmp */

#include <errno.h>			/* For POK_ERRNO_... maccros */
#ifdef DEBUG_EVT
#include <libc/stdio.h>
#endif

#define CHECK_EVENTS_INIT if (pok_arinc653_events_initialized == 0) \
	{ \
		uint16_t bla; \
		for (bla = 0; bla < POK_CONFIG_ARINC653_NB_EVENTS; bla++) \
		{ \
			pok_arinc653_events_layers[bla].ready = 0; \
		} \
	} \
	pok_arinc653_events_initialized = 1;


bool_t pok_arinc653_events_initialized = 0;

typedef struct
{
	pok_event_id_t core_id;
	pok_bool_t ready;
} pok_arinc653_event_layer_t;

extern char *pok_arinc653_events_names[POK_CONFIG_ARINC653_NB_EVENTS];
pok_arinc653_event_layer_t pok_arinc653_events_layers[POK_CONFIG_NB_EVENTS];

/********************************************************************
 * SERVICE  CREATE_EVENT
 *********************************************************************/
void CREATE_EVENT (EVENT_NAME_TYPE EVENT_NAME,
                   EVENT_ID_TYPE *EVENT_ID,
                   RETURN_CODE_TYPE *RETURN_CODE)
{
	RETURN_CODE_TYPE return_code_name;
	pok_event_id_t core_id;
	pok_ret_t core_ret;

	*RETURN_CODE = INVALID_CONFIG;

	CHECK_EVENTS_INIT

	GET_EVENT_ID (EVENT_NAME, EVENT_ID, &return_code_name);

	if (return_code_name == INVALID_CONFIG)
	{
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	if (*EVENT_ID >= POK_CONFIG_ARINC653_NB_EVENTS)
	{
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	if (pok_arinc653_events_layers[*EVENT_ID].ready)
	{
		*RETURN_CODE = NO_ACTION;
		return;
	}

	core_ret = pok_event_create (&core_id);
	// ARINC EVENTS are created in a "down" state
	pok_event_lock (core_id);

	/* DEBUG INFO
	   printf("ID=%d\n", core_id);
	 */

	if (core_ret != POK_ERRNO_OK)
	{
		*RETURN_CODE = INVALID_PARAM;
		return;
	}

	pok_arinc653_events_layers[*EVENT_ID].ready = 1;
	pok_arinc653_events_layers[*EVENT_ID].core_id = core_id;
	*RETURN_CODE = NO_ERROR;
}

/********************************************************************
 * SERVICE  SET_EVENT
 *********************************************************************/
void SET_EVENT (EVENT_ID_TYPE EVENT_ID, RETURN_CODE_TYPE *RETURN_CODE)
{
	pok_ret_t core_ret;
	*RETURN_CODE = INVALID_PARAM;
	CHECK_EVENTS_INIT

	if (EVENT_ID >= POK_CONFIG_ARINC653_NB_EVENTS)
	{
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	if (pok_arinc653_events_layers[EVENT_ID].ready == 0)
	{

		*RETURN_CODE = NOT_AVAILABLE;
		return;
	}

	core_ret = pok_event_signal (pok_arinc653_events_layers[EVENT_ID].core_id);

	if (core_ret == POK_ERRNO_OK)
	{
#ifdef DEBUG_EVT
		printf ("Evt->UP\n");
#endif
		*RETURN_CODE = NO_ERROR;
		return;
	}
	else
	{
		*RETURN_CODE = INVALID_PARAM;
		return;
	}
}

/********************************************************************
 * SERVICE  RESET_EVENT
 *********************************************************************/
void RESET_EVENT (EVENT_ID_TYPE EVENT_ID, RETURN_CODE_TYPE *RETURN_CODE)
{
	//(void) EVENT_ID;
	//(void) RETURN_CODE;
	pok_ret_t core_ret;

	*RETURN_CODE = INVALID_PARAM;

	CHECK_EVENTS_INIT

	if (EVENT_ID >= POK_CONFIG_ARINC653_NB_EVENTS)
	{
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	core_ret = pok_event_lock (pok_arinc653_events_layers[EVENT_ID].core_id);

	if (core_ret == POK_ERRNO_OK)
	{
#ifdef DEBUG_EVT
		printf ("Evt->Down\n");
#endif
		*RETURN_CODE = NO_ERROR;
		return;
	}
}

/********************************************************************
 * SERVICE  WAIT_EVENT
 *********************************************************************/
void WAIT_EVENT (EVENT_ID_TYPE EVENT_ID,
			SYSTEM_TIME_TYPE TIME_OUT,
			RETURN_CODE_TYPE *RETURN_CODE)
{
	pok_ret_t core_ret;

	*RETURN_CODE = INVALID_PARAM;

	CHECK_EVENTS_INIT

	if (EVENT_ID >= POK_CONFIG_ARINC653_NB_EVENTS)
	{
		*RETURN_CODE = INVALID_CONFIG;
#ifdef DEBUG_EVT
		printf ("Evt: INVALID CONFIG");
#endif
		return;
	}

	if (pok_arinc653_events_layers[EVENT_ID].ready == 0)
	{
		*RETURN_CODE = NOT_AVAILABLE;
#ifdef DEBUG_EVT
		printf ("Evt: NOT AVAILABLE");
#endif
		return;
	}
#ifdef DEBUG_EVT
	printf ("Evt->Wait");
#endif
	uint64_t timeout = TIME_OUT / 10;  // expressed in 100 us, converted ms
	core_ret = pok_event_wait (pok_arinc653_events_layers[EVENT_ID].core_id, timeout);
#ifdef DEBUG_EVT
	printf ("<-Evt\n");
#endif
	switch (core_ret)
	{
	case POK_ERRNO_OK:
		*RETURN_CODE = NO_ERROR;
		break;

	case POK_ERRNO_TIMEOUT:
		*RETURN_CODE = TIMED_OUT;
		break;

	default:
		*RETURN_CODE = INVALID_PARAM;
		break;
	}
#ifdef DEBUG_EVT
	printf ("Evt WAIT returns: %i", (int)(*RETURN_CODE));
#endif
}

/********************************************************************
 * SERVICE  GET_EVENT_ID
 *********************************************************************/
void GET_EVENT_ID (EVENT_NAME_TYPE EVENT_NAME,
						EVENT_ID_TYPE *EVENT_ID,
						RETURN_CODE_TYPE *RETURN_CODE)
{
	uint16_t i;
	uint16_t len;

	*RETURN_CODE = INVALID_CONFIG;

	len = strlen (EVENT_NAME);

	CHECK_EVENTS_INIT

	for (i = 0; i < POK_CONFIG_ARINC653_NB_EVENTS; i++)
	{
		if (strncmp (EVENT_NAME, pok_arinc653_events_names[i], len) == 0)
		{
			*EVENT_ID = i;
			*RETURN_CODE = NO_ERROR;
			return;
		}
	}
}

/********************************************************************
 * SERVICE  GET_EVENT_STATUS
 *********************************************************************/
void GET_EVENT_STATUS (EVENT_ID_TYPE EVENT_ID,
						EVENT_STATUS_TYPE *EVENT_STATUS,
						RETURN_CODE_TYPE *RETURN_CODE)
{
	(void) EVENT_ID;
	(void) EVENT_STATUS;
	(void) RETURN_CODE;
}

#endif

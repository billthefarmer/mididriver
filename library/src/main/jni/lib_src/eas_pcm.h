/*----------------------------------------------------------------------------
 *
 * File:
 * eas_pcm.h
 *
 * Contents and purpose:
 * External function prototypes for eas_pcm.c module
 *
 *
 * Copyright Sonic Network Inc. 2005

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *----------------------------------------------------------------------------
 * Revision Control:
 *   $Revision: 847 $
 *   $Date: 2007-08-27 21:30:08 -0700 (Mon, 27 Aug 2007) $
 *----------------------------------------------------------------------------
*/

#ifndef _EAS_PCM_H
#define _EAS_PCM_H

/* default gain setting - roughly unity gain */
#define PCM_DEFAULT_GAIN_SETTING    0x6000

typedef struct s_pcm_state_tag *EAS_PCM_HANDLE;
typedef void (*EAS_PCM_CALLBACK) (EAS_DATA_HANDLE pEASData, EAS_VOID_PTR cbInstData, EAS_PCM_HANDLE pcmHandle, EAS_STATE state);

/*----------------------------------------------------------------------------
 * EAS_PEInit()
 *----------------------------------------------------------------------------
 * Purpose:
 * Initializes the PCM engine
 *
 * Inputs:
 *
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_RESULT EAS_PEInit (EAS_DATA_HANDLE pEASData);

/*----------------------------------------------------------------------------
 * EAS_PEShutdown()
 *----------------------------------------------------------------------------
 * Purpose:
 * Shuts down the PCM engine
 *
 * Inputs:
 *
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_RESULT EAS_PEShutdown (EAS_DATA_HANDLE pEASData);

/*----------------------------------------------------------------------------
 * EAS_PERender()
 *----------------------------------------------------------------------------
 * Purpose:
 * Render a buffer of PCM audio
 *
 * Inputs:
 *
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_RESULT EAS_PERender (EAS_DATA_HANDLE pEASData, EAS_I32 numSamples);

#endif /* end _EAS_PCM_H */


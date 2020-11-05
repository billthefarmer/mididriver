/*----------------------------------------------------------------------------
 *
 * File:
 * eas_pcm.c
 *
 * Contents and purpose:
 * Implements the PCM engine including ADPCM decode for SMAF and CMX audio playback.
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
 *   $Revision: 849 $
 *   $Date: 2007-08-28 08:59:11 -0700 (Tue, 28 Aug 2007) $
 *----------------------------------------------------------------------------
*/

#include "eas_data.h"
#include "eas_report.h"
#include "eas_host.h"
#include "eas_config.h"
#include "eas_parser.h"
#include "eas_pcm.h"
#include "eas_math.h"
#include "eas_mixer.h"

#define PCM_MIXER_GUARD_BITS (NUM_MIXER_GUARD_BITS + 1)

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
EAS_RESULT EAS_PEInit (S_EAS_DATA *pEASData)
{
    return EAS_SUCCESS;
}

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
EAS_RESULT EAS_PEShutdown (S_EAS_DATA *pEASData)
{
    return EAS_SUCCESS;
}
